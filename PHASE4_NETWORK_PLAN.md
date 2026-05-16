# Phase 4: Network Socket Implementation Plan

## Overview

Phase 4 implements TCP/IP socket functionality for dosjava, enabling network communication on 16-bit PC-DOS. This phase is divided into two sub-phases:

- **Phase 4.1**: Wattcp Integration and C-level Socket API (2 weeks)
- **Phase 4.2**: Java Socket Classes and VM Integration (2 weeks)

## Goals

### Primary Goals
1. Integrate Wattcp/Watt-32 TCP/IP library with dosjava
2. Implement C-level socket wrapper functions
3. Create Java socket classes (Socket, ServerSocket)
4. Enable network communication from Java code
5. Verify with client-server test programs

### Success Criteria
- C-level socket tests pass (tsock.exe)
- Java client can connect to external servers
- Java server can accept connections
- Data transfer works bidirectionally
- Memory usage stays within DOS limits (640KB)

## Technical Background

### Wattcp/Watt-32 Library
- **Purpose**: BSD Socket-compatible TCP/IP stack for DOS
- **API**: socket(), bind(), listen(), accept(), connect(), send(), recv()
- **Requirements**: Packet driver (e.g., NE2000 emulation in DOSBox-X)
- **License**: Open source (check specific license terms)
- **Documentation**: Available at Wattcp project site

### Network Stack Architecture

```
┌─────────────────────────────────────┐
│   Java Application (*.jav)          │
│   Socket/ServerSocket classes       │
└─────────────────┬───────────────────┘
                  │ Java API
┌─────────────────▼───────────────────┐
│   dosjava VM (djvm.exe)             │
│   - OP_INVOKE_NATIVE bytecode       │
│   - Socket object management        │
└─────────────────┬───────────────────┘
                  │ Native calls
┌─────────────────▼───────────────────┐
│   C Runtime Socket Wrappers         │
│   - socket_create(), socket_bind()  │
│   - socket_connect(), socket_send() │
└─────────────────┬───────────────────┘
                  │ Wattcp API
┌─────────────────▼───────────────────┐
│   Wattcp TCP/IP Stack               │
│   - TCP/IP protocol implementation  │
│   - Buffer management               │
└─────────────────┬───────────────────┘
                  │ Packet Driver API
┌─────────────────▼───────────────────┐
│   Packet Driver (NE2000 emulation)  │
│   - Network interface               │
└─────────────────────────────────────┘
```

## Phase 4.1: Wattcp Integration and C-level Socket API

**Duration**: 2 weeks (Day 1-14)

### Week 1: Wattcp Setup and Basic Socket API

#### Day 1-2: Wattcp Integration
**Goal**: Set up Wattcp library and verify basic functionality

Tasks:
1. Download and install Wattcp/Watt-32 library
2. Configure Open Watcom to link with Wattcp
3. Create test program to verify Wattcp initialization
4. Test packet driver connectivity in DOSBox-X

Deliverables:
- Wattcp library files in appropriate directory
- Updated Makefile with Wattcp linking
- Basic connectivity test (twatt.exe)

#### Day 3-5: C-level Socket Wrapper Functions
**Goal**: Create C wrapper functions for socket operations

Tasks:
1. Design socket wrapper API
2. Implement socket creation and configuration
3. Implement bind/listen/accept for servers
4. Implement connect for clients
5. Add error handling and logging

Files to create:
- `src/runtime/socket.h` - Socket wrapper API declarations
- `src/runtime/socket.c` - Socket wrapper implementations

Key functions:
```c
/* Socket creation and configuration */
int socket_init(void);
int socket_create(int type);  /* TCP or UDP */
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
```

#### Day 6-7: C-level Socket Testing
**Goal**: Verify socket wrapper functions work correctly

Tasks:
1. Create C test program for socket operations
2. Test TCP server (bind, listen, accept)
3. Test TCP client (connect)
4. Test data transfer (send, recv)
5. Test error conditions

Deliverables:
- `tests/network/test_socket.c` - Socket wrapper tests
- `tsock.exe` - Compiled test program
- Test results documentation

### Week 2: Advanced Socket Features and Integration

#### Day 8-10: Non-blocking I/O and Timeouts
**Goal**: Add non-blocking socket support

Tasks:
1. Implement socket_set_nonblocking()
2. Implement socket_set_timeout()
3. Add select() wrapper for multiple sockets
4. Test non-blocking operations

#### Day 11-12: Memory Management and Optimization
**Goal**: Optimize for DOS memory constraints

Tasks:
1. Implement socket buffer management
2. Optimize memory usage
3. Add socket pool for reuse
4. Profile memory consumption

#### Day 13-14: Documentation and Cleanup
**Goal**: Document C-level socket API

Tasks:
1. Write API documentation
2. Create usage examples
3. Clean up debug code
4. Prepare for Phase 4.2

Deliverables:
- `PHASE4_SOCKET_API.md` - C-level API documentation
- Working socket wrapper library
- Comprehensive test suite

## Phase 4.2: Java Socket Classes and VM Integration

**Duration**: 2 weeks (Day 15-28)

### Week 3: Java Socket Classes

#### Day 15-17: Socket and ServerSocket Classes
**Goal**: Implement Java socket classes

Tasks:
1. Design Java socket class hierarchy
2. Implement Socket class
3. Implement ServerSocket class
4. Add SocketInputStream/SocketOutputStream

Files to create:
- `src/runtime/socket_java.h` - Java socket object structures
- `src/runtime/socket_java.c` - Java socket implementations

Java API design:
```java
class Socket {
    Socket(String host, int port);  // Client socket
    void close();
    InputStream getInputStream();
    OutputStream getOutputStream();
}

class ServerSocket {
    ServerSocket(int port);  // Server socket
    Socket accept();
    void close();
}

class SocketInputStream extends InputStream {
    int read();
    int available();
}

class SocketOutputStream extends OutputStream {
    void write(int b);
    void flush();
}
```

#### Day 18-19: Native Method Mechanism
**Goal**: Implement native method calling from VM

Tasks:
1. Add OP_INVOKE_NATIVE bytecode
2. Implement native method registration
3. Create native method dispatcher
4. Add socket native methods

VM changes:
- `src/format/opcodes.h` - Add OP_INVOKE_NATIVE
- `src/vm/interpreter.c` - Implement native method dispatch
- `src/vm/native.h` - Native method registry
- `src/vm/native.c` - Native method implementations

#### Day 20-21: VM Socket Integration
**Goal**: Integrate socket objects with VM

Tasks:
1. Add socket object creation in OP_NEW
2. Implement socket method calls
3. Add socket cleanup on VM shutdown
4. Test socket lifecycle

### Week 4: Testing and Finalization

#### Day 22-24: Java Socket Testing
**Goal**: Create comprehensive Java socket tests

Tasks:
1. Create TCP echo server test (echoserv.jav)
2. Create TCP echo client test (echocli.jav)
3. Test client-server communication
4. Test error handling

Test files:
- `tests/network/echoserv.jav` - Echo server
- `tests/network/echocli.jav` - Echo client
- `tests/network/httpget.jav` - HTTP GET client
- `tests/network/README.md` - Test documentation

#### Day 25-26: Integration Testing
**Goal**: Verify end-to-end functionality

Tasks:
1. Test with external servers (HTTP)
2. Test multiple concurrent connections
3. Test large data transfers
4. Measure performance

#### Day 27-28: Documentation and Cleanup
**Goal**: Complete Phase 4 documentation

Tasks:
1. Write user guide for socket programming
2. Document limitations and known issues
3. Create example programs
4. Clean up and optimize code

Deliverables:
- `PHASE4_NETWORK_QUICKSTART.md` - Quick start guide
- `PHASE4_NETWORK_TASKS.md` - Task checklist
- Working socket implementation
- Example programs

## Memory Considerations

### DOS Memory Constraints
- **Total**: 640KB conventional memory
- **VM**: ~100KB (code + data)
- **Wattcp**: ~50KB (TCP/IP stack)
- **Buffers**: ~50KB (socket buffers)
- **Application**: ~400KB remaining

### Optimization Strategies
1. Use small socket buffers (1-2KB per socket)
2. Limit maximum concurrent connections (4-8)
3. Reuse socket objects when possible
4. Implement buffer pooling
5. Profile and optimize hot paths

## Testing Strategy

### C-level Tests (Phase 4.1)
1. **Unit tests**: Individual socket functions
2. **Integration tests**: Client-server communication
3. **Stress tests**: Multiple connections, large transfers
4. **Error tests**: Network failures, timeouts

### Java-level Tests (Phase 4.2)
1. **API tests**: Socket class methods
2. **Echo tests**: Simple client-server
3. **HTTP tests**: Real-world protocol
4. **Concurrent tests**: Multiple connections

### DOSBox-X Testing
1. Configure NE2000 emulation
2. Set up bridge networking
3. Test with external servers
4. Verify packet driver operation

## Risk Mitigation

### Technical Risks
1. **Memory exhaustion**: Monitor usage, implement limits
2. **Wattcp compatibility**: Test thoroughly, have fallback
3. **Packet driver issues**: Document setup, provide troubleshooting
4. **Performance**: Profile early, optimize critical paths

### Mitigation Strategies
1. Implement memory monitoring and limits
2. Create comprehensive test suite
3. Document all dependencies and setup
4. Provide example configurations

## Success Metrics

### Phase 4.1 Success
- [ ] Wattcp integrated and working
- [ ] C-level socket API implemented
- [ ] tsock.exe passes all tests
- [ ] Memory usage acceptable (<150KB)

### Phase 4.2 Success
- [ ] Java Socket/ServerSocket classes working
- [ ] Native method mechanism functional
- [ ] Echo client-server test passes
- [ ] HTTP GET test retrieves web page

### Overall Phase 4 Success
- [ ] Network communication works reliably
- [ ] Memory usage within DOS limits
- [ ] Performance acceptable for simple protocols
- [ ] Documentation complete and clear

## Next Steps After Phase 4

Potential future enhancements:
1. UDP socket support
2. SSL/TLS support (if feasible)
3. HTTP client library
4. FTP client library
5. DNS resolution support

## References

- Wattcp/Watt-32 documentation
- BSD Socket API reference
- DOSBox-X networking guide
- Open Watcom C library documentation
- Phase 3 I/O implementation (for reference)