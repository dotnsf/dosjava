/*
 * socket.h - C-level Socket Wrapper for mTCP
 * 
 * Provides C interface to mTCP TCP/UDP socket functionality
 * for integration with dosjava VM.
 */

#ifndef SOCKET_H
#define SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Socket types */
#define SOCKET_TYPE_TCP 1
#define SOCKET_TYPE_UDP 2

/* Socket states */
#define SOCKET_STATE_CLOSED     0
#define SOCKET_STATE_LISTENING  1
#define SOCKET_STATE_CONNECTING 2
#define SOCKET_STATE_CONNECTED  3
#define SOCKET_STATE_CLOSING    4

/* Error codes */
#define SOCKET_OK              0
#define SOCKET_ERR_INIT       -1
#define SOCKET_ERR_CREATE     -2
#define SOCKET_ERR_CONNECT    -3
#define SOCKET_ERR_BIND       -4
#define SOCKET_ERR_LISTEN     -5
#define SOCKET_ERR_ACCEPT     -6
#define SOCKET_ERR_SEND       -7
#define SOCKET_ERR_RECV       -8
#define SOCKET_ERR_CLOSE      -9
#define SOCKET_ERR_TIMEOUT   -10
#define SOCKET_ERR_INVALID   -11
#define SOCKET_ERR_NOMEM     -12

/* Maximum number of sockets (reduced to 2 like doscurl to avoid memory issues) */
#define MAX_SOCKETS 2

/* Socket handle type */
typedef int socket_handle_t;

/* Invalid socket handle */
#define INVALID_SOCKET -1

/*
 * Initialize socket subsystem
 * Must be called before any other socket functions
 * Returns: SOCKET_OK on success, error code on failure
 */
int socket_init(void);

/*
 * Shutdown socket subsystem
 * Closes all open sockets and releases resources
 */
void socket_shutdown(void);

/*
 * Create a new socket
 * type: SOCKET_TYPE_TCP or SOCKET_TYPE_UDP
 * Returns: socket handle on success, INVALID_SOCKET on failure
 */
socket_handle_t socket_create(int type);

/*
 * Connect to remote host
 * handle: socket handle
 * host: hostname or IP address (e.g., "192.168.1.1")
 * port: port number
 * Returns: SOCKET_OK on success, error code on failure
 */
int socket_connect(socket_handle_t handle, const char* host, unsigned int port);

/*
 * Bind socket to local address
 * handle: socket handle
 * port: local port number (0 for any available port)
 * Returns: SOCKET_OK on success, error code on failure
 */
int socket_bind(socket_handle_t handle, unsigned int port);

/*
 * Listen for incoming connections (TCP only)
 * handle: socket handle
 * backlog: maximum number of pending connections
 * Returns: SOCKET_OK on success, error code on failure
 */
int socket_listen(socket_handle_t handle, int backlog);

/*
 * Accept incoming connection (TCP only)
 * handle: listening socket handle
 * Returns: new socket handle for accepted connection, INVALID_SOCKET on failure
 */
socket_handle_t socket_accept(socket_handle_t handle);

/*
 * Send data through socket
 * handle: socket handle
 * data: pointer to data buffer
 * length: number of bytes to send
 * Returns: number of bytes sent on success, error code on failure
 */
int socket_send(socket_handle_t handle, const void* data, unsigned int length);

/*
 * Receive data from socket
 * handle: socket handle
 * buffer: pointer to receive buffer
 * length: maximum number of bytes to receive
 * Returns: number of bytes received on success, 0 on connection closed, error code on failure
 */
int socket_recv(socket_handle_t handle, void* buffer, unsigned int length);

/*
 * Close socket
 * handle: socket handle
 * Returns: SOCKET_OK on success, error code on failure
 */
int socket_close(socket_handle_t handle);

/*
 * Get socket state
 * handle: socket handle
 * Returns: socket state (SOCKET_STATE_*)
 */
int socket_get_state(socket_handle_t handle);

/*
 * Check if socket is ready for reading
 * handle: socket handle
 * Returns: 1 if data available, 0 if not, negative on error
 */
int socket_can_read(socket_handle_t handle);

/*
 * Check if socket is ready for writing
 * handle: socket handle
 * Returns: 1 if ready, 0 if not, negative on error
 */
int socket_can_write(socket_handle_t handle);

/*
 * Get last error message
 * Returns: pointer to error message string
 */
const char* socket_get_error(void);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_H */

// Made with Bob
