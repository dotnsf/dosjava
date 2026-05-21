/*
 * sockhelp.c - External Socket Helper for DOS Java VM
 * 
 * This program runs independently from djvm.exe and handles all socket operations.
 * It has sufficient memory to initialize mTCP properly.
 * 
 * Communication with djvm.exe is done via files:
 * - SOCK.IN: Input commands from djvm
 * - SOCK.OUT: Output results to djvm
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
#define CONNECT_TIMEOUT 60000  /* 60 seconds - increased for debugging */

/* Global state */
static int g_initialized = 0;
static TcpSocket* g_socket = NULL;

/* Ctrl-Break handler - required for proper mTCP operation */
static volatile uint8_t g_ctrl_break_detected = 0;

void __interrupt __far ctrlBreakHandler(void) {
    g_ctrl_break_detected = 1;
}

/* Check if user wants to abort */
static uint8_t userWantsOut(void) {
    if (g_ctrl_break_detected) {
        printf("DEBUG: Ctrl-Break detected - aborting!\n");
        return 1;
    }
    return 0;
}

/*
 * Initialize mTCP stack
 */
int init_mtcp(void) {
    int rc;
    
    if (g_initialized) {
        return 0;
    }
    
    printf("DEBUG: Initializing mTCP...\n");
    
    /* Check available memory before initialization */
    unsigned long freemem = _memavl();
    unsigned long maxblock = _memmax();
    printf("DEBUG: Available memory: %lu bytes, Largest block: %lu bytes\n", freemem, maxblock);
    
    /* Parse mTCP configuration */
    printf("DEBUG: Parsing mTCP configuration...\n");
    {
        char* mtcp_cfg = getenv("MTCPCFG");
        if (mtcp_cfg) {
            printf("DEBUG: MTCPCFG environment variable: %s\n", mtcp_cfg);
        } else {
            printf("DEBUG: MTCPCFG environment variable not set!\n");
        }
    }
    rc = Utils::parseEnv();
    printf("DEBUG: parseEnv() returned: %d\n", rc);
    if (rc != 0) {
        printf("sockhelp: Failed to parse mTCP configuration\n");
        return -1;
    }
    
    /* Check memory after parseEnv */
    freemem = _memavl();
    maxblock = _memmax();
    printf("DEBUG: After parseEnv - Available: %lu bytes, Largest block: %lu bytes\n", freemem, maxblock);
    
    /* Initialize mTCP stack with sufficient resources
     * Args: TCP sockets, TCP transmit buffers, ctrlBreakHandler, ctrlCHandler
     * Use 4 transmit buffers (same as doscurl which works successfully)
     * IMPORTANT: Must provide Ctrl-Break handler for proper operation
     */
    printf("DEBUG: Calling initStack(2, 4, ctrlBreakHandler, ctrlBreakHandler)...\n");
    printf("DEBUG: TCP_MAX_XMIT_BUFS should be >= 4\n");
    rc = Utils::initStack(2, 4, ctrlBreakHandler, ctrlBreakHandler);
    printf("DEBUG: initStack() returned: %d\n", rc);
    if (rc != 0) {
        printf("sockhelp: Failed to initialize mTCP stack\n");
        printf("DEBUG: Check WATTCP.CFG file and TCP_RECV_BUFFER setting\n");
        return -1;
    }
    
    printf("DEBUG: mTCP initialized successfully\n");
    g_initialized = 1;
    return 0;
}

/*
 * Initialize network stack by performing a DNS lookup
 * This ensures the network stack is fully initialized, similar to doscurl
 */
int init_network_stack(void) {
    IpAddr_t resolved_ip;
    
    printf("DEBUG: Initializing network stack via DNS lookup...\n");
    
    /* Try to resolve a non-existent hostname to initialize the network stack */
    /* The resolution will fail, but the process initializes UDP/ARP/network stack */
    /* This mimics doscurl's DNS resolution process without affecting the actual connection */
    int rc = Dns::resolve("_dummy_nonexistent_host_.local", resolved_ip, 1);  /* 1 second timeout */
    
    printf("DEBUG: DNS resolve returned: %d (expected to fail, that's OK)\n", rc);
    
    /* Give the network stack time to process any ARP/network traffic */
    clockTicks_t settle_start = TIMER_GET_CURRENT();
    while (Timer_diff(settle_start, TIMER_GET_CURRENT()) < TIMER_MS_TO_TICKS(500)) {
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
    }
    
    printf("DEBUG: Network stack initialization complete\n");
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
 * Connect to remote host
 * Usage: sockhelp connect <host> <port>
 */
int do_connect(const char* host, int port) {
    FILE* fp;
    uint8_t ip_addr[4];
    uint16_t local_port;
    int rc;
    clockTicks_t start;
    
    /* Initialize mTCP */
    if (init_mtcp() != 0) {
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to initialize mTCP\n");
            fclose(fp);
        }
        return -1;
    }
    
    /* Initialize network stack (similar to doscurl's DNS resolution) */
    init_network_stack();
    
    /* Parse IP address */
    printf("DEBUG: Parsing IP address: %s\n", host);
    if (sscanf(host, "%hhu.%hhu.%hhu.%hhu",
               &ip_addr[0], &ip_addr[1], &ip_addr[2], &ip_addr[3]) != 4) {
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Invalid IP address\n");
            fclose(fp);
        }
        return -1;
    }
    printf("DEBUG: IP address parsed: %d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    
    /* Get socket */
    printf("DEBUG: Getting socket...\n");
    g_socket = TcpSocketMgr::getSocket();
    if (!g_socket) {
        printf("DEBUG: Failed to get socket\n");
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to get socket\n");
            fclose(fp);
        }
        return -1;
    }
    printf("DEBUG: Socket obtained successfully\n");
    
    /* Set receive buffer */
    printf("DEBUG: Setting receive buffer to %d bytes...\n", TCP_RECV_BUFFER);
    if (g_socket->setRecvBuffer(TCP_RECV_BUFFER) != 0) {
        printf("DEBUG: Failed to set receive buffer\n");
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to set receive buffer\n");
            fclose(fp);
        }
        TcpSocketMgr::freeSocket(g_socket);
        g_socket = NULL;
        return -1;
    }
    printf("DEBUG: Receive buffer set successfully\n");
    
    /* Generate random local port */
    srand((unsigned int)time(NULL));
    local_port = 2048 + (rand() % 1000);
    printf("DEBUG: Using local port: %u\n", local_port);
    
    /* Connect non-blocking */
    printf("DEBUG: Initiating non-blocking connection to %d.%d.%d.%d:%d...\n",
           ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3], port);
    
    rc = g_socket->connectNonBlocking(local_port, ip_addr, port);
    if (rc != 0) {
        printf("DEBUG: connectNonBlocking() failed with rc=%d\n", rc);
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Failed to initiate connection\n");
            fclose(fp);
        }
        TcpSocketMgr::freeSocket(g_socket);
        g_socket = NULL;
        return -1;
    }
    printf("DEBUG: Connection initiated, waiting for completion...\n");
    /* Wait for connection - copied from doscurl */
    start = TIMER_GET_CURRENT();
    
    printf("DEBUG: CONNECT_TIMEOUT = %lu ms\n", (unsigned long)CONNECT_TIMEOUT);
    printf("DEBUG: TIMER_MS_TO_TICKS(CONNECT_TIMEOUT) = %lu ticks\n",
           (unsigned long)TIMER_MS_TO_TICKS(CONNECT_TIMEOUT));
    printf("DEBUG: Expected timeout at ~%lu ticks (60000ms * 18.2 / 1000 = ~1092 ticks)\n",
           (unsigned long)(60000UL * 182UL / 10000UL));
    printf("DEBUG: Entering connection wait loop...\n");
    {
    int loop_count = 0;
    clockTicks_t timeout_ticks = TIMER_MS_TO_TICKS(CONNECT_TIMEOUT);
    while (1) {
        clockTicks_t current = TIMER_GET_CURRENT();
        clockTicks_t elapsed = Timer_diff(start, current);
        
        /* Periodic debug output every 1000 iterations */
        if ((loop_count % 1000) == 0) {
            printf("DEBUG: Loop iteration %d, start=%lu, current=%lu, elapsed=%lu, timeout=%lu\n",
                   loop_count, (unsigned long)start, (unsigned long)current,
                   (unsigned long)elapsed, (unsigned long)timeout_ticks);
            printf("DEBUG: Socket state - isConnectComplete=%d, isRemoteClosed=%d\n",
                   g_socket->isConnectComplete(), g_socket->isRemoteClosed());
        }
        loop_count++;
        
        /* Check for user abort */
        if (userWantsOut()) {
            printf("DEBUG: User abort detected\n");
            fp = fopen("SOCK.OUT", "w");
            if (fp) {
                fprintf(fp, "STATUS: ERROR\n");
                fprintf(fp, "MESSAGE: User aborted\n");
                fclose(fp);
            }
            return -1;
        }
        
        /* Check timeout */
        if (elapsed > timeout_ticks) {
            printf("DEBUG: Connection timeout after %d iterations (elapsed=%lu > timeout=%lu)\n",
                   loop_count, (unsigned long)elapsed, (unsigned long)timeout_ticks);
                fp = fopen("SOCK.OUT", "w");
                if (fp) {
                    fprintf(fp, "STATUS: ERROR\n");
                    fprintf(fp, "MESSAGE: Connection timeout\n");
                    fclose(fp);
                }
                return -1;
            }
            
            /* Process packets */
            PACKET_PROCESS_SINGLE;
            Arp::driveArp();
            Tcp::drivePackets();
            
            /* Check if connected */
            if (g_socket->isConnectComplete()) {
                printf("DEBUG: Connection completed successfully after %d iterations!\n", loop_count);
                break;
            }
        
            /* Check for connection errors */
            if (g_socket->isRemoteClosed()) {
                printf("DEBUG: Connection failed after %d iterations\n", loop_count);
                fp = fopen("SOCK.OUT", "w");
                if (fp) {
                    fprintf(fp, "STATUS: ERROR\n");
                    fprintf(fp, "MESSAGE: Connection refused\n");
                    fclose(fp);
                }
                TcpSocketMgr::freeSocket(g_socket);
                g_socket = NULL;
                return -1;
            }
        }
    }
    
    /* Success */
    fp = fopen("SOCK.OUT", "w");
    if (fp) {
        fprintf(fp, "STATUS: OK\n");
        fprintf(fp, "HANDLE: 1\n");
        fprintf(fp, "MESSAGE: Connected to %s:%d\n", host, port);
        fclose(fp);
    }
    
    return 0;
}

/*
 * Send data
 * Usage: sockhelp send <handle> <data>
 */
int do_send_data(int handle, const char* data) {
    FILE* fp_out;
    int len;
    int sent;
    
    (void)handle;  /* Handle not used in this simple implementation */
    
    if (!g_socket) {
        fp_out = fopen("SOCK.OUT", "w");
        if (fp_out) {
            fprintf(fp_out, "STATUS: ERROR\n");
            fprintf(fp_out, "MESSAGE: Not connected\n");
            fclose(fp_out);
        }
        return -1;
    }
    
    len = strlen(data);
    if (len <= 0) {
        fp_out = fopen("SOCK.OUT", "w");
        if (fp_out) {
            fprintf(fp_out, "STATUS: ERROR\n");
            fprintf(fp_out, "MESSAGE: No data to send\n");
            fclose(fp_out);
        }
        return -1;
    }
    
    /* Send data */
    sent = g_socket->send((uint8_t*)data, len);
    
    /* Write result */
    fp_out = fopen("SOCK.OUT", "w");
    if (fp_out) {
        if (sent > 0) {
            fprintf(fp_out, "STATUS: OK\n");
            fprintf(fp_out, "BYTES: %d\n", sent);
        } else {
            fprintf(fp_out, "STATUS: ERROR\n");
            fprintf(fp_out, "MESSAGE: Send failed\n");
        }
        fclose(fp_out);
    }
    
    return (sent > 0) ? 0 : -1;
}

/*
 * Receive data
 * Usage: sockhelp recv <handle>
 */
int do_recv_data(int handle) {
    FILE* fp;
    uint8_t buffer[1024];
    int received;
    clockTicks_t start;
    
    (void)handle;  /* Handle not used in this simple implementation */
    
    if (!g_socket) {
        fp = fopen("SOCK.OUT", "w");
        if (fp) {
            fprintf(fp, "STATUS: ERROR\n");
            fprintf(fp, "MESSAGE: Not connected\n");
            fclose(fp);
        }
        return -1;
    }
    
    /* Wait for data (with timeout) */
    start = TIMER_GET_CURRENT();
    while (1) {
        /* Check timeout (5 seconds) */
        if (Timer_diff(start, TIMER_GET_CURRENT()) > TIMER_MS_TO_TICKS(5000)) {
            fp = fopen("SOCK.OUT", "w");
            if (fp) {
                fprintf(fp, "STATUS: ERROR\n");
                fprintf(fp, "MESSAGE: Receive timeout\n");
                fclose(fp);
            }
            return -1;
        }
        
        /* Process packets */
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
        
        /* Try to receive */
        received = g_socket->recv(buffer, sizeof(buffer));
        if (received > 0) {
            break;
        }
        
        /* Check if connection closed */
        if (g_socket->isRemoteClosed()) {
            fp = fopen("SOCK.OUT", "w");
            if (fp) {
                fprintf(fp, "STATUS: ERROR\n");
                fprintf(fp, "MESSAGE: Connection closed\n");
                fclose(fp);
            }
            return -1;
        }
        
        /* Try to receive data */
        received = g_socket->recv(buffer, sizeof(buffer) - 1);
        if (received > 0) {
            break;
        }
        
        /* Process packets */
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
    }
    
    /* Null-terminate received data */
    buffer[received] = '\0';
    
    /* Write result */
    fp = fopen("SOCK.OUT", "w");
    if (fp) {
        fprintf(fp, "STATUS: OK\n");
        fprintf(fp, "DATA: %s\n", (char*)buffer);
        fclose(fp);
    }
    
    return 0;
}

/*
 * Close connection
 * Usage: sockhelp close <handle>
 */
int do_close_socket(int handle) {
    FILE* fp;
    
    (void)handle;  /* Handle not used in this simple implementation */
    
    if (g_socket) {
        g_socket->close();
        TcpSocketMgr::freeSocket(g_socket);
        g_socket = NULL;
    }
    
    fp = fopen("SOCK.OUT", "w");
    if (fp) {
        fprintf(fp, "STATUS: OK\n");
        fprintf(fp, "MESSAGE: Connection closed\n");
        fclose(fp);
    }
    
    return 0;
}

/*
 * Check socket status
 * Usage: sockhelp status <handle>
 */
int do_status(int handle) {
    FILE* fp;
    int connected;
    
    (void)handle;  /* Handle not used in this simple implementation */
    
    /* Check if socket exists and is not closed */
    connected = (g_socket != NULL && !g_socket->isRemoteClosed());
    
    fp = fopen("SOCK.OUT", "w");
    if (fp) {
        fprintf(fp, "STATUS: OK\n");
        fprintf(fp, "CONNECTED: %d\n", connected ? 1 : 0);
        fclose(fp);
    }
    
    return 0;
}

/*
 * Main function
 */
int main(int argc, char* argv[]) {
    int result = -1;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: sockhelp <command> [args...]\n");
        fprintf(stderr, "Commands:\n");
        fprintf(stderr, "  connect <host> <port>  - Connect to remote host\n");
        fprintf(stderr, "  send <handle> <data>   - Send data through socket\n");
        fprintf(stderr, "  recv <handle>          - Receive data from socket\n");
        fprintf(stderr, "  close <handle>         - Close connection\n");
        fprintf(stderr, "  status <handle>        - Check socket status\n");
        return 1;
    }
    
    /* Execute command */
    if (strcmp(argv[1], "connect") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: sockhelp connect <host> <port>\n");
            return 1;
        }
        result = do_connect(argv[2], atoi(argv[3]));
    }
    else if (strcmp(argv[1], "send") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: sockhelp send <handle> <data>\n");
            return 1;
        }
        result = do_send_data(atoi(argv[2]), argv[3]);
    }
    else if (strcmp(argv[1], "recv") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: sockhelp recv <handle>\n");
            return 1;
        }
        result = do_recv_data(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "close") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: sockhelp close <handle>\n");
            return 1;
        }
        result = do_close_socket(atoi(argv[2]));
    }
    else if (strcmp(argv[1], "status") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: sockhelp status <handle>\n");
            return 1;
        }
        result = do_status(atoi(argv[2]));
    }
    else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }
    
    /* Cleanup - ALWAYS call shutdown_mtcp() to properly cleanup mTCP state */
    printf("DEBUG: Calling shutdown_mtcp()...\n");
    shutdown_mtcp();
    printf("DEBUG: Exiting main() with result=%d\n", result);
    
    return result;
}

// Made with Bob
