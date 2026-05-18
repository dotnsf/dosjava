/*
 * socket.cpp - C-level Socket Wrapper Implementation
 * 
 * Wraps mTCP C++ API for use from C code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "socket.h"

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

/* TCP receive buffer size (same as doscurl) */
#define TCP_RECV_BUFFER 4096

/* Note: TCP_SOCKET_RING_SIZE is defined in GLOBAL.CFG (included via sample.cfg) */
/* We don't redefine it here to avoid conflicts */

/* Socket structure */
typedef struct {
    int type;              /* SOCKET_TYPE_TCP or SOCKET_TYPE_UDP */
    int state;             /* SOCKET_STATE_* */
    TcpSocket* tcp_socket; /* mTCP TCP socket (NULL if UDP) */
    void* udp_socket;      /* mTCP UDP socket (NULL if TCP) - placeholder */
    int in_use;            /* 1 if socket is allocated, 0 if free */
} Socket;

/* Global socket table */
static Socket g_sockets[MAX_SOCKETS];
static int g_initialized = 0;
static char g_error_msg[256];

/* Forward declarations for internal functions */
static socket_handle_t allocate_socket(void);
static void free_socket(socket_handle_t handle);
static int is_valid_handle(socket_handle_t handle);

/*
 * Initialize socket subsystem
 */
int socket_init(void) {
    int i;
    
    if (g_initialized) {
        return SOCKET_OK;
    }
    
    /* Initialize socket table */
    for (i = 0; i < MAX_SOCKETS; i++) {
        g_sockets[i].type = 0;
        g_sockets[i].state = SOCKET_STATE_CLOSED;
        g_sockets[i].tcp_socket = NULL;
        g_sockets[i].udp_socket = NULL;
        g_sockets[i].in_use = 0;
    }
    
    /* Initialize random number generator for local port selection */
    srand((unsigned int)time(NULL));
    
    /* Parse mTCP configuration */
    if (Utils::parseEnv() != 0) {
        strcpy(g_error_msg, "Failed to parse mTCP configuration");
        return SOCKET_ERR_INIT;
    }
    
    /* Initialize mTCP stack (like doscurl)
     * First arg: number of TCP sockets to allocate (must match TCP_MAX_SOCKETS in sample.cfg)
     * Second arg: ring buffer size (TCP_SOCKET_RING_SIZE from GLOBAL.CFG)
     * Note: sample.cfg defines TCP_MAX_SOCKETS as 2, so we use 2 here
     */
    if (Utils::initStack(2, TCP_SOCKET_RING_SIZE, NULL, NULL) != 0) {
        strcpy(g_error_msg, "Failed to initialize mTCP stack");
        return SOCKET_ERR_INIT;
    }
    
    g_initialized = 1;
    strcpy(g_error_msg, "OK");
    return SOCKET_OK;
}

/*
 * Shutdown socket subsystem
 */
void socket_shutdown(void) {
    int i;
    
    if (!g_initialized) {
        return;
    }
    
    /* Close all open sockets */
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (g_sockets[i].in_use) {
            socket_close(i);
        }
    }
    
    /* Shutdown mTCP stack */
    Utils::endStack();
    
    g_initialized = 0;
}

/*
 * Create a new socket
 */
socket_handle_t socket_create(int type) {
    socket_handle_t handle;
    TcpSocket* tcp_sock;
    
    if (!g_initialized) {
        strcpy(g_error_msg, "Socket subsystem not initialized");
        return INVALID_SOCKET;
    }
    
    if (type != SOCKET_TYPE_TCP && type != SOCKET_TYPE_UDP) {
        strcpy(g_error_msg, "Invalid socket type");
        return INVALID_SOCKET;
    }
    
    /* Allocate socket handle */
    handle = allocate_socket();
    if (handle == INVALID_SOCKET) {
        strcpy(g_error_msg, "No free socket handles");
        return INVALID_SOCKET;
    }
    
    /* Create mTCP socket using TcpSocketMgr (like doscurl) */
    if (type == SOCKET_TYPE_TCP) {
        tcp_sock = TcpSocketMgr::getSocket();
        if (tcp_sock == NULL) {
            free_socket(handle);
            strcpy(g_error_msg, "Failed to get TCP socket from manager");
            return INVALID_SOCKET;
        }
        
        /* Set receive buffer size (like doscurl) */
        if (tcp_sock->setRecvBuffer(TCP_RECV_BUFFER) != 0) {
            TcpSocketMgr::freeSocket(tcp_sock);
            free_socket(handle);
            strcpy(g_error_msg, "Failed to set receive buffer");
            return INVALID_SOCKET;
        }
        
        g_sockets[handle].tcp_socket = tcp_sock;
        g_sockets[handle].type = SOCKET_TYPE_TCP;
        g_sockets[handle].state = SOCKET_STATE_CLOSED;
    } else {
        /* UDP not yet implemented */
        free_socket(handle);
        strcpy(g_error_msg, "UDP sockets not yet implemented");
        return INVALID_SOCKET;
    }
    
    strcpy(g_error_msg, "OK");
    return handle;
}

/*
 * Connect to remote host
 */
int socket_connect(socket_handle_t handle, const char* host, unsigned int port) {
    IpAddr_t ip_addr;
    int rc;
    unsigned char a, b, c, d;
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Connect only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    /* Check if host is an IP address (xxx.xxx.xxx.xxx format) */
    if (sscanf(host, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d) == 4) {
        /* Direct IP address - no DNS resolution needed */
        ip_addr[0] = a;
        ip_addr[1] = b;
        ip_addr[2] = c;
        ip_addr[3] = d;
    } else {
        /* Hostname - resolve via DNS */
        rc = Dns::resolve((char*)host, ip_addr, 10);
        if (rc < 0) {
            strcpy(g_error_msg, "Failed to resolve hostname");
            return SOCKET_ERR_CONNECT;
        }
    }
    
    /* Generate random local port (like doscurl does) */
    unsigned int local_port = 2048 + (rand() % 1000);
    
    /* Connect to remote host using non-blocking method (like doscurl) */
    g_sockets[handle].state = SOCKET_STATE_CONNECTING;
    rc = g_sockets[handle].tcp_socket->connectNonBlocking(local_port, ip_addr, port);
    
    if (rc != 0) {
        /* Failed to initiate connection */
        g_sockets[handle].state = SOCKET_STATE_CLOSED;
        sprintf(g_error_msg, "Failed to initiate connection (rc=%d)", rc);
        return SOCKET_ERR_CONNECT;
    }
    
    /* Connection initiated - wait for completion */
    unsigned long start_time = TIMER_GET_CURRENT();
    
    /* Poll for connection completion (60 second timeout) */
    while (1) {
        unsigned long current_time, ticks_elapsed;
        
        /* Process packets */
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
        
        /* Check if connection completed FIRST (like doscurl) */
        if (g_sockets[handle].tcp_socket->isConnectComplete()) {
            g_sockets[handle].state = SOCKET_STATE_CONNECTED;
            strcpy(g_error_msg, "OK");
            return SOCKET_OK;
        }
        
        /* Get current time for checks */
        current_time = TIMER_GET_CURRENT();
        ticks_elapsed = Timer_diff(start_time, current_time);
        
        /* Check for timeout (60 seconds) */
        if (ticks_elapsed > TIMER_MS_TO_TICKS(60000)) {
            g_sockets[handle].state = SOCKET_STATE_CLOSED;
            strcpy(g_error_msg, "Connection timeout");
            return SOCKET_ERR_TIMEOUT;
        }
        
        /* Check if remote closed (like doscurl) */
        if (g_sockets[handle].tcp_socket->isRemoteClosed()) {
            g_sockets[handle].state = SOCKET_STATE_CLOSED;
            strcpy(g_error_msg, "Connection refused or reset");
            return SOCKET_ERR_CONNECT;
        }
    }
}

/*
 * Bind socket to local address
 */
int socket_bind(socket_handle_t handle, unsigned int port) {
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Bind only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    /* mTCP doesn't require explicit bind for client sockets */
    /* For server sockets, bind is implicit in listen() */
    strcpy(g_error_msg, "OK");
    return SOCKET_OK;
}

/*
 * Listen for incoming connections
 */
int socket_listen(socket_handle_t handle, int backlog) {
    int rc;
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Listen only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    /* Start listening on any available port */
    rc = g_sockets[handle].tcp_socket->listen(0, 1);
    if (rc != 0) {
        strcpy(g_error_msg, "Failed to start listening");
        return SOCKET_ERR_LISTEN;
    }
    
    g_sockets[handle].state = SOCKET_STATE_LISTENING;
    strcpy(g_error_msg, "OK");
    return SOCKET_OK;
}

/*
 * Accept incoming connection
 */
socket_handle_t socket_accept(socket_handle_t handle) {
    socket_handle_t new_handle;
    TcpSocket* new_sock;
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return INVALID_SOCKET;
    }
    
    if (g_sockets[handle].state != SOCKET_STATE_LISTENING) {
        strcpy(g_error_msg, "Socket not in listening state");
        return INVALID_SOCKET;
    }
    
    /* Note: mTCP listen/accept mechanism is different from BSD sockets */
    /* For now, return error - will implement properly in Phase 4.1 Day 8-10 */
    strcpy(g_error_msg, "Accept not yet fully implemented");
    return INVALID_SOCKET;
    
    strcpy(g_error_msg, "OK");
    return new_handle;
}

/*
 * Send data through socket
 * Implements non-blocking send with packet processing loop
 * Based on doscurl implementation
 */
int socket_send(socket_handle_t handle, const void* data, unsigned int length) {
    unsigned int bytes_to_send;
    unsigned int bytes_sent;
    int16_t rc;
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].state != SOCKET_STATE_CONNECTED) {
        strcpy(g_error_msg, "Socket not connected");
        return SOCKET_ERR_SEND;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Send only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    bytes_to_send = length;
    bytes_sent = 0;
    
    /* Send all data with packet processing loop (like doscurl) */
    while (bytes_sent < bytes_to_send) {
        /* Process packets to push out buffers and free them up */
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();
        
        /* Try to send remaining data */
        rc = g_sockets[handle].tcp_socket->send(
            (uint8_t*)data + bytes_sent,
            bytes_to_send - bytes_sent
        );
        
        if (rc > 0) {
            bytes_sent += rc;
        } else if (rc == 0) {
            /* Out of send buffers - loop around to process packets */
            continue;
        } else {
            /* Send error */
            strcpy(g_error_msg, "Send failed");
            return SOCKET_ERR_SEND;
        }
    }
    
    strcpy(g_error_msg, "OK");
    return bytes_sent;
}

/*
 * Receive data from socket
 * Non-blocking receive with remote close detection
 * Based on doscurl implementation
 */
int socket_recv(socket_handle_t handle, void* buffer, unsigned int length) {
    int16_t received;
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].state != SOCKET_STATE_CONNECTED) {
        strcpy(g_error_msg, "Socket not connected");
        return SOCKET_ERR_RECV;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Receive only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    /* Process packets before receiving */
    PACKET_PROCESS_SINGLE;
    Arp::driveArp();
    Tcp::drivePackets();
    
    /* Try to receive data */
    received = g_sockets[handle].tcp_socket->recv((uint8_t*)buffer, length);
    
    if (received < 0) {
        strcpy(g_error_msg, "Receive failed");
        return SOCKET_ERR_RECV;
    }
    
    /* Check if remote closed (return 0 to indicate connection closed) */
    if (received == 0 && g_sockets[handle].tcp_socket->isRemoteClosed()) {
        strcpy(g_error_msg, "Connection closed by remote");
        return 0;  /* 0 indicates connection closed */
    }
    
    strcpy(g_error_msg, "OK");
    return received;
}

/*
 * Close socket
 */
int socket_close(socket_handle_t handle) {
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    /* Close mTCP socket */
    if (g_sockets[handle].type == SOCKET_TYPE_TCP && g_sockets[handle].tcp_socket != NULL) {
        g_sockets[handle].tcp_socket->close();
        TcpSocketMgr::freeSocket(g_sockets[handle].tcp_socket);
        g_sockets[handle].tcp_socket = NULL;
    }
    
    /* Free socket handle */
    free_socket(handle);
    
    strcpy(g_error_msg, "OK");
    return SOCKET_OK;
}

/*
 * Get socket state
 */
int socket_get_state(socket_handle_t handle) {
    if (!is_valid_handle(handle)) {
        return SOCKET_STATE_CLOSED;
    }
    
    return g_sockets[handle].state;
}

/*
 * Check if socket is ready for reading
 */
int socket_can_read(socket_handle_t handle) {
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return -1;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Can read only supported for TCP sockets");
        return -1;
    }
    
    if (g_sockets[handle].state != SOCKET_STATE_CONNECTED) {
        return 0;
    }
    
    /* Check if data is available */
    return (g_sockets[handle].tcp_socket->isEstablished() &&
            g_sockets[handle].tcp_socket->recvDataWaiting()) ? 1 : 0;
}

/*
 * Check if socket is ready for writing
 */
int socket_can_write(socket_handle_t handle) {
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return -1;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Can write only supported for TCP sockets");
        return -1;
    }
    
    if (g_sockets[handle].state != SOCKET_STATE_CONNECTED) {
        return 0;
    }
    
    /* Check if socket is ready for writing */
    return g_sockets[handle].tcp_socket->isEstablished() ? 1 : 0;
}

/*
 * Get last error message
 */
const char* socket_get_error(void) {
    return g_error_msg;
}

/*
 * Internal: Allocate a socket handle
 */
static socket_handle_t allocate_socket(void) {
    int i;
    
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (!g_sockets[i].in_use) {
            g_sockets[i].in_use = 1;
            return i;
        }
    }
    
    return INVALID_SOCKET;
}

/*
 * Internal: Free a socket handle
 */
static void free_socket(socket_handle_t handle) {
    if (handle >= 0 && handle < MAX_SOCKETS) {
        g_sockets[handle].in_use = 0;
        g_sockets[handle].type = 0;
        g_sockets[handle].state = SOCKET_STATE_CLOSED;
        g_sockets[handle].tcp_socket = NULL;
        g_sockets[handle].udp_socket = NULL;
    }
}

/*
 * Internal: Check if socket handle is valid
 */
static int is_valid_handle(socket_handle_t handle) {
    return (handle >= 0 && handle < MAX_SOCKETS && g_sockets[handle].in_use);
}

// Made with Bob
