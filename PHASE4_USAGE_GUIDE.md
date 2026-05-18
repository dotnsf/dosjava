# Socket Usage and Troubleshooting Guide

## Overview

This guide provides practical examples, best practices, and troubleshooting information for using the dosjava socket API in 16-bit PC-DOS applications.

**Target**: 16-bit PC-DOS (640KB RAM)  
**Library**: mTCP TCP/IP stack  
**Phase**: 4.1

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Usage Patterns](#basic-usage-patterns)
3. [Advanced Techniques](#advanced-techniques)
4. [Error Handling](#error-handling)
5. [Performance Tips](#performance-tips)
6. [Common Problems](#common-problems)
7. [Debugging Techniques](#debugging-techniques)
8. [Real-World Examples](#real-world-examples)

---

## Getting Started

### Prerequisites

**Required**:
1. Open Watcom V2 installed at C:\WATCOM
2. mTCP library files in dosjava/src/network/mtcp/
3. Packet driver loaded (e.g., NE2000 in DOSBox-X)
4. MTCP.CFG environment variable set

**Verification**:

```batch
REM Check Watcom
DIR C:\WATCOM\BINW\WCL.EXE

REM Check mTCP files
DIR dosjava\src\network\mtcp\*.H

REM Check environment
ECHO %MTCP.CFG%

REM Check packet driver (in DOS)
PING 127.0.0.1
```

### First Program

**hello_socket.c**:

```c
#include <stdio.h>
#include "socket.h"

int main(void) {
    /* Initialize socket subsystem */
    if (socket_init() != SOCKET_OK) {
        printf("Failed to initialize: %s\n", socket_get_error());
        return 1;
    }
    
    printf("Socket subsystem initialized successfully!\n");
    printf("mTCP stack is ready.\n");
    
    /* Cleanup */
    socket_shutdown();
    return 0;
}
```

**Build**:
```batch
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINW;%PATH%
SET INCLUDE=%WATCOM%\H
WCL -ms -I.\src\network -I.\src\network\mtcp hello_socket.c socket.cpp mtcp\*.cpp
```

**Run** (in DOSBox-X):
```
SET MTCP.CFG=C:\MTCP\MTCP.CFG
hello_socket.exe
```

**Expected Output**:
```
Socket subsystem initialized successfully!
mTCP stack is ready.
```

---

## Basic Usage Patterns

### Pattern 1: Simple HTTP GET

**Purpose**: Fetch a web page

```c
#include <stdio.h>
#include <string.h>
#include "socket.h"

int http_get(const char* host, int port, const char* path) {
    socket_handle_t sock;
    char request[256];
    char response[2048];
    int rc, total = 0;
    
    /* Create socket */
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Create failed: %s\n", socket_get_error());
        return -1;
    }
    
    /* Connect */
    printf("Connecting to %s:%d...\n", host, port);
    rc = socket_connect(sock, host, port);
    if (rc != SOCKET_OK) {
        printf("Connect failed: %s\n", socket_get_error());
        socket_close(sock);
        return -1;
    }
    printf("Connected!\n");
    
    /* Build HTTP request */
    sprintf(request, 
            "GET %s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host);
    
    /* Send request */
    printf("Sending request...\n");
    rc = socket_send(sock, request, strlen(request));
    if (rc < 0) {
        printf("Send failed: %s\n", socket_get_error());
        socket_close(sock);
        return -1;
    }
    printf("Sent %d bytes\n", rc);
    
    /* Receive response */
    printf("Receiving response...\n");
    while (1) {
        rc = socket_recv(sock, response, sizeof(response) - 1);
        if (rc > 0) {
            response[rc] = '\0';
            printf("%s", response);
            total += rc;
        } else if (rc == 0) {
            /* Connection closed */
            break;
        } else {
            printf("Receive error: %s\n", socket_get_error());
            break;
        }
    }
    
    printf("\n\nTotal received: %d bytes\n", total);
    
    /* Cleanup */
    socket_close(sock);
    return total;
}

int main(void) {
    if (socket_init() != SOCKET_OK) {
        printf("Init failed\n");
        return 1;
    }
    
    http_get("192.168.1.100", 80, "/");
    
    socket_shutdown();
    return 0;
}
```

### Pattern 2: Download File

**Purpose**: Download and save file

```c
#include <stdio.h>
#include <string.h>
#include "socket.h"

int download_file(const char* host, int port, 
                  const char* path, const char* filename) {
    socket_handle_t sock;
    FILE* fp;
    char request[256];
    char buffer[4096];
    int rc, total = 0;
    int header_done = 0;
    
    /* Create socket */
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) return -1;
    
    /* Connect */
    if (socket_connect(sock, host, port) != SOCKET_OK) {
        socket_close(sock);
        return -1;
    }
    
    /* Send HTTP request */
    sprintf(request, 
            "GET %s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host);
    
    if (socket_send(sock, request, strlen(request)) < 0) {
        socket_close(sock);
        return -1;
    }
    
    /* Open output file */
    fp = fopen(filename, "wb");
    if (!fp) {
        printf("Failed to open %s\n", filename);
        socket_close(sock);
        return -1;
    }
    
    /* Receive and save */
    while (1) {
        rc = socket_recv(sock, buffer, sizeof(buffer));
        if (rc > 0) {
            if (!header_done) {
                /* Skip HTTP headers */
                char* body = strstr(buffer, "\r\n\r\n");
                if (body) {
                    body += 4;
                    fwrite(body, 1, rc - (body - buffer), fp);
                    header_done = 1;
                }
            } else {
                fwrite(buffer, 1, rc, fp);
            }
            total += rc;
            printf("\rReceived: %d bytes", total);
        } else if (rc == 0) {
            break;
        } else {
            printf("\nReceive error\n");
            break;
        }
    }
    
    printf("\nDownload complete: %d bytes\n", total);
    
    fclose(fp);
    socket_close(sock);
    return total;
}
```

### Pattern 3: Multiple Connections

**Purpose**: Parallel requests

```c
#include <stdio.h>
#include "socket.h"

#define MAX_HOSTS 4

typedef struct {
    socket_handle_t sock;
    const char* host;
    int port;
    int connected;
    int bytes_received;
} connection_t;

int main(void) {
    connection_t conns[MAX_HOSTS];
    const char* hosts[] = {
        "192.168.1.100",
        "192.168.1.101",
        "192.168.1.102",
        "192.168.1.103"
    };
    char buffer[1024];
    int i, active, rc;
    
    /* Initialize */
    if (socket_init() != SOCKET_OK) {
        printf("Init failed\n");
        return 1;
    }
    
    /* Create and connect all sockets */
    for (i = 0; i < MAX_HOSTS; i++) {
        conns[i].host = hosts[i];
        conns[i].port = 80;
        conns[i].connected = 0;
        conns[i].bytes_received = 0;
        
        conns[i].sock = socket_create(SOCKET_TYPE_TCP);
        if (conns[i].sock == INVALID_SOCKET) {
            printf("Failed to create socket %d\n", i);
            continue;
        }
        
        printf("Connecting to %s...\n", hosts[i]);
        if (socket_connect(conns[i].sock, hosts[i], 80) == SOCKET_OK) {
            conns[i].connected = 1;
            printf("Connected to %s\n", hosts[i]);
        } else {
            printf("Failed to connect to %s\n", hosts[i]);
            socket_close(conns[i].sock);
            conns[i].sock = INVALID_SOCKET;
        }
    }
    
    /* Send requests to all */
    for (i = 0; i < MAX_HOSTS; i++) {
        if (conns[i].connected) {
            const char* req = "GET / HTTP/1.0\r\n\r\n";
            socket_send(conns[i].sock, req, strlen(req));
        }
    }
    
    /* Receive from all (round-robin) */
    active = MAX_HOSTS;
    while (active > 0) {
        for (i = 0; i < MAX_HOSTS; i++) {
            if (!conns[i].connected) continue;
            
            rc = socket_recv(conns[i].sock, buffer, sizeof(buffer) - 1);
            if (rc > 0) {
                conns[i].bytes_received += rc;
                printf("[%s] Received %d bytes (total: %d)\n",
                       conns[i].host, rc, conns[i].bytes_received);
            } else if (rc == 0) {
                printf("[%s] Connection closed (total: %d bytes)\n",
                       conns[i].host, conns[i].bytes_received);
                socket_close(conns[i].sock);
                conns[i].connected = 0;
                active--;
            }
        }
    }
    
    /* Cleanup */
    socket_shutdown();
    return 0;
}
```

### Pattern 4: Connection with Timeout

**Purpose**: Avoid hanging on slow connections

```c
#include <stdio.h>
#include "socket.h"

/* Note: Requires mTCP timer functions */
#include "timer.h"

int recv_with_timeout(socket_handle_t sock, void* buffer, 
                      unsigned int length, unsigned int timeout_ms) {
    clockTicks_t start = TIMER_GET_CURRENT();
    clockTicks_t timeout_ticks = TIMER_MS_TO_TICKS(timeout_ms);
    int rc;
    
    while (1) {
        rc = socket_recv(sock, buffer, length);
        if (rc != 0) {
            /* Got data or error */
            return rc;
        }
        
        /* Check timeout */
        if (Timer_diff(start, TIMER_GET_CURRENT()) > timeout_ticks) {
            return -1;  /* Timeout */
        }
        
        /* Small delay to avoid busy-wait */
        delay(10);
    }
}

int main(void) {
    socket_handle_t sock;
    char buffer[1024];
    int rc;
    
    socket_init();
    
    sock = socket_create(SOCKET_TYPE_TCP);
    socket_connect(sock, "192.168.1.100", 80);
    
    /* Send request */
    const char* req = "GET / HTTP/1.0\r\n\r\n";
    socket_send(sock, req, strlen(req));
    
    /* Receive with 5-second timeout */
    rc = recv_with_timeout(sock, buffer, sizeof(buffer) - 1, 5000);
    if (rc > 0) {
        buffer[rc] = '\0';
        printf("Received: %s\n", buffer);
    } else if (rc == 0) {
        printf("Connection closed\n");
    } else {
        printf("Timeout or error\n");
    }
    
    socket_close(sock);
    socket_shutdown();
    return 0;
}
```

---

## Advanced Techniques

### Technique 1: Connection Pooling

**Purpose**: Reuse sockets efficiently

```c
typedef struct {
    socket_handle_t sock;
    int in_use;
    char host[64];
    int port;
} pool_entry_t;

#define POOL_SIZE 4
pool_entry_t g_pool[POOL_SIZE];

void pool_init(void) {
    int i;
    for (i = 0; i < POOL_SIZE; i++) {
        g_pool[i].sock = INVALID_SOCKET;
        g_pool[i].in_use = 0;
    }
}

socket_handle_t pool_get(const char* host, int port) {
    int i;
    
    /* Check for existing connection */
    for (i = 0; i < POOL_SIZE; i++) {
        if (g_pool[i].in_use && 
            strcmp(g_pool[i].host, host) == 0 &&
            g_pool[i].port == port) {
            return g_pool[i].sock;
        }
    }
    
    /* Find free slot */
    for (i = 0; i < POOL_SIZE; i++) {
        if (!g_pool[i].in_use) {
            g_pool[i].sock = socket_create(SOCKET_TYPE_TCP);
            if (g_pool[i].sock == INVALID_SOCKET) {
                return INVALID_SOCKET;
            }
            
            if (socket_connect(g_pool[i].sock, host, port) != SOCKET_OK) {
                socket_close(g_pool[i].sock);
                return INVALID_SOCKET;
            }
            
            strcpy(g_pool[i].host, host);
            g_pool[i].port = port;
            g_pool[i].in_use = 1;
            return g_pool[i].sock;
        }
    }
    
    return INVALID_SOCKET;  /* Pool full */
}

void pool_release(socket_handle_t sock) {
    int i;
    for (i = 0; i < POOL_SIZE; i++) {
        if (g_pool[i].sock == sock) {
            socket_close(sock);
            g_pool[i].in_use = 0;
            break;
        }
    }
}
```

### Technique 2: Chunked Transfer

**Purpose**: Handle large data transfers

```c
int send_large_data(socket_handle_t sock, const void* data, 
                    unsigned long total_size) {
    const unsigned char* ptr = (const unsigned char*)data;
    unsigned long sent = 0;
    int rc;
    
    printf("Sending %lu bytes...\n", total_size);
    
    while (sent < total_size) {
        unsigned int chunk_size = (total_size - sent > 4096) ? 
                                  4096 : (total_size - sent);
        
        rc = socket_send(sock, ptr + sent, chunk_size);
        if (rc < 0) {
            printf("Send failed at %lu bytes\n", sent);
            return -1;
        }
        
        sent += rc;
        printf("\rSent: %lu / %lu bytes (%.1f%%)", 
               sent, total_size, (sent * 100.0) / total_size);
    }
    
    printf("\nSend complete\n");
    return 0;
}
```

### Technique 3: Non-blocking Polling

**Purpose**: Check multiple sockets without blocking

```c
typedef struct {
    socket_handle_t sock;
    int active;
} poll_entry_t;

int poll_sockets(poll_entry_t* entries, int count, int timeout_ms) {
    clockTicks_t start = TIMER_GET_CURRENT();
    clockTicks_t timeout = TIMER_MS_TO_TICKS(timeout_ms);
    int i, ready = 0;
    char dummy[1];
    
    while (1) {
        for (i = 0; i < count; i++) {
            if (!entries[i].active) continue;
            
            if (socket_can_read(entries[i].sock) > 0) {
                ready++;
            }
        }
        
        if (ready > 0) return ready;
        
        if (Timer_diff(start, TIMER_GET_CURRENT()) > timeout) {
            return 0;  /* Timeout */
        }
        
        delay(10);
    }
}
```

---

## Error Handling

### Comprehensive Error Handling

```c
int robust_connect(const char* host, int port, int retries) {
    socket_handle_t sock;
    int attempt;
    
    for (attempt = 0; attempt < retries; attempt++) {
        printf("Connection attempt %d/%d...\n", attempt + 1, retries);
        
        sock = socket_create(SOCKET_TYPE_TCP);
        if (sock == INVALID_SOCKET) {
            printf("Create failed: %s\n", socket_get_error());
            delay(1000);
            continue;
        }
        
        if (socket_connect(sock, host, port) == SOCKET_OK) {
            printf("Connected successfully\n");
            return sock;
        }
        
        printf("Connect failed: %s\n", socket_get_error());
        socket_close(sock);
        
        if (attempt < retries - 1) {
            printf("Retrying in 2 seconds...\n");
            delay(2000);
        }
    }
    
    printf("All connection attempts failed\n");
    return INVALID_SOCKET;
}
```

### Error Recovery

```c
int send_with_recovery(socket_handle_t sock, const void* data, 
                       unsigned int length) {
    int rc = socket_send(sock, data, length);
    
    if (rc < 0) {
        /* Check socket state */
        int state = socket_get_state(sock);
        
        if (state == SOCKET_STATE_CLOSED) {
            printf("Socket closed, cannot recover\n");
            return -1;
        }
        
        if (state == SOCKET_STATE_CLOSING) {
            printf("Socket closing, waiting...\n");
            delay(1000);
            
            /* Retry once */
            rc = socket_send(sock, data, length);
        }
    }
    
    return rc;
}
```

---

## Performance Tips

### Tip 1: Batch Small Sends

**Bad**:
```c
/* Multiple small sends - inefficient */
socket_send(sock, "GET ", 4);
socket_send(sock, "/", 1);
socket_send(sock, " HTTP/1.0\r\n", 11);
```

**Good**:
```c
/* Single send - efficient */
char request[256];
sprintf(request, "GET / HTTP/1.0\r\n");
socket_send(sock, request, strlen(request));
```

### Tip 2: Use Larger Buffers

**Bad**:
```c
char buffer[128];  /* Too small */
while (1) {
    int rc = socket_recv(sock, buffer, sizeof(buffer));
    /* Many recv() calls needed */
}
```

**Good**:
```c
char buffer[4096];  /* Optimal size */
while (1) {
    int rc = socket_recv(sock, buffer, sizeof(buffer));
    /* Fewer recv() calls */
}
```

### Tip 3: Reuse Connections

**Bad**:
```c
/* Create/destroy for each request */
for (i = 0; i < 10; i++) {
    sock = socket_create(SOCKET_TYPE_TCP);
    socket_connect(sock, host, port);
    /* Use socket */
    socket_close(sock);
}
```

**Good**:
```c
/* Reuse connection */
sock = socket_create(SOCKET_TYPE_TCP);
socket_connect(sock, host, port);
for (i = 0; i < 10; i++) {
    /* Use socket */
}
socket_close(sock);
```

---

## Common Problems

### Problem 1: "Failed to initialize mTCP stack"

**Symptoms**:
- socket_init() returns SOCKET_ERR_INIT
- Error message: "Failed to initialize mTCP stack"

**Causes**:
1. MTCP.CFG not set
2. Configuration file not found
3. Packet driver not loaded
4. Configuration mismatch

**Solutions**:

```batch
REM 1. Set environment variable
SET MTCP.CFG=C:\MTCP\MTCP.CFG

REM 2. Verify file exists
TYPE %MTCP.CFG%

REM 3. Check packet driver (should respond)
PING 127.0.0.1

REM 4. Verify configuration
REM Check TCP_MAX_SOCKETS in sample.cfg matches socket.h
```

### Problem 2: "Failed to create socket"

**Symptoms**:
- socket_create() returns INVALID_SOCKET
- Works for first N sockets, then fails

**Causes**:
- All MAX_SOCKETS sockets in use
- Socket not closed properly

**Solutions**:

```c
/* Check socket count */
int count = 0;
for (i = 0; i < MAX_SOCKETS; i++) {
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock != INVALID_SOCKET) {
        count++;
        socket_close(sock);
    }
}
printf("Available sockets: %d\n", count);

/* Always close sockets */
if (sock != INVALID_SOCKET) {
    socket_close(sock);
}
```

### Problem 3: Connection Timeout

**Symptoms**:
- socket_connect() takes 60 seconds then fails
- Error: "Connection timeout"

**Causes**:
- Host unreachable
- Wrong IP address
- Firewall blocking
- Network not configured

**Solutions**:

```batch
REM Test connectivity
PING 192.168.1.100

REM Check routing
ROUTE PRINT

REM Verify DNS (if using hostname)
NSLOOKUP www.example.com

REM Try IP address instead of hostname
```

```c
/* Use IP address directly */
socket_connect(sock, "192.168.1.100", 80);  /* Instead of hostname */
```

### Problem 4: Partial Data Received

**Symptoms**:
- socket_recv() returns less than expected
- Data appears truncated

**Cause**:
- This is normal TCP behavior!
- Must loop to receive all data

**Solution**:

```c
/* Correct way to receive known length */
int recv_exact(socket_handle_t sock, void* buffer, int length) {
    int total = 0;
    int rc;
    
    while (total < length) {
        rc = socket_recv(sock, (char*)buffer + total, length - total);
        if (rc > 0) {
            total += rc;
        } else if (rc == 0) {
            return total;  /* Connection closed */
        } else {
            return -1;  /* Error */
        }
    }
    
    return total;
}
```

### Problem 5: Memory Leak

**Symptoms**:
- Available memory decreases over time
- Eventually runs out of memory

**Causes**:
- Sockets not closed
- socket_shutdown() not called

**Solution**:

```c
/* Always cleanup */
int main(void) {
    socket_handle_t sock;
    
    if (socket_init() != SOCKET_OK) {
        return 1;
    }
    
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock != INVALID_SOCKET) {
        /* Use socket */
        socket_close(sock);  /* IMPORTANT */
    }
    
    socket_shutdown();  /* IMPORTANT */
    return 0;
}
```

---

## Debugging Techniques

### Technique 1: Enable Verbose Output

```c
#define DEBUG_SOCKET 1

#ifdef DEBUG_SOCKET
#define SOCK_DEBUG(fmt, ...) printf("[SOCKET] " fmt "\n", ##__VA_ARGS__)
#else
#define SOCK_DEBUG(fmt, ...)
#endif

int main(void) {
    SOCK_DEBUG("Initializing...");
    if (socket_init() != SOCKET_OK) {
        SOCK_DEBUG("Init failed: %s", socket_get_error());
        return 1;
    }
    SOCK_DEBUG("Init OK");
    
    /* ... */
}
```

### Technique 2: Log All Operations

```c
FILE* g_logfile = NULL;

void log_init(void) {
    g_logfile = fopen("socket.log", "w");
}

void log_msg(const char* fmt, ...) {
    if (g_logfile) {
        va_list args;
        va_start(args, fmt);
        vfprintf(g_logfile, fmt, args);
        fprintf(g_logfile, "\n");
        fflush(g_logfile);
        va_end(args);
    }
}

void log_close(void) {
    if (g_logfile) {
        fclose(g_logfile);
    }
}

/* Usage */
log_init();
log_msg("Connecting to %s:%d", host, port);
rc = socket_connect(sock, host, port);
log_msg("Connect result: %d", rc);
log_close();
```

### Technique 3: Dump Data

```c
void dump_hex(const void* data, int length) {
    const unsigned char* p = (const unsigned char*)data;
    int i;
    
    printf("Data dump (%d bytes):\n", length);
    for (i = 0; i < length; i++) {
        printf("%02X ", p[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (length % 16 != 0) printf("\n");
}

/* Usage */
char buffer[256];
int rc = socket_recv(sock, buffer, sizeof(buffer));
if (rc > 0) {
    dump_hex(buffer, rc);
}
```

### Technique 4: Monitor Socket State

```c
const char* state_name(int state) {
    switch (state) {
        case SOCKET_STATE_CLOSED: return "CLOSED";
        case SOCKET_STATE_LISTENING: return "LISTENING";
        case SOCKET_STATE_CONNECTING: return "CONNECTING";
        case SOCKET_STATE_CONNECTED: return "CONNECTED";
        case SOCKET_STATE_CLOSING: return "CLOSING";
        default: return "UNKNOWN";
    }
}

void monitor_socket(socket_handle_t sock) {
    int state = socket_get_state(sock);
    printf("Socket %d state: %s\n", sock, state_name(state));
}
```

---

## Real-World Examples

### Example 1: Simple Web Client

**Purpose**: Fetch and display web page

**File**: web_client.c

```c
#include <stdio.h>
#include <string.h>
#include "socket.h"

int main(int argc, char* argv[]) {
    socket_handle_t sock;
    char request[512];
    char response[4096];
    const char* host;
    const char* path;
    int rc, total = 0;
    
    if (argc < 2) {
        printf("Usage: web_client <host> [path]\n");
        printf("Example: web_client 192.168.1.100 /index.html\n");
        return 1;
    }
    
    host = argv[1];
    path = (argc > 2) ? argv[2] : "/";
    
    /* Initialize */
    printf("Initializing socket subsystem...\n");
    if (socket_init() != SOCKET_OK) {
        printf("Failed to initialize: %s\n", socket_get_error());
        return 1;
    }
    
    /* Create socket */
    printf("Creating socket...\n");
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Failed to create socket: %s\n", socket_get_error());
        socket_shutdown();
        return 1;
    }
    
    /* Connect */
    printf("Connecting to %s:80...\n", host);
    rc = socket_connect(sock, host, 80);
    if (rc != SOCKET_OK) {
        printf("Failed to connect: %s\n", socket_get_error());
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    printf("Connected!\n\n");
    
    /* Build and send HTTP request */
    sprintf(request,
            "GET %s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: dosjava-web-client/1.0\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host);
    
    printf("Sending request:\n%s\n", request);
    rc = socket_send(sock, request, strlen(request));
    if (rc < 0) {
        printf("Failed to send: %s\n", socket_get_error());
        socket_close(sock);
        socket_shutdown();
        return 1;
    }
    
    /* Receive and display response */
    printf("Receiving response...\n");
    printf("----------------------------------------\n");
    while (1) {
        rc = socket_recv(sock, response, sizeof(response) - 1);
        if (rc > 0) {
            response[rc] = '\0';
            printf("%s", response);
            total += rc;
        } else if (rc == 0) {
            break;
        } else {
            printf("\nReceive error: %s\n", socket_get_error());
            break;
        }
    }
    printf("\n----------------------------------------\n");
    printf("Total received: %d bytes\n", total);
    
    /* Cleanup */
    socket_close(sock);
    socket_shutdown();
    
    return 0;
}
```

**Build**:
```batch
wmake web_client
```

**Run**:
```
web_client 192.168.1.100 /
```

### Example 2: File Downloader

**Purpose**: Download file from HTTP server

**File**: downloader.c

```c
#include <stdio.h>
#include <string.h>
#include "socket.h"

int download(const char* host, const char* path, const char* output) {
    socket_handle_t sock;
    FILE* fp;
    char request[512];
    char buffer[4096];
    int rc, total = 0, header_done = 0;
    
    /* Create socket and connect */
    sock = socket_create(SOCKET_TYPE_TCP);
    if (sock == INVALID_SOCKET) return -1;
    
    printf("Connecting to %s...\n", host);
    if (socket_connect(sock, host, 80) != SOCKET_OK) {
        printf("Connection failed: %s\n", socket_get_error());
        socket_close(sock);
        return -1;
    }
    
    /* Send HTTP request */
    sprintf(request,
            "GET %s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n",
            path, host);
    
    if (socket_send(sock, request, strlen(request)) < 0) {
        printf("Send failed: %s\n", socket_get_error());
        socket_close(sock);
        return -1;
    }
    
    /* Open output file */
    fp = fopen(output, "wb");
    if (!fp) {
        printf("Failed to open %s for writing\n", output);
        socket_close(sock);
        return -1;
    }
    
    printf("Downloading to %s...\n", output);
    
    /* Receive and save */
    while (1) {
        rc = socket_recv(sock, buffer, sizeof(buffer));
        if (rc > 0) {
            if (!header_done) {
                /* Skip HTTP headers */
                char* body = strstr(buffer, "\r\n\r\n");
                if (body) {
                    body += 4;
                    int body_len = rc - (body - buffer);
                    fwrite(body, 1, body_len, fp);
                    total += body_len;
                    header_done = 1;
                }
            } else {
                fwrite(buffer, 1, rc, fp);
                total += rc;
            }
            printf("\rReceived: %d bytes", total);
            fflush(stdout);
        } else if (rc == 0) {
            break;
        } else {
            printf("\nReceive error: %s\n", socket_get_error());
            break;
        }
    }
    
    printf("\nDownload complete: %d bytes saved to %s\n", total, output);
    
    fclose(fp);
    socket_close(sock);
    return total;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: downloader <host> <path> <output>\n");
        printf("Example: downloader 192.168.1.100 /file.txt file.txt\n");
        return 1;
    }
    
    if (socket_init() != SOCKET_OK) {
        printf("Init failed: %s\n", socket_get_error());
        return 1;
    }
    
    download(argv[1], argv[2], argv[3]);
    
    socket_shutdown();
    return 0;
}
```

---

## See Also

- [PHASE4_SOCKET_API.md](PHASE4_SOCKET_API.md) - Complete API reference
- [PHASE4_CONFIGURATION_GUIDE.md](PHASE4_CONFIGURATION_GUIDE.md) - Configuration details
- [PHASE4_MEMORY_OPTIMIZATION.md](PHASE4_MEMORY_OPTIMIZATION.md) - Memory management

---

**Document Version**: 1.0  
**Last Updated**: 2026-05-18  
**Phase**: 4.1 Day 13-14