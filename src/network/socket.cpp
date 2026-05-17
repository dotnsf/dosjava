/*
 * socket.cpp - C-level Socket Wrapper Implementation
 * 
 * Wraps mTCP C++ API for use from C code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    
    /* Parse mTCP configuration */
    if (Utils::parseEnv() != 0) {
        strcpy(g_error_msg, "Failed to parse mTCP configuration");
        return SOCKET_ERR_INIT;
    }
    
    /* Initialize mTCP stack */
    if (Utils::initStack(MAX_SOCKETS, MAX_SOCKETS, NULL, NULL) != 0) {
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
    
    /* Create mTCP socket */
    if (type == SOCKET_TYPE_TCP) {
        tcp_sock = new TcpSocket();
        if (tcp_sock == NULL) {
            free_socket(handle);
            strcpy(g_error_msg, "Failed to create TCP socket");
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
    
    if (!is_valid_handle(handle)) {
        strcpy(g_error_msg, "Invalid socket handle");
        return SOCKET_ERR_INVALID;
    }
    
    if (g_sockets[handle].type != SOCKET_TYPE_TCP) {
        strcpy(g_error_msg, "Connect only supported for TCP sockets");
        return SOCKET_ERR_INVALID;
    }
    
    /* Resolve hostname to IP address */
    rc = Dns::resolve((char*)host, ip_addr, 10);
    if (rc < 0) {
        strcpy(g_error_msg, "Failed to resolve hostname");
        return SOCKET_ERR_CONNECT;
    }
    
    /* Connect to remote host */
    /* TcpSocket::connect(srcPort, host, dstPort, timeout) */
    g_sockets[handle].state = SOCKET_STATE_CONNECTING;
    rc = g_sockets[handle].tcp_socket->connect(0, ip_addr, port, 10000);
    
    if (rc == 0) {
        g_sockets[handle].state = SOCKET_STATE_CONNECTED;
        strcpy(g_error_msg, "OK");
        return SOCKET_OK;
    } else {
        g_sockets[handle].state = SOCKET_STATE_CLOSED;
        strcpy(g_error_msg, "Connection failed");
        return SOCKET_ERR_CONNECT;
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
 */
int socket_send(socket_handle_t handle, const void* data, unsigned int length) {
    int sent;
    
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
    
    /* Send data */
    sent = g_sockets[handle].tcp_socket->send((uint8_t*)data, length);
    
    if (sent < 0) {
        strcpy(g_error_msg, "Send failed");
        return SOCKET_ERR_SEND;
    }
    
    strcpy(g_error_msg, "OK");
    return sent;
}

/*
 * Receive data from socket
 */
int socket_recv(socket_handle_t handle, void* buffer, unsigned int length) {
    int received;
    
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
    
    /* Receive data */
    received = g_sockets[handle].tcp_socket->recv((uint8_t*)buffer, length);
    
    if (received < 0) {
        strcpy(g_error_msg, "Receive failed");
        return SOCKET_ERR_RECV;
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
        delete g_sockets[handle].tcp_socket;
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
