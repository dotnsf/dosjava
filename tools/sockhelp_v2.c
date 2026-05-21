/*
 * sockhelp_v2.c - External Socket Helper for DOS Java VM (Version 2)
 * 
 * This version is based directly on doscurl.cpp to ensure compatibility.
 * Uses the exact same connection logic as doscurl.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

/* mTCP headers */
#include "types.h"
#include "utils.h"
#include "packet.h"
#include "arp.h"
#include "tcp.h"
#include "tcpsockm.h"
#include "udp.h"
#include "dns.h"
#include "timer.h"

/* Configuration */
#define TCP_RECV_BUFFER 4096
#define CONNECT_TIMEOUT 60000UL  /* 60 seconds - use UL suffix for unsigned long */

/* Global state */
static int g_initialized = 0;
static TcpSocket* g_socket = NULL;

/* Ctrl-Break handler */
static volatile uint8_t g_ctrl_break_detected = 0;

void __interrupt __far ctrlBreakHandler(void) {
    g_ctrl_break_detected = 1;
}

static uint8_t userWantsOut(void) {
    if (g_ctrl_break_detected) {
        printf("Ctrl-Break detected - aborting!\n");
        return 1;
    }
    return 0;
}

/*
 * Initialize mTCP stack (same as doscurl)
 */
int init_mtcp(void) {
    int rc;
    
    if (g_initialized) {
        return 0;
    }
    
    printf("Initializing mTCP...\n");
    
    /* Parse mTCP configuration */
    rc = Utils::parseEnv();
    if (rc != 0) {
        printf("Failed to parse mTCP configuration: %d\n", rc);
        printf("Make sure MTCP environment variable is set to your config file\n");
        return -1;
    }
    
    /* Initialize stack with 2 TCP sockets and 4 transmit buffers */
    rc = Utils::initStack(2, 4, ctrlBreakHandler, ctrlBreakHandler);
    if (rc != 0) {
        printf("Failed to initialize TCP/IP stack: %d\n", rc);
        printf("Check packet driver and network configuration\n");
        return -1;
    }
    
    printf("mTCP initialized successfully\n");
    g_initialized = 1;
    return 0;
}

/*
 * Shutdown mTCP stack
 */
void shutdown_mtcp(void) {
    if (g_socket) {
        g_socket->close();
        TcpSocketMgr::freeSocket(g_socket);
        g_socket = NULL;
    }
    
    if (g_initialized) {
        Utils::endStack();
        g_initialized = 0;
    }
}

/*
 * Connect to server (copied from doscurl.cpp connectToServer())
 */
int connect_to_server(IpAddr_t host_addr, uint16_t port) {
    printf("Connecting to server...\n");
    printf("DEBUG: CONNECT_TIMEOUT constant = %d\n", CONNECT_TIMEOUT);
    printf("DEBUG: CONNECT_TIMEOUT as unsigned long = %lu\n", (unsigned long)CONNECT_TIMEOUT);
    
    uint16_t localport = 2048 + rand();
    printf("DEBUG: Local port = %u\n", localport);
    
    g_socket = TcpSocketMgr::getSocket();
    if (g_socket == NULL) {
        printf("failed to get socket\n");
        return -1;
    }
    
    if (g_socket->setRecvBuffer(TCP_RECV_BUFFER)) {
        printf("failed to set buffer\n");
        return -1;
    }
    
    if (g_socket->connectNonBlocking(localport, host_addr, port)) {
        printf("failed to initiate connection\n");
        return -1;
    }
    
    clockTicks_t start = TIMER_GET_CURRENT();
    clockTicks_t timeout_ticks = TIMER_MS_TO_TICKS(CONNECT_TIMEOUT);
    
    printf("DEBUG: start ticks = %lu\n", (unsigned long)start);
    printf("DEBUG: timeout_ticks = %lu\n", (unsigned long)timeout_ticks);
    printf("DEBUG: Expected timeout at ~1092 ticks (60000ms * 18.2 / 1000)\n");
    
    int loop_count = 0;
    while (1) {
        if (userWantsOut()) return -1;
        
        clockTicks_t current = TIMER_GET_CURRENT();
        clockTicks_t elapsed = Timer_diff(start, current);
        
        if ((loop_count % 1000) == 0) {
            printf("DEBUG: Loop %d - elapsed=%lu, timeout=%lu, complete=%d, closed=%d\n",
                   loop_count, (unsigned long)elapsed, (unsigned long)timeout_ticks,
                   g_socket->isConnectComplete(), g_socket->isRemoteClosed());
        }
        loop_count++;
        
        if (elapsed > timeout_ticks) {
            printf("connection timeout after %lu ms (elapsed ticks=%lu, timeout ticks=%lu)\n",
                   (unsigned long)CONNECT_TIMEOUT, (unsigned long)elapsed, (unsigned long)timeout_ticks);
            return -1;
        }
        
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
        
        if (g_socket->isConnectComplete()) {
            break;
        }
        
        // Check for connection errors
        if (g_socket->isRemoteClosed()) {
            printf("connection failed\n");
            return -1;
        }
    }
    
    printf("connected\n");
    return 0;
}

/*
 * Main function
 */
int main(int argc, char* argv[]) {
    FILE* fp;
    int result = -1;
    
    if (argc < 4 || strcmp(argv[1], "connect") != 0) {
        printf("Usage: %s connect <host> <port>\n", argv[0]);
        return 1;
    }
    
    const char* host = argv[2];
    int port = atoi(argv[3]);
    
    /* Initialize mTCP */
    if (init_mtcp() != 0) {
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to initialize mTCP\n");
            fclose(fp);
        }
        return 1;
    }
    
    /* Resolve host (even if it's an IP address, this initializes DNS system) */
    IpAddr_t ip_addr;
    ip_addr[0] = ip_addr[1] = ip_addr[2] = ip_addr[3] = 0;
    
    printf("Resolving host: %s\n", host);
    int8_t rc = Dns::resolve(host, ip_addr, 1);
    
    if (rc < 0) {
        printf("Failed to resolve host: %s\n", host);
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to resolve host\n");
            fclose(fp);
        }
        shutdown_mtcp();
        return 1;
    }
    
    /* If DNS resolution is needed (rc == 1), wait for it */
    if (rc == 1) {
        printf("Waiting for DNS resolution...\n");
        clockTicks_t dns_start = TIMER_GET_CURRENT();
        
        while (!userWantsOut() && Dns::isQueryPending()) {
            if (Timer_diff(dns_start, TIMER_GET_CURRENT()) > TIMER_MS_TO_TICKS(CONNECT_TIMEOUT)) {
                printf("DNS resolution timeout\n");
                fp = fopen("SOCK.OUT", "w");
                if (fp) {
                    fprintf(fp, "STATUS: ERROR\n");
                    fprintf(fp, "MESSAGE: DNS resolution timeout\n");
                    fclose(fp);
                }
                shutdown_mtcp();
                return 1;
            }
            
            PACKET_PROCESS_SINGLE;
            Arp::driveArp();
            Tcp::drivePackets();
        }
        
        if (Dns::isQueryPending()) {
            printf("DNS resolution aborted\n");
            fp = fopen("SOCK.OUT", "w");
            if (fp) {
                fprintf(fp, "STATUS: ERROR\n");
                fprintf(fp, "MESSAGE: DNS resolution aborted\n");
                fclose(fp);
            }
            shutdown_mtcp();
            return 1;
        }
    }
    
    printf("Resolved to: %d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    
    /* Connect to server */
    if (connect_to_server(ip_addr, port) == 0) {
        printf("Connection successful!\n");
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: OK\n");
            fprintf(fp, "SOCKET_ID: 0\n");
            fclose(fp);
        }
        result = 0;
    } else {
        printf("Connection failed\n");
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Connection failed\n");
            fclose(fp);
        }
        result = 1;
    }
    
    /* Cleanup */
    shutdown_mtcp();
    
    return result;
}

// Made with Bob
