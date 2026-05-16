# Phase 4: Network Socket Implementation - Task Checklist

## Phase 4.1: Wattcp Integration and C-level Socket API (Week 1-2)

### Week 1: Wattcp Setup and Basic Socket API

#### Day 1-2: Wattcp Integration
- [ ] Download Wattcp/Watt-32 library
- [ ] Install Wattcp to appropriate directory (e.g., C:\WATTCP)
- [ ] Update Makefile to include Wattcp headers (-i=C:\WATTCP\INC)
- [ ] Update Makefile to link Wattcp library (wattcpwf.lib)
- [ ] Create basic Wattcp initialization test
- [ ] Test packet driver in DOSBox-X (NE2000 emulation)
- [ ] Verify TCP/IP connectivity (ping test)
- [ ] Build and test twatt.exe

**Deliverables:**
- Wattcp library installed
- Makefile updated
- twatt.exe working

#### Day 3-5: C-level Socket Wrapper Functions
- [ ] Create `src/runtime/socket.h` header file
- [ ] Create `src/runtime/socket.c` implementation file
- [ ] Implement `socket_init()` - Initialize Wattcp
- [ ] Implement `socket_create()` - Create TCP socket
- [ ] Implement `socket_close()` - Close socket
- [ ] Implement `socket_bind()` - Bind to address/port
- [ ] Implement `socket_listen()` - Listen for connections
- [ ] Implement `socket_accept()` - Accept connection
- [ ] Implement `socket_connect()` - Connect to server
- [ ] Implement `socket_send()` - Send data
- [ ] Implement `socket_recv()` - Receive data
- [ ] Implement `socket_available()` - Check available data
- [ ] Implement `socket_get_error()` - Get error message
- [ ] Add error handling for all functions
- [ ] Add debug logging (conditional compilation)

**Deliverables:**
- socket.h with complete API
- socket.c with all functions implemented
- Error handling in place

#### Day 6-7: C-level Socket Testing
- [ ] Create `tests/network/` directory
- [ ] Create `tests/network/test_socket.c`
- [ ] Test 1: Socket initialization
- [ ] Test 2: Socket creation and close
- [ ] Test 3: Server bind and listen
- [ ] Test 4: Client connect
- [ ] Test 5: Data send/receive
- [ ] Test 6: Multiple connections
- [ ] Test 7: Error conditions
- [ ] Test 8: Socket cleanup
- [ ] Update Makefile to build tsock.exe
- [ ] Run all tests in DOSBox-X
- [ ] Document test results

**Deliverables:**
- test_socket.c with 8 tests
- tsock.exe passing all tests
- Test results documented

### Week 2: Advanced Socket Features and Integration

#### Day 8-10: Non-blocking I/O and Timeouts
- [ ] Implement `socket_set_nonblocking()` function
- [ ] Implement `socket_set_timeout()` function
- [ ] Implement `socket_select()` wrapper (if needed)
- [ ] Test non-blocking socket operations
- [ ] Test timeout behavior
- [ ] Add tests for non-blocking mode
- [ ] Document non-blocking API usage

**Deliverables:**
- Non-blocking socket support
- Timeout support
- Tests passing

#### Day 11-12: Memory Management and Optimization
- [ ] Implement socket buffer pool
- [ ] Optimize buffer sizes for DOS (1-2KB)
- [ ] Add socket object reuse mechanism
- [ ] Profile memory usage with test programs
- [ ] Optimize Wattcp buffer configuration
- [ ] Test with multiple concurrent sockets
- [ ] Verify memory usage < 150KB total
- [ ] Document memory optimization strategies

**Deliverables:**
- Optimized memory usage
- Socket pooling working
- Memory profiling results

#### Day 13-14: Documentation and Cleanup
- [ ] Create `PHASE4_SOCKET_API.md` documentation
- [ ] Document all socket wrapper functions
- [ ] Create usage examples
- [ ] Document error codes and handling
- [ ] Remove debug printf statements
- [ ] Clean up code formatting
- [ ] Add code comments
- [ ] Review and test all functionality
- [ ] Prepare for Phase 4.2

**Deliverables:**
- PHASE4_SOCKET_API.md complete
- Code cleaned and documented
- Ready for Java integration

## Phase 4.2: Java Socket Classes and VM Integration (Week 3-4)

### Week 3: Java Socket Classes

#### Day 15-17: Socket and ServerSocket Classes
- [ ] Create `src/runtime/socket_java.h` header
- [ ] Create `src/runtime/socket_java.c` implementation
- [ ] Define Socket object structure
- [ ] Define ServerSocket object structure
- [ ] Implement `socket_java_create()` - Create Socket object
- [ ] Implement `serversocket_java_create()` - Create ServerSocket
- [ ] Implement `socket_java_close()` - Close socket
- [ ] Implement `serversocket_java_accept()` - Accept connection
- [ ] Implement `socket_java_get_input_stream()` - Get InputStream
- [ ] Implement `socket_java_get_output_stream()` - Get OutputStream
- [ ] Define SocketInputStream structure
- [ ] Define SocketOutputStream structure
- [ ] Implement SocketInputStream read operations
- [ ] Implement SocketOutputStream write operations
- [ ] Add object lifecycle management

**Deliverables:**
- socket_java.h with structures
- socket_java.c with implementations
- Socket object management working

#### Day 18-19: Native Method Mechanism
- [ ] Add `OP_INVOKE_NATIVE` to `src/format/opcodes.h`
- [ ] Create `src/vm/native.h` header
- [ ] Create `src/vm/native.c` implementation
- [ ] Implement native method registry
- [ ] Implement native method dispatcher in interpreter.c
- [ ] Register socket native methods
- [ ] Add `Socket.<init>(String,int)` native
- [ ] Add `ServerSocket.<init>(int)` native
- [ ] Add `Socket.close()` native
- [ ] Add `ServerSocket.accept()` native
- [ ] Add `Socket.getInputStream()` native
- [ ] Add `Socket.getOutputStream()` native
- [ ] Test native method calling
- [ ] Add error handling for native calls

**Deliverables:**
- OP_INVOKE_NATIVE implemented
- Native method registry working
- Socket natives registered

#### Day 20-21: VM Socket Integration
- [ ] Update `OP_NEW` in interpreter.c for Socket
- [ ] Update `OP_NEW` in interpreter.c for ServerSocket
- [ ] Implement socket method dispatch
- [ ] Add socket cleanup on VM shutdown
- [ ] Test Socket object creation
- [ ] Test ServerSocket object creation
- [ ] Test socket method calls
- [ ] Test socket lifecycle (create, use, close)
- [ ] Add socket object tracking
- [ ] Test memory cleanup

**Deliverables:**
- Socket objects integrated with VM
- Method dispatch working
- Lifecycle management complete

### Week 4: Testing and Finalization

#### Day 22-24: Java Socket Testing
- [ ] Create `tests/network/echoserv.jav` - Echo server
- [ ] Create `tests/network/echocli.jav` - Echo client
- [ ] Create `tests/network/httpget.jav` - HTTP GET client
- [ ] Create `tests/network/README.md` - Test documentation
- [ ] Test echo server startup
- [ ] Test echo client connection
- [ ] Test bidirectional data transfer
- [ ] Test multiple client connections
- [ ] Test error handling (connection refused, etc.)
- [ ] Test socket close and cleanup
- [ ] Test HTTP GET to real web server
- [ ] Document test procedures

**Deliverables:**
- echoserv.jav working
- echocli.jav working
- httpget.jav working
- Test documentation complete

#### Day 25-26: Integration Testing
- [ ] Test with external HTTP server
- [ ] Test with external echo server
- [ ] Test multiple concurrent connections (4-8)
- [ ] Test large data transfers (>10KB)
- [ ] Measure throughput performance
- [ ] Test error recovery
- [ ] Test memory usage under load
- [ ] Profile CPU usage
- [ ] Test in DOSBox-X with various network configs
- [ ] Document performance characteristics

**Deliverables:**
- Integration tests passing
- Performance metrics documented
- Known limitations documented

#### Day 27-28: Documentation and Cleanup
- [ ] Create `PHASE4_NETWORK_QUICKSTART.md`
- [ ] Document Socket API for Java programmers
- [ ] Document ServerSocket API
- [ ] Create example programs with explanations
- [ ] Document DOSBox-X network setup
- [ ] Document Wattcp configuration
- [ ] Document limitations and known issues
- [ ] Document memory usage guidelines
- [ ] Remove all debug code
- [ ] Clean up and format code
- [ ] Final code review
- [ ] Update main README.md

**Deliverables:**
- PHASE4_NETWORK_QUICKSTART.md complete
- All documentation updated
- Code cleaned and finalized
- Phase 4 complete!

## Testing Checklist

### C-level Tests (Phase 4.1)
- [ ] Socket initialization works
- [ ] Socket creation/close works
- [ ] Server bind/listen works
- [ ] Client connect works
- [ ] Data send/receive works
- [ ] Multiple connections work
- [ ] Error handling works
- [ ] Memory usage acceptable

### Java-level Tests (Phase 4.2)
- [ ] Socket class instantiation works
- [ ] ServerSocket class instantiation works
- [ ] Socket.getInputStream() works
- [ ] Socket.getOutputStream() works
- [ ] ServerSocket.accept() works
- [ ] Data transfer works
- [ ] Socket.close() works
- [ ] Error handling works

### Integration Tests
- [ ] Echo server/client works
- [ ] HTTP GET works
- [ ] Multiple connections work
- [ ] Large transfers work
- [ ] Performance acceptable
- [ ] Memory usage acceptable

## Success Criteria

### Phase 4.1 Complete When:
- [x] Wattcp integrated
- [x] C-level socket API implemented
- [x] tsock.exe passes all tests
- [x] Memory usage < 150KB
- [x] Documentation complete

### Phase 4.2 Complete When:
- [x] Java Socket/ServerSocket classes work
- [x] Native method mechanism works
- [x] Echo test passes
- [x] HTTP GET test passes
- [x] Documentation complete

### Phase 4 Complete When:
- [x] All Phase 4.1 criteria met
- [x] All Phase 4.2 criteria met
- [x] Integration tests pass
- [x] Performance acceptable
- [x] Documentation complete
- [x] Code cleaned and reviewed

## Notes

- Test frequently in DOSBox-X with actual network
- Monitor memory usage throughout development
- Document any Wattcp quirks or issues
- Keep socket buffers small (1-2KB)
- Limit concurrent connections (4-8 max)
- Profile performance early and often