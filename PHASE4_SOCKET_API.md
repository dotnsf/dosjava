# Socket API Reference

## Overview

This document provides complete reference documentation for the dosjava C-level socket API. The API wraps mTCP functionality for use in 16-bit PC-DOS applications.

**Version**: Phase 4.1  
**Target**: 16-bit PC-DOS (640KB RAM)  
**Library**: mTCP TCP/IP stack  
**Memory Model**: Small (-ms)

## Table of Contents

1. [Quick Start](#quick-start)
2. [API Functions](#api-functions)
3. [Data Types](#data-types)
4. [Error Codes](#error-codes)
5. [Usage Examples](#usage-examples)
6. [Best Practices](#best-practices)
7. [Memory Management](#memory-management)
8. [Troubleshooting](#troubleshooting)

## Quick Start

### Basic Client Example

```c
#include "socket.h"

int main(void) {
    socket_handle_t sock;
    char buffer[256];
    int rc;
    
    /* Initialize */
    if (socket_init() != SOCKET_OK) {
        printf("Init failed: %s\n", socket_get_error());
        return 1;
    }
    
    /* Create socket */
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Create failed: %s\n", socket_get_error());
        socket_shutdown();
        return 1;
    }
    
    /* Connect */
    rc = socket_connect(sock, "192.168.1.100", 80);
    if (rc != SOCKET_OK) {
        printf("Connect failed: %s\n", socket_get_error());
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    
    /* Send data */
    const char* request = "GET / HTTP/1.0\r\n\r\n";
    rc = socket_send(sock, request, strlen(request));
    if (rc < 0) {
        printf("Send failed: %s\n", socket_get_error());
    }
    
    /* Receive data */
    rc = socket_recv(sock, buffer, sizeof(buffer) - 1);
    if (rc > 0) {
        buffer[rc] = '\0';
        printf("Received: %s\n", buffer);
    }
    
    /* Cleanup */
    socket_close(sock);
    socket_shutdown();
    return 0;
}
```

## API Functions

### Initialization and Shutdown

#### socket_init()

Initialize the socket subsystem.

```c
int socket_init(void);
```

**Returns**:
- `SOCKET_OK` (0) on success
- `SOCKET_ERR_INIT` (-1) on failure

**Description**:
- Must be called before any other socket functions
- Initializes mTCP stack with configuration from sample.cfg
- Allocates memory pool for sockets (not returned to DOS until program exit)
- Can be called multiple times (subsequent calls reuse existing pool)

**Example**:
```c
if (socket_init() != SOCKET_OK) {
    fprintf(stderr, "Failed to initialize: %s\n", socket_get_error());
    return 1;
}
```

**Notes**:
- Requires MTCP.CFG environment variable pointing to configuration file
- Uses ~24 KB for mTCP stack initialization
- Configuration must match MAX_SOCKETS in socket.h

---

#### socket_shutdown()

Shutdown the socket subsystem.

```c
void socket_shutdown(void);
```

**Description**:
- Closes all open sockets
- Releases socket handles
- Does NOT return memory to DOS (mTCP design)
- Memory is reclaimed when program exits

**Example**:
```c
socket_shutdown();
```

**Notes**:
- Safe to call even if no sockets are open
- Should be called before program exit for clean shutdown
- Memory pool remains allocated (normal mTCP behavior)

---

### Socket Creation and Destruction

#### socket_create()

Create a new socket.

```c
socket_handle_t socket_create(int type);
```

**Parameters**:
- `type`: Socket type
  - `SOCKET_TYPE_TCP` (1) - TCP socket
  - `SOCKET_TYPE_UDP` (2) - UDP socket (not yet implemented)

**Returns**:
- Socket handle (0 to MAX_SOCKETS-1) on success
- `INVALID_SOCKET` (-1) on failure

**Description**:
- Allocates a socket from the pool
- Sets up receive buffer (4 KB per socket)
- Socket is in CLOSED state initially

**Example**:
```c
socket_handle_t sock = socket_create(SOCKET_TYPE_TCP);
if (sock == INVALID_SOCKET) {
    fprintf(stderr, "Failed to create socket: %s\n", socket_get_error());
    return 1;
}
```

**Notes**:
- Maximum MAX_SOCKETS (currently 4) can be created
- Each socket uses ~4 KB memory
- Must call socket_close() when done

---

#### socket_close()

Close a socket.

```c
int socket_close(socket_handle_t handle);
```

**Parameters**:
- `handle`: Socket handle returned by socket_create()

**Returns**:
- `SOCKET_OK` (0) on success
- `SOCKET_ERR_INVALID` (-11) if handle is invalid
- `SOCKET_ERR_CLOSE` (-9) on other errors

**Description**:
- Closes the connection
- Frees the socket handle for reuse
- Does NOT return memory to DOS (mTCP design)

**Example**:
```c
if (socket_close(sock) != SOCKET_OK) {
    fprintf(stderr, "Failed to close socket: %s\n", socket_get_error());
}
```

**Notes**:
- Always close sockets when done to free handles
- Memory remains in pool for reuse
- Safe to call on already closed sockets

---

### Connection Management

#### socket_connect()

Connect to a remote host (client).

```c
int socket_connect(socket_handle_t handle, const char* host, unsigned int port);
```

**Parameters**:
- `handle`: Socket handle
- `host`: Hostname or IP address (e.g., "192.168.1.1" or "example.com")
- `port`: Port number (1-65535)

**Returns**:
- `SOCKET_OK` (0) on success
- `SOCKET_ERR_CONNECT` (-3) on connection failure
- `SOCKET_ERR_TIMEOUT` (-10) if connection times out (60 seconds)
- `SOCKET_ERR_INVALID` (-11) if handle is invalid

**Description**:
- Initiates non-blocking connection to remote host
- Performs DNS resolution if hostname is provided
- Uses random local port (2048-3047)
- Polls for connection completion with 60-second timeout

**Example**:
```c
/* Connect by IP */
rc = socket_connect(sock, "192.168.1.100", 80);

/* Connect by hostname */
rc = socket_connect(sock, "www.example.com", 80);

if (rc != SOCKET_OK) {
    fprintf(stderr, "Connection failed: %s\n", socket_get_error());
}
```

**Notes**:
- Blocks until connection completes or times out
- Processes packets during connection (non-blocking internally)
- DNS resolution may take several seconds
- Connection timeout is 60 seconds

---

#### socket_bind()

Bind socket to local address (server).

```c
int socket_bind(socket_handle_t handle, unsigned int port);
```

**Parameters**:
- `handle`: Socket handle
- `port`: Local port number (0 for any available port)

**Returns**:
- `SOCKET_OK` (0) on success
- `SOCKET_ERR_BIND` (-4) on failure
- `SOCKET_ERR_INVALID` (-11) if handle is invalid

**Description**:
- Binds socket to local port
- Required before socket_listen() for servers
- Port 0 lets system choose available port

**Example**:
```c
rc = socket_bind(sock, 8080);
if (rc != SOCKET_OK) {
    fprintf(stderr, "Bind failed: %s\n", socket_get_error());
}
```

**Notes**:
- Currently a placeholder (mTCP handles binding internally)
- Will be fully implemented in Phase 4.2

---

#### socket_listen()

Listen for incoming connections (server).

```c
int socket_listen(socket_handle_t handle, int backlog);
```

**Parameters**:
- `handle`: Socket handle
- `backlog`: Maximum pending connections (typically 1-5)

**Returns**:
- `SOCKET_OK` (0) on success
- `SOCKET_ERR_LISTEN` (-5) on failure
- `SOCKET_ERR_INVALID` (-11) if handle is invalid

**Description**:
- Puts socket in listening state
- Allows incoming connections
- Must call socket_bind() first

**Example**:
```c
rc = socket_listen(sock, 5);
if (rc != SOCKET_OK) {
    fprintf(stderr, "Listen failed: %s\n", socket_get_error());
}
```

**Notes**:
- Server functionality
- Will be fully implemented in Phase 4.2

---

#### socket_accept()

Accept incoming connection (server).

```c
socket_handle_t socket_accept(socket_handle_t handle);
```

**Parameters**:
- `handle`: Listening socket handle

**Returns**:
- New socket handle for accepted connection on success
- `INVALID_SOCKET` (-1) on failure

**Description**:
- Accepts pending connection
- Returns new socket for communication
- Original socket remains in listening state

**Example**:
```c
socket_handle_t client = socket_accept(server_sock);
if (client == INVALID_SOCKET) {
    fprintf(stderr, "Accept failed: %s\n", socket_get_error());
}
```

**Notes**:
- Server functionality
- Will be fully implemented in Phase 4.2

---

### Data Transfer

#### socket_send()

Send data through socket.

```c
int socket_send(socket_handle_t handle, const void* data, unsigned int length);
```

**Parameters**:
- `handle`: Socket handle
- `data`: Pointer to data buffer
- `length`: Number of bytes to send

**Returns**:
- Number of bytes sent on success (may be less than length)
- `SOCKET_ERR_SEND` (-7) on error
- `SOCKET_ERR_INVALID` (-11) if handle is invalid

**Description**:
- Sends data through connected socket
- Non-blocking with packet processing loop
- Automatically handles partial sends
- Continues until all data is sent or error occurs

**Example**:
```c
const char* msg = "Hello, World!";
int sent = socket_send(sock, msg, strlen(msg));
if (sent < 0) {
    fprintf(stderr, "Send failed: %s\n", socket_get_error());
} else {
    printf("Sent %d bytes\n", sent);
}
```

**Notes**:
- Processes packets during send (PACKET_PROCESS_SINGLE)
- Handles send buffer full condition automatically
- Returns total bytes sent (always equals length on success)
- For large data, may take multiple internal iterations

---

#### socket_recv()

Receive data from socket.

```c
int socket_recv(socket_handle_t handle, void* buffer, unsigned int length);
```

**Parameters**:
- `handle`: Socket handle
- `buffer`: Pointer to receive buffer
- `length`: Maximum bytes to receive

**Returns**:
- Number of bytes received (1 to length) on success
- 0 if connection closed by remote
- `SOCKET_ERR_RECV` (-8) on error
- `SOCKET_ERR_INVALID` (-11) if handle is invalid

**Description**:
- Receives data from connected socket
- Non-blocking operation
- Processes packets before receiving
- Detects remote connection close

**Example**:
```c
char buffer[1024];
int received = socket_recv(sock, buffer, sizeof(buffer) - 1);
if (received > 0) {
    buffer[received] = '\0';
    printf("Received: %s\n", buffer);
} else if (received == 0) {
    printf("Connection closed\n");
} else {
    fprintf(stderr, "Receive failed: %s\n", socket_get_error());
}
```

**Notes**:
- Returns 0 when remote closes connection
- May return less than requested length
- Call multiple times to receive all data
- Processes packets before each receive

---

### Socket State and Status

#### socket_get_state()

Get current socket state.

```c
int socket_get_state(socket_handle_t handle);
```

**Parameters**:
- `handle`: Socket handle

**Returns**:
- `SOCKET_STATE_CLOSED` (0) - Socket is closed
- `SOCKET_STATE_LISTENING` (1) - Socket is listening
- `SOCKET_STATE_CONNECTING` (2) - Connection in progress
- `SOCKET_STATE_CONNECTED` (3) - Socket is connected
- `SOCKET_STATE_CLOSING` (4) - Socket is closing

**Example**:
```c
int state = socket_get_state(sock);
if (state == SOCKET_STATE_CONNECTED) {
    printf("Socket is connected\n");
}
```

---

#### socket_can_read()

Check if socket has data available to read.

```c
int socket_can_read(socket_handle_t handle);
```

**Parameters**:
- `handle`: Socket handle

**Returns**:
- 1 if data is available
- 0 if no data available
- Negative value on error

**Example**:
```c
if (socket_can_read(sock) > 0) {
    int received = socket_recv(sock, buffer, sizeof(buffer));
    /* Process data */
}
```

**Notes**:
- Non-blocking check
- Useful for polling multiple sockets

---

#### socket_can_write()

Check if socket is ready for writing.

```c
int socket_can_write(socket_handle_t handle);
```

**Parameters**:
- `handle`: Socket handle

**Returns**:
- 1 if socket is ready for writing
- 0 if not ready
- Negative value on error

**Example**:
```c
if (socket_can_write(sock) > 0) {
    socket_send(sock, data, length);
}
```

**Notes**:
- Checks if send buffer has space
- Useful before large sends

---

### Error Handling

#### socket_get_error()

Get last error message.

```c
const char* socket_get_error(void);
```

**Returns**:
- Pointer to error message string
- "OK" if no error

**Example**:
```c
if (socket_connect(sock, host, port) != SOCKET_OK) {
    fprintf(stderr, "Error: %s\n", socket_get_error());
}
```

**Notes**:
- Returns human-readable error message
- Message is stored in static buffer
- Valid until next socket function call

---

## Data Types

### socket_handle_t

```c
typedef int socket_handle_t;
```

Socket handle type. Valid handles are 0 to MAX_SOCKETS-1.

### INVALID_SOCKET

```c
#define INVALID_SOCKET -1
```

Invalid socket handle constant.

### Socket Types

```c
#define SOCKET_TYPE_TCP 1
#define SOCKET_TYPE_UDP 2
```

### Socket States

```c
#define SOCKET_STATE_CLOSED     0
#define SOCKET_STATE_LISTENING  1
#define SOCKET_STATE_CONNECTING 2
#define SOCKET_STATE_CONNECTED  3
#define SOCKET_STATE_CLOSING    4
```

### Maximum Sockets

```c
#define MAX_SOCKETS 4
```

Maximum number of concurrent sockets (configurable).

---

## Error Codes

| Code | Name | Value | Description |
|------|------|-------|-------------|
| SOCKET_OK | Success | 0 | Operation successful |
| SOCKET_ERR_INIT | Init error | -1 | Initialization failed |
| SOCKET_ERR_CREATE | Create error | -2 | Socket creation failed |
| SOCKET_ERR_CONNECT | Connect error | -3 | Connection failed |
| SOCKET_ERR_BIND | Bind error | -4 | Bind failed |
| SOCKET_ERR_LISTEN | Listen error | -5 | Listen failed |
| SOCKET_ERR_ACCEPT | Accept error | -6 | Accept failed |
| SOCKET_ERR_SEND | Send error | -7 | Send failed |
| SOCKET_ERR_RECV | Receive error | -8 | Receive failed |
| SOCKET_ERR_CLOSE | Close error | -9 | Close failed |
| SOCKET_ERR_TIMEOUT | Timeout | -10 | Operation timed out |
| SOCKET_ERR_INVALID | Invalid handle | -11 | Invalid socket handle |
| SOCKET_ERR_NOMEM | No memory | -12 | Out of memory |

---

## Usage Examples

### Example 1: Simple HTTP GET

```c
#include <stdio.h>
#include <string.h>
#include "socket.h"

int main(void) {
    socket_handle_t sock;
    char buffer[2048];
    int rc, total = 0;
    
    /* Initialize */
    if (socket_init() != SOCKET_OK) {
        fprintf(stderr, "Init failed\n");
        return 1;
    }
    
    /* Create and connect */
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "Create failed\n");
        socket_shutdown();
        return 1;
    }
    
    if (socket_connect(sock, "192.168.1.100", 80) != SOCKET_OK) {
        fprintf(stderr, "Connect failed\n");
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    
    /* Send HTTP request */
    const char* request = 
        "GET / HTTP/1.0\r\n"
        "Host: 192.168.1.100\r\n"
        "Connection: close\r\n"
        "\r\n";
    
    rc = socket_send(sock, request, strlen(request));
    if (rc < 0) {
        fprintf(stderr, "Send failed\n");
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    
    /* Receive response */
    while (1) {
        rc = socket_recv(sock, buffer, sizeof(buffer) - 1);
        if (rc > 0) {
            buffer[rc] = '\0';
            printf("%s", buffer);
            total += rc;
        } else if (rc == 0) {
            /* Connection closed */
            break;
        } else {
            fprintf(stderr, "Receive error\n");
            break;
        }
    }
    
    printf("\nTotal received: %d bytes\n", total);
    
    /* Cleanup */
    socket_close(sock);
    socket_shutdown();
    return 0;
}
```

### Example 2: Multiple Connections

```c
#include <stdio.h>
#include "socket.h"

#define NUM_HOSTS 3

int main(void) {
    socket_handle_t socks[NUM_HOSTS];
    const char* hosts[] = {
        "192.168.1.100",
        "192.168.1.101",
        "192.168.1.102"
    };
    int i;
    
    /* Initialize */
    socket_init();
    
    /* Create and connect all sockets */
    for (i = 0; i < NUM_HOSTS; i++) {
        socks[i] = socket_create(SOCKET_TYPE_TCP);
        if (socks[i] == INVALID_SOCKET) {
            fprintf(stderr, "Failed to create socket %d\n", i);
            continue;
        }
        
        if (socket_connect(socks[i], hosts[i], 80) != SOCKET_OK) {
            fprintf(stderr, "Failed to connect to %s\n", hosts[i]);
            socket_close(socks[i]);
            socks[i] = INVALID_SOCKET;
        } else {
            printf("Connected to %s\n", hosts[i]);
        }
    }
    
    /* Use sockets... */
    
    /* Cleanup */
    for (i = 0; i < NUM_HOSTS; i++) {
        if (socks[i] != INVALID_SOCKET) {
            socket_close(socks[i]);
        }
    }
    socket_shutdown();
    
    return 0;
}
```

---

## Best Practices

### 1. Always Check Return Values

```c
/* Good */
if (socket_init() != SOCKET_OK) {
    fprintf(stderr, "Init failed: %s\n", socket_get_error());
    return 1;
}

/* Bad */
socket_init();  /* Ignoring return value */
```

### 2. Close Sockets When Done

```c
/* Good */
socket_close(sock);

/* Bad - leaks socket handle */
/* (socket never closed) */
```

### 3. Handle Partial Receives

```c
/* Good - loop until all data received */
int total = 0;
while (total < expected_length) {
    int rc = socket_recv(sock, buffer + total, expected_length - total);
    if (rc > 0) {
        total += rc;
    } else if (rc == 0) {
        break;  /* Connection closed */
    } else {
        /* Error */
        break;
    }
}

/* Bad - assumes all data in one recv */
int rc = socket_recv(sock, buffer, expected_length);
```

### 4. Use Timeouts for Long Operations

```c
/* Good - implement timeout */
clockTicks_t start = TIMER_GET_CURRENT();
while (1) {
    int rc = socket_recv(sock, buffer, sizeof(buffer));
    if (rc > 0) {
        /* Got data */
        break;
    }
    if (Timer_diff(start, TIMER_GET_CURRENT()) > TIMER_MS_TO_TICKS(5000)) {
        /* Timeout after 5 seconds */
        break;
    }
}
```

### 5. Reuse Sockets

```c
/* Good - reuse socket handles */
for (i = 0; i < num_requests; i++) {
    sock = socket_create(SOCKET_TYPE_TCP);
    socket_connect(sock, host, port);
    /* Use socket */
    socket_close(sock);  /* Free for reuse */
}

/* Bad - creates all sockets at once */
for (i = 0; i < num_requests; i++) {
    socks[i] = socket_create(SOCKET_TYPE_TCP);  /* May run out */
}
```

---

## Memory Management

### Memory Usage

**Per Socket**:
- Socket structure: 10 bytes
- TCP receive buffer: 4,096 bytes
- mTCP internal: ~1-2 KB
- **Total**: ~5-6 KB per socket

**System Overhead**:
- mTCP initialization: ~24 KB
- Packet buffers (10): ~15 KB
- **Total**: ~40 KB

**Maximum 4 Sockets**:
- 4 × 6 KB = 24 KB
- System: 40 KB
- **Total**: ~64 KB

### Memory Behavior

**Important**: mTCP does NOT return memory to DOS

- `socket_close()` frees socket handle but keeps memory in pool
- `socket_shutdown()` does not return memory to DOS
- Memory is reclaimed only when program exits
- This is normal mTCP behavior, not a bug

**Implications**:
- Short-lived programs: No concern
- Long-running programs: Reuse sockets via close/create cycle
- Memory usage is predictable and constant

### Configuration

**Adjusting Socket Count**:

To change MAX_SOCKETS, update three files:

1. **socket.h**:
```c
#define MAX_SOCKETS 4  /* Change this */
```

2. **socket.cpp**:
```cpp
Utils::initStack(4, TCP_SOCKET_RING_SIZE, NULL, NULL);  /* Match above */
```

3. **sample.cfg**:
```c
#define TCP_MAX_SOCKETS (4)  /* Match above */
```

All three must be synchronized!

---

## Troubleshooting

### Problem: "Failed to initialize mTCP stack"

**Causes**:
- MTCP.CFG environment variable not set
- Configuration file not found
- Packet driver not loaded
- TCP_MAX_SOCKETS mismatch

**Solutions**:
1. Set MTCP.CFG: `SET MTCP.CFG=C:\MTCP\MTCP.CFG`
2. Check file exists and is readable
3. Load packet driver (e.g., NE2000 in DOSBox-X)
4. Verify socket.h, socket.cpp, sample.cfg match

### Problem: "Failed to create socket"

**Causes**:
- All MAX_SOCKETS sockets already created
- Socket not closed properly

**Solutions**:
1. Close unused sockets with socket_close()
2. Increase MAX_SOCKETS (see Memory Management)
3. Check for socket leaks in code

### Problem: "Connection timeout"

**Causes**:
- Host unreachable
- Firewall blocking
- DNS resolution failed
- Network not configured

**Solutions**:
1. Verify host is reachable (ping)
2. Check firewall settings
3. Use IP address instead of hostname
4. Verify MTCP.CFG network settings

### Problem: "Connection refused"

**Causes**:
- Server not running
- Wrong port number
- Server firewall blocking

**Solutions**:
1. Verify server is running
2. Check port number
3. Test with telnet or other client

### Problem: Memory not freed after socket_close()

**This is normal behavior!**

mTCP maintains internal memory pools and does not return memory to DOS. Memory is reclaimed when program exits. This is by design for performance.

---

## See Also

- [PHASE4_MEMORY_OPTIMIZATION.md](PHASE4_MEMORY_OPTIMIZATION.md) - Memory usage details
- [PHASE4_NETWORK_PLAN.md](PHASE4_NETWORK_PLAN.md) - Implementation plan
- [sample.cfg](tests/network/sample.cfg) - mTCP configuration
- [test_send_recv.c](tests/network/test_send_recv.c) - Usage example

---

**Document Version**: 1.0  
**Last Updated**: 2026-05-18  
**Phase**: 4.1 Day 13-14