# Phase 4: Network Socket Implementation - Quick Start Guide

## Overview

This guide provides a quick introduction to implementing and using network socket functionality in dosjava. Phase 4 adds TCP/IP socket support using the Wattcp library, enabling Java programs to communicate over networks on 16-bit PC-DOS.

## Prerequisites

### Required Software
- Open Watcom V2 (installed at C:\WATCOM)
- Wattcp/Watt-32 library (to be installed at C:\WATTCP)
- DOSBox-X with NE2000 network emulation
- Packet driver for NE2000 (included with DOSBox-X)

### Required Knowledge
- Basic TCP/IP networking concepts
- Socket programming fundamentals
- C programming (for Phase 4.1)
- Java programming (for Phase 4.2)

## Phase 4.1: C-level Socket API

### Step 1: Install Wattcp

1. Download Wattcp/Watt-32 from the official source
2. Extract to C:\WATTCP
3. Verify directory structure:
   ```
   C:\WATTCP\
   ├── INC\          (header files)
   ├── LIB\          (library files)
   └── DOC\          (documentation)
   ```

### Step 2: Update Makefile

Add Wattcp include and library paths:

```makefile
# Wattcp paths
WATTCP_INC = C:\WATTCP\INC
WATTCP_LIB = C:\WATTCP\LIB

# Compiler flags
CFLAGS = -ms -0 -w4 -zq -os -s -i=$(WATCOM_INC) -i=$(WATTCP_INC)

# Linker flags
LDFLAGS = libpath $(WATTCP_LIB)
LIBS = wattcpwf.lib
```

### Step 3: Create Socket Wrapper API

Create `src/runtime/socket.h`:

```c
#ifndef SOCKET_H
#define SOCKET_H

/* Socket types */
#define SOCK_TCP 1
#define SOCK_UDP 2

/* Initialize Wattcp */
int socket_init(void);

/* Socket operations */
int socket_create(int type);
int socket_close(int sockfd);

/* Server operations */
int socket_bind(int sockfd, const char* addr, int port);
int socket_listen(int sockfd, int backlog);
int socket_accept(int sockfd);

/* Client operations */
int socket_connect(int sockfd, const char* addr, int port);

/* Data transfer */
int socket_send(int sockfd, const char* data, int len);
int socket_recv(int sockfd, char* buffer, int len);

/* Utility */
int socket_available(int sockfd);
const char* socket_get_error(void);

#endif /* SOCKET_H */
```

### Step 4: Implement Socket Wrappers

Create `src/runtime/socket.c`:

```c
#include "socket.h"
#include <tcp.h>  /* Wattcp header */
#include <stdio.h>
#include <string.h>

static char error_msg[256];

int socket_init(void) {
    /* Initialize Wattcp */
    sock_init();
    return 0;
}

int socket_create(int type) {
    /* Create socket using Wattcp API */
    tcp_Socket* s = (tcp_Socket*)malloc(sizeof(tcp_Socket));
    if (!s) {
        strcpy(error_msg, "Out of memory");
        return -1;
    }
    
    if (!tcp_open(s, 0, 0, NULL)) {
        free(s);
        strcpy(error_msg, "Failed to create socket");
        return -1;
    }
    
    return (int)s;  /* Return socket handle */
}

/* ... implement other functions ... */
```

### Step 5: Test C-level Sockets

Create `tests/network/test_socket.c`:

```c
#include "../../src/runtime/socket.h"
#include <stdio.h>

int main(void) {
    int result;
    
    printf("Testing socket API...\n");
    
    /* Test 1: Initialize */
    result = socket_init();
    if (result < 0) {
        printf("FAIL: socket_init()\n");
        return 1;
    }
    printf("PASS: socket_init()\n");
    
    /* Test 2: Create socket */
    int sock = socket_create(SOCK_TCP);
    if (sock < 0) {
        printf("FAIL: socket_create()\n");
        return 1;
    }
    printf("PASS: socket_create()\n");
    
    /* Test 3: Close socket */
    result = socket_close(sock);
    if (result < 0) {
        printf("FAIL: socket_close()\n");
        return 1;
    }
    printf("PASS: socket_close()\n");
    
    printf("\nAll tests passed!\n");
    return 0;
}
```

Build and run:
```batch
wmake tsock.exe
tsock.exe
```

## Phase 4.2: Java Socket Classes

### Step 1: Design Java Socket API

```java
// Socket class for client connections
class Socket {
    // Constructor: connect to host:port
    Socket(String host, int port);
    
    // Get input stream for reading
    InputStream getInputStream();
    
    // Get output stream for writing
    OutputStream getOutputStream();
    
    // Close the socket
    void close();
}

// ServerSocket class for accepting connections
class ServerSocket {
    // Constructor: bind to port
    ServerSocket(int port);
    
    // Accept incoming connection
    Socket accept();
    
    // Close the server socket
    void close();
}
```

### Step 2: Implement Native Method Mechanism

Add to `src/format/opcodes.h`:

```c
#define OP_INVOKE_NATIVE 0x50  /* Call native method */
```

Create `src/vm/native.h`:

```c
#ifndef NATIVE_H
#define NATIVE_H

#include <stdint.h>

/* Native method signature */
typedef int (*NativeMethod)(void* vm_context, uint16_t* args, int arg_count);

/* Register native method */
void native_register(const char* class_name, const char* method_name, 
                     NativeMethod func);

/* Call native method */
int native_invoke(const char* class_name, const char* method_name,
                  void* vm_context, uint16_t* args, int arg_count);

#endif /* NATIVE_H */
```

### Step 3: Implement Socket Java Objects

Create `src/runtime/socket_java.h`:

```c
#ifndef SOCKET_JAVA_H
#define SOCKET_JAVA_H

#include <stdint.h>

/* Socket object structure */
typedef struct {
    int sockfd;           /* C-level socket handle */
    uint16_t is_open;     /* Socket state */
    uint16_t input_stream;  /* InputStream object handle */
    uint16_t output_stream; /* OutputStream object handle */
} SocketObject;

/* ServerSocket object structure */
typedef struct {
    int sockfd;           /* C-level socket handle */
    uint16_t is_open;     /* Socket state */
    int port;             /* Bound port */
} ServerSocketObject;

/* Create Socket object */
SocketObject* socket_java_create(const char* host, int port);

/* Create ServerSocket object */
ServerSocketObject* serversocket_java_create(int port);

/* Accept connection */
SocketObject* serversocket_java_accept(ServerSocketObject* server);

/* Close socket */
void socket_java_close(SocketObject* socket);

#endif /* SOCKET_JAVA_H */
```

### Step 4: Write Java Socket Programs

#### Echo Server (echoserv.jav)

```java
class EchoServer {
    public static void main() {
        ServerSocket server;
        Socket client;
        InputStream in;
        OutputStream out;
        int b;
        
        System.out.println("Starting echo server on port 7777...");
        
        server = new ServerSocket(7777);
        System.out.println("Waiting for connection...");
        
        client = server.accept();
        System.out.println("Client connected!");
        
        in = client.getInputStream();
        out = client.getOutputStream();
        
        // Echo loop
        b = in.read();
        while (b != -1) {
            out.write(b);
            b = in.read();
        }
        
        client.close();
        server.close();
        System.out.println("Server closed");
    }
}
```

#### Echo Client (echocli.jav)

```java
class EchoClient {
    public static void main() {
        Socket socket;
        OutputStream out;
        InputStream in;
        String msg;
        int i;
        int b;
        
        System.out.println("Connecting to localhost:7777...");
        
        socket = new Socket("127.0.0.1", 7777);
        System.out.println("Connected!");
        
        out = socket.getOutputStream();
        in = socket.getInputStream();
        
        // Send message
        msg = "Hello, Server!";
        i = 0;
        while (i < 14) {  // Length of "Hello, Server!"
            out.write(msg.charAt(i));
            i = i + 1;
        }
        
        // Read echo
        System.out.print("Echo: ");
        i = 0;
        while (i < 14) {
            b = in.read();
            System.out.print((char)b);
            i = i + 1;
        }
        System.out.println("");
        
        socket.close();
        System.out.println("Client closed");
    }
}
```

#### HTTP GET Client (httpget.jav)

```java
class HttpGet {
    public static void main() {
        Socket socket;
        OutputStream out;
        InputStream in;
        String request;
        int i;
        int b;
        
        System.out.println("Connecting to example.com:80...");
        
        socket = new Socket("93.184.216.34", 80);  // example.com IP
        System.out.println("Connected!");
        
        out = socket.getOutputStream();
        in = socket.getInputStream();
        
        // Send HTTP GET request
        request = "GET / HTTP/1.0\r\n\r\n";
        i = 0;
        while (i < 18) {  // Length of request
            out.write(request.charAt(i));
            i = i + 1;
        }
        
        // Read response
        System.out.println("Response:");
        b = in.read();
        while (b != -1) {
            System.out.print((char)b);
            b = in.read();
        }
        
        socket.close();
        System.out.println("\nClient closed");
    }
}
```

### Step 5: Compile and Test

```batch
REM Compile echo server
djc echoserv.jav

REM Compile echo client
djc echocli.jav

REM Run server in one DOSBox-X instance
djvm echoserv.djc

REM Run client in another DOSBox-X instance
djvm echocli.djc
```

## DOSBox-X Network Setup

### Configure NE2000 Emulation

Edit DOSBox-X configuration file:

```ini
[ne2000]
ne2000=true
nicbase=300
nicirq=3
macaddr=AC:DE:48:88:99:AA
realnic=auto
```

### Load Packet Driver

In DOSBox-X:
```batch
C:\> ne2000 0x60 3 0x300
```

### Configure Wattcp

Create `WATTCP.CFG`:
```
my_ip = 192.168.1.100
netmask = 255.255.255.0
gateway = 192.168.1.1
nameserver = 8.8.8.8
```

### Test Connectivity

```batch
C:\> ping 192.168.1.1
C:\> ping 8.8.8.8
```

## Common Issues and Solutions

### Issue 1: "Packet driver not found"
**Solution**: Load NE2000 packet driver before running programs
```batch
ne2000 0x60 3 0x300
```

### Issue 2: "Connection refused"
**Solution**: Check firewall settings, verify server is running

### Issue 3: "Out of memory"
**Solution**: Reduce socket buffer sizes, limit concurrent connections

### Issue 4: "Wattcp initialization failed"
**Solution**: Check WATTCP.CFG file, verify network configuration

## Performance Tips

1. **Use small buffers**: 1-2KB per socket
2. **Limit connections**: 4-8 concurrent max
3. **Reuse sockets**: Implement socket pooling
4. **Avoid blocking**: Use timeouts or non-blocking mode
5. **Profile memory**: Monitor usage regularly

## Memory Usage Guidelines

- **Wattcp stack**: ~50KB
- **Per socket**: ~2-4KB (buffers + overhead)
- **VM overhead**: ~100KB
- **Application**: Remaining ~400KB

**Total budget**: 640KB conventional memory

## Next Steps

1. Complete Phase 4.1 (C-level socket API)
2. Test thoroughly with tsock.exe
3. Implement Phase 4.2 (Java socket classes)
4. Test with echo server/client
5. Test with HTTP GET client
6. Optimize and document

## References

- Wattcp documentation: http://www.wattcp.com/
- DOSBox-X networking: https://dosbox-x.com/wiki/
- BSD Socket API: Standard Unix socket programming
- Phase 3 I/O implementation: See PHASE3_IO_QUICKSTART.md

## Support

For issues or questions:
1. Check PHASE4_NETWORK_PLAN.md for detailed information
2. Review PHASE4_NETWORK_TASKS.md for implementation checklist
3. Consult Wattcp documentation
4. Test in DOSBox-X with network debugging enabled