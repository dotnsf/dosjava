#ifndef SOCKET_H_RUNTIME
#define SOCKET_H_RUNTIME

#include "../types.h"
#include "object.h"
#include "string.h"

/**
 * Socket structure - C representation of java.net.Socket
 * 
 * Provides network socket functionality for TCP connections
 * Wraps the C-level socket API from src/network/socket.h
 */
typedef struct Socket {
    Object base;             /* Base Object structure */
    int16_t handle;          /* C-level socket handle (-1 if closed) */
    String* host;            /* Remote host (IP or hostname) */
    uint16_t port;           /* Remote port number */
    uint8_t connected;       /* 1 if connected, 0 otherwise */
    uint8_t closed;          /* 1 if closed, 0 otherwise */
} Socket;

/**
 * SocketException structure - C representation of java.net.SocketException
 * 
 * Exception thrown when socket operations fail
 */
typedef struct SocketException {
    Object base;             /* Base Object structure */
    String* message;         /* Error message */
    int16_t error_code;      /* Error code from C-level API */
} SocketException;

/* Socket class methods */

/**
 * Create a new Socket instance (unconnected)
 * @return Pointer to new Socket, or NULL on error
 */
Socket* socket_runtime_new(void);

/**
 * Create a new Socket and connect to host:port
 * @param host Remote host (IP address or hostname)
 * @param port Remote port number
 * @return Pointer to new connected Socket, or NULL on error
 */
Socket* socket_runtime_new_connected(String* host, uint16_t port);

/**
 * Delete a Socket instance
 * Automatically closes the socket if still open
 * @param sock Socket to delete
 */
void socket_runtime_delete(Socket* sock);

/**
 * Connect socket to remote host
 * @param sock Socket instance
 * @param host Remote host (IP address or hostname)
 * @param port Remote port number
 * @return 0 on success, -1 on error
 */
int socket_runtime_connect(Socket* sock, String* host, uint16_t port);

/**
 * Send data through socket
 * @param sock Socket instance
 * @param data Data buffer to send
 * @param length Number of bytes to send
 * @return Number of bytes sent on success, -1 on error
 */
int socket_runtime_send(Socket* sock, const uint8_t* data, uint16_t length);

/**
 * Receive data from socket
 * @param sock Socket instance
 * @param buffer Buffer to store received data
 * @param max_length Maximum bytes to receive
 * @return Number of bytes received (0 if connection closed, -1 on error)
 */
int socket_runtime_recv(Socket* sock, uint8_t* buffer, uint16_t max_length);

/**
 * Close the socket
 * @param sock Socket instance
 * @return 0 on success, -1 on error
 */
int socket_runtime_close(Socket* sock);

/**
 * Check if socket is connected
 * @param sock Socket instance
 * @return 1 if connected, 0 otherwise
 */
uint8_t socket_runtime_is_connected(Socket* sock);

/**
 * Check if socket is closed
 * @param sock Socket instance
 * @return 1 if closed, 0 otherwise
 */
uint8_t socket_runtime_is_closed(Socket* sock);

/**
 * Get remote host
 * @param sock Socket instance
 * @return Host string, or NULL if not connected
 */
String* socket_runtime_get_host(Socket* sock);

/**
 * Get remote port
 * @param sock Socket instance
 * @return Port number, or 0 if not connected
 */
uint16_t socket_runtime_get_port(Socket* sock);

/**
 * Get socket handle (for debugging)
 * @param sock Socket instance
 * @return C-level socket handle
 */
int16_t socket_runtime_get_handle(Socket* sock);

/* SocketException class methods */

/**
 * Create a new SocketException
 * @param message Error message
 * @param error_code Error code from C-level API
 * @return Pointer to new SocketException, or NULL on error
 */
SocketException* socket_exception_new(const char* message, int16_t error_code);

/**
 * Create a new SocketException from C-level error
 * Uses socket_get_error() to get error message
 * @param error_code Error code from C-level API
 * @return Pointer to new SocketException, or NULL on error
 */
SocketException* socket_exception_from_error(int16_t error_code);

/**
 * Delete a SocketException instance
 * @param ex Exception to delete
 */
void socket_exception_delete(SocketException* ex);

/**
 * Get exception message
 * @param ex Exception instance
 * @return Error message string
 */
String* socket_exception_get_message(SocketException* ex);

/**
 * Get exception error code
 * @param ex Exception instance
 * @return Error code
 */
int16_t socket_exception_get_error_code(SocketException* ex);

/* Global socket subsystem management */

/**
 * Initialize socket subsystem
 * Must be called before any socket operations
 * @return 0 on success, -1 on error
 */
int socket_subsystem_init(void);

/**
 * Shutdown socket subsystem
 * Should be called before program exit
 */
void socket_subsystem_shutdown(void);

/**
 * Check if socket subsystem is initialized
 * @return 1 if initialized, 0 otherwise
 */
uint8_t socket_subsystem_is_initialized(void);

#endif /* SOCKET_H_RUNTIME */

// Made with Bob
