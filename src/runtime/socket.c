/*
 * socket.c - Runtime Socket class implementation
 *
 * C implementation of java.net.Socket for DOS Java VM
 * Wraps the C-level socket API from src/network/socket.h
 */

#include "socket.h"
#include "../network/socket.h"
#include "../vm/memory.h"
#include <stdio.h>
#include <string.h>

/* Global flag for socket subsystem initialization */
static uint8_t g_socket_initialized = 0;

/* Class IDs for runtime objects */
#define CLASS_ID_SOCKET 100
#define CLASS_ID_SOCKET_EXCEPTION 101

/*
 * Socket subsystem management
 */

int socket_subsystem_init(void) {
    if (g_socket_initialized) {
        return 0;  /* Already initialized */
    }
    
    /* Initialize C-level socket API */
    if (socket_init() != SOCKET_OK) {
        return -1;
    }
    
    g_socket_initialized = 1;
    return 0;
}

void socket_subsystem_shutdown(void) {
    if (!g_socket_initialized) {
        return;  /* Not initialized */
    }
    
    /* Shutdown C-level socket API */
    socket_shutdown();
    g_socket_initialized = 0;
}

uint8_t socket_subsystem_is_initialized(void) {
    return g_socket_initialized;
}

/*
 * Socket class implementation
 */

Socket* socket_runtime_new(void) {
    Socket* sock;
    
    /* Check if subsystem is initialized */
    if (!g_socket_initialized) {
        return NULL;
    }
    
    /* Allocate Socket structure */
    sock = (Socket*)memory_alloc(sizeof(Socket));
    if (sock == NULL) {
        return NULL;
    }
    
    /* Initialize base Object */
    sock->base.class_id = CLASS_ID_SOCKET;
    sock->base.ref_count = 1;
    
    /* Initialize Socket fields */
    sock->handle = -1;  /* Not connected */
    sock->host = NULL;
    sock->port = 0;
    sock->connected = 0;
    sock->closed = 0;
    
    return sock;
}

Socket* socket_runtime_new_connected(String* host, uint16_t port) {
    Socket* sock;
    
    /* Create unconnected socket */
    sock = socket_runtime_new();
    if (sock == NULL) {
        return NULL;
    }
    
    /* Connect to host:port */
    if (socket_runtime_connect(sock, host, port) != 0) {
        socket_runtime_delete(sock);
        return NULL;
    }
    
    return sock;
}

void socket_runtime_delete(Socket* sock) {
    if (sock == NULL) {
        return;
    }
    
    /* Close socket if still open */
    if (!sock->closed && sock->handle >= 0) {
        socket_runtime_close(sock);
    }
    
    /* Release host string */
    if (sock->host != NULL) {
        string_delete(sock->host);
        sock->host = NULL;
    }
    
    /* Free Socket structure */
    memory_free(sock);
}

int socket_runtime_connect(Socket* sock, String* host, uint16_t port) {
    const char* host_cstr;
    int rc;
    
    if (sock == NULL) {
        return -1;
    }
    
    if (sock->closed) {
        return -1;
    }
    
    if (sock->connected) {
        return -1;
    }
    
    if (host == NULL) {
        return -1;
    }
    
    /* Create C-level socket */
    sock->handle = socket_create(SOCKET_TYPE_TCP);
    if (sock->handle == INVALID_SOCKET) {
        return -1;
    }
    
    /* Connect to remote host */
    host_cstr = string_tocstr(host);
    rc = socket_connect(sock->handle, host_cstr, port);
    if (rc != SOCKET_OK) {
        socket_close(sock->handle);
        sock->handle = -1;
        return -1;
    }
    
    /* Store connection info */
    sock->host = string_new(host_cstr);
    sock->port = port;
    sock->connected = 1;
    
    return 0;
}

int socket_runtime_send(Socket* sock, const uint8_t* data, uint16_t length) {
    int sent;
    
    if (sock == NULL) {
        return -1;
    }
    
    if (sock->closed) {
        return -1;
    }
    
    if (!sock->connected) {
        return -1;
    }
    
    if (data == NULL || length == 0) {
        return 0;  /* Nothing to send */
    }
    
    /* Send data through C-level socket */
    sent = socket_send(sock->handle, data, length);
    if (sent < 0) {
        return -1;
    }
    
    return sent;
}

int socket_runtime_recv(Socket* sock, uint8_t* buffer, uint16_t max_length) {
    int received;
    
    if (sock == NULL) {
        return -1;
    }
    
    if (sock->closed) {
        return -1;
    }
    
    if (!sock->connected) {
        return -1;
    }
    
    if (buffer == NULL || max_length == 0) {
        return 0;  /* Nothing to receive */
    }
    
    /* Receive data from C-level socket */
    received = socket_recv(sock->handle, buffer, max_length);
    if (received < 0) {
        return -1;
    }
    
    /* Check if connection was closed by remote */
    if (received == 0) {
        sock->connected = 0;
    }
    
    return received;
}

int socket_runtime_close(Socket* sock) {
    int rc;
    
    if (sock == NULL) {
        return 0;  /* Already NULL */
    }
    
    if (sock->closed) {
        return 0;  /* Already closed */
    }
    
    /* Close C-level socket */
    if (sock->handle >= 0) {
        rc = socket_close(sock->handle);
        if (rc != SOCKET_OK) {
            /* Continue anyway */
        }
        sock->handle = -1;
    }
    
    /* Mark as closed */
    sock->connected = 0;
    sock->closed = 1;
    
    return 0;
}

uint8_t socket_runtime_is_connected(Socket* sock) {
    if (sock == NULL) {
        return 0;
    }
    return sock->connected;
}

uint8_t socket_runtime_is_closed(Socket* sock) {
    if (sock == NULL) {
        return 1;  /* NULL socket is considered closed */
    }
    return sock->closed;
}

String* socket_runtime_get_host(Socket* sock) {
    if (sock == NULL || !sock->connected) {
        return NULL;
    }
    return sock->host;
}

uint16_t socket_runtime_get_port(Socket* sock) {
    if (sock == NULL || !sock->connected) {
        return 0;
    }
    return sock->port;
}

int16_t socket_runtime_get_handle(Socket* sock) {
    if (sock == NULL) {
        return -1;
    }
    return sock->handle;
}

/*
 * SocketException class implementation
 */

SocketException* socket_exception_new(const char* message, int16_t error_code) {
    SocketException* ex;
    
    /* Allocate SocketException structure */
    ex = (SocketException*)memory_alloc(sizeof(SocketException));
    if (ex == NULL) {
        return NULL;
    }
    
    /* Initialize base Object */
    ex->base.class_id = CLASS_ID_SOCKET_EXCEPTION;
    ex->base.ref_count = 1;
    
    /* Initialize SocketException fields */
    ex->message = string_new(message);
    ex->error_code = error_code;
    
    return ex;
}

SocketException* socket_exception_from_error(int16_t error_code) {
    const char* error_msg;
    
    /* Get error message from C-level API */
    error_msg = socket_get_error();
    
    return socket_exception_new(error_msg, error_code);
}

void socket_exception_delete(SocketException* ex) {
    if (ex == NULL) {
        return;
    }
    
    /* Release message string */
    if (ex->message != NULL) {
        string_delete(ex->message);
        ex->message = NULL;
    }
    
    /* Free SocketException structure */
    memory_free(ex);
}

String* socket_exception_get_message(SocketException* ex) {
    if (ex == NULL) {
        return NULL;
    }
    return ex->message;
}

int16_t socket_exception_get_error_code(SocketException* ex) {
    if (ex == NULL) {
        return 0;
    }
    return ex->error_code;
}

// Made with Bob
