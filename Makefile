# DOS Java Makefile for Open Watcom v2
# Target: 16-bit DOS executable (Small memory model)

# Compiler and linker
CC = wcc
CXX = wpp
LD = wlink
AR = wlib

# Compiler flags
# -mm: Medium memory model (changed from -ms for 64KB code segment limit)
# -0: 8086 instructions
# -w4: Warning level 4
# -zq: Quiet mode
# -os: Optimize for size
# -s: Disable stack overflow checks
# -i: Include path
CFLAGS = -mm -0 -w4 -zq -os -s -i=C:\WATCOM\h
CXXFLAGS = -mm -0 -w4 -zq -os -s -i=C:\WATCOM\h

# mTCP library settings (Phase 4.1) - Based on doscurl implementation
MTCP_TCP_H_DIR = C:\mTCP\src\TCPINC
MTCP_TCP_C_DIR = C:\mTCP\src\TCPLIB
MTCP_COMMON_H_DIR = C:\mTCP\src\INCLUDE
MTCP_CFG_DIR = tests\network
# Use doscurl-style compile options: optimizations + mTCP config
# Changed to -mm (medium model) to avoid 64KB code segment limit
MTCP_CXXFLAGS = -0 -mm -DCFG_H="sample.cfg" -oh -ok -ot -s -oa -ei -zp2 -zpw -ob -ol+ -oi+ -i=$(MTCP_TCP_H_DIR) -i=$(MTCP_COMMON_H_DIR) -i=$(MTCP_CFG_DIR)

# Linker flags
LDFLAGS = system dos

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Source files
VM_SRCS = $(SRC_DIR)/vm/memory.c $(SRC_DIR)/vm/stack.c $(SRC_DIR)/vm/interpreter.c $(SRC_DIR)/vm/native.c
FORMAT_SRCS = $(SRC_DIR)/format/djc.c $(SRC_DIR)/format/opcodes.c
RUNTIME_SRCS = $(SRC_DIR)/runtime/object.c $(SRC_DIR)/runtime/string.c $(SRC_DIR)/runtime/system.c $(SRC_DIR)/runtime/integer.c $(SRC_DIR)/runtime/inputstream.c $(SRC_DIR)/runtime/outputstream.c $(SRC_DIR)/runtime/fileinputstream.c $(SRC_DIR)/runtime/fileoutputstream.c $(SRC_DIR)/runtime/bufferedreader.c $(SRC_DIR)/runtime/bufferedwriter.c $(SRC_DIR)/runtime/dostime.c $(SRC_DIR)/runtime/date.c
NETWORK_SRCS = $(SRC_DIR)/network/socket.cpp
TEST_SRCS = $(SRC_DIR)/test_memory.c

# Object files
VM_OBJS = $(OBJ_DIR)/memory.obj $(OBJ_DIR)/stack.obj $(OBJ_DIR)/interpreter.obj $(OBJ_DIR)/native.obj
FORMAT_OBJS = $(OBJ_DIR)/djc.obj $(OBJ_DIR)/opcodes.obj
RUNTIME_OBJS = $(OBJ_DIR)/object.obj $(OBJ_DIR)/string.obj $(OBJ_DIR)/system.obj $(OBJ_DIR)/integer.obj $(OBJ_DIR)/inputstream.obj $(OBJ_DIR)/outputstream.obj $(OBJ_DIR)/fileinputstream.obj $(OBJ_DIR)/fileoutputstream.obj $(OBJ_DIR)/bufferedreader.obj $(OBJ_DIR)/bufferedwriter.obj $(OBJ_DIR)/dostime.obj $(OBJ_DIR)/date.obj
NETWORK_OBJS = $(OBJ_DIR)/socket.obj
TEST_OBJS = $(OBJ_DIR)/test_memory.obj

# Compiler object files
COMPILER_OBJS = $(OBJ_DIR)/lexer.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/codegen.obj

# Targets
all: test_memory test_interpreter mkdjc java2djc test_lexer test_parser test_semantic test_codegen djc djvm test_stream test_fileinputstream test_fileoutputstream test_buffered test_dostime test_date

# Test memory program
test_memory: $(BIN_DIR)/test_mem.exe

# Test interpreter program
test_interpreter: $(BIN_DIR)/test_int.exe

# Test stream program
test_stream: $(BIN_DIR)/tstrm.exe

# Test file input stream program
test_fileinputstream: $(BIN_DIR)/tfis.exe

# Test file output stream program
test_fileoutputstream: $(BIN_DIR)/tfos.exe

# Test buffered reader/writer program
test_buffered: $(BIN_DIR)/tbuf.exe

# Test DOS time API program
test_dostime: $(BIN_DIR)/tdtime.exe

# Test Date class program
test_date: $(BIN_DIR)/tdate.exe

# .djc file generator tool
mkdjc: $(BIN_DIR)/mkdjc.exe

# Test lexer program
test_lexer: $(BIN_DIR)/tlex.exe

# Test parser program
test_parser: $(BIN_DIR)/tpars.exe

# Java to .djc converter
java2djc: $(BIN_DIR)/java2djc.exe

# DOS Java Virtual Machine
djvm: $(BIN_DIR)/djvm.exe

$(BIN_DIR)/test_mem.exe: $(TEST_OBJS) $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS)
	@echo Linking test_mem.exe...
	$(LD) $(LDFLAGS) name $@ file { $(TEST_OBJS) $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS) }

$(BIN_DIR)/test_int.exe: $(OBJ_DIR)/test_interpreter.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS)
	@echo Linking test_int.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_interpreter.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS) }

$(BIN_DIR)/tstrm.exe: $(OBJ_DIR)/test_stream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking tstrm.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_stream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj }

$(BIN_DIR)/tfis.exe: $(OBJ_DIR)/test_fileinputstream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking tfis.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_fileinputstream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj }

$(BIN_DIR)/tfos.exe: $(OBJ_DIR)/test_fileoutputstream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking tfos.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_fileoutputstream.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj }

$(BIN_DIR)/tbuf.exe: $(OBJ_DIR)/test_buffered.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking tbuf.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_buffered.obj $(RUNTIME_OBJS) $(OBJ_DIR)/memory.obj }

$(BIN_DIR)/tdtime.exe: $(OBJ_DIR)/test_dostime.obj $(OBJ_DIR)/dostime.obj
	@echo Linking tdtime.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_dostime.obj $(OBJ_DIR)/dostime.obj }

$(BIN_DIR)/tdate.exe: $(OBJ_DIR)/test_date.obj $(OBJ_DIR)/date.obj $(OBJ_DIR)/dostime.obj
	@echo Linking tdate.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_date.obj $(OBJ_DIR)/date.obj $(OBJ_DIR)/dostime.obj }


$(BIN_DIR)/mkdjc.exe: $(OBJ_DIR)/mkdjc.obj
	@echo Linking mkdjc.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/mkdjc.obj }

$(BIN_DIR)/java2djc.exe: $(OBJ_DIR)/java2djc.obj $(OBJ_DIR)/classfile.obj $(FORMAT_OBJS) $(VM_OBJS) $(RUNTIME_OBJS)
	@echo Linking java2djc.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/java2djc.obj $(OBJ_DIR)/classfile.obj $(FORMAT_OBJS) $(VM_OBJS) $(RUNTIME_OBJS) }

$(BIN_DIR)/djvm.exe: $(OBJ_DIR)/djvm.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS)
	@echo Linking djvm.exe...
	$(LD) $(LDFLAGS) option stack=16384 name $@ file { $(OBJ_DIR)/djvm.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS) }

# Compile rules - VM
$(OBJ_DIR)/memory.obj: $(SRC_DIR)/vm/memory.c $(SRC_DIR)/vm/memory.h
	@echo Compiling memory.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/memory.c

$(OBJ_DIR)/stack.obj: $(SRC_DIR)/vm/stack.c $(SRC_DIR)/vm/stack.h
	@echo Compiling stack.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/stack.c

$(OBJ_DIR)/interpreter.obj: $(SRC_DIR)/vm/interpreter.c $(SRC_DIR)/vm/interpreter.h $(SRC_DIR)/vm/native.h
	@echo Compiling interpreter.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/interpreter.c

$(OBJ_DIR)/native.obj: $(SRC_DIR)/vm/native.c $(SRC_DIR)/vm/native.h
	@echo Compiling native.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/native.c

$(OBJ_DIR)/djvm.obj: $(SRC_DIR)/vm/djvm.c $(SRC_DIR)/vm/interpreter.h $(SRC_DIR)/vm/native.h
	@echo Compiling djvm.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/djvm.c

# Compile rules - Format
$(OBJ_DIR)/djc.obj: $(SRC_DIR)/format/djc.c $(SRC_DIR)/format/djc.h
	@echo Compiling djc.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/format/djc.c

$(OBJ_DIR)/opcodes.obj: $(SRC_DIR)/format/opcodes.c $(SRC_DIR)/format/opcodes.h
	@echo Compiling opcodes.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/format/opcodes.c

# Compile rules - Runtime
$(OBJ_DIR)/object.obj: $(SRC_DIR)/runtime/object.c $(SRC_DIR)/runtime/object.h
	@echo Compiling object.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/object.c

$(OBJ_DIR)/string.obj: $(SRC_DIR)/runtime/string.c $(SRC_DIR)/runtime/string.h
	@echo Compiling string.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/string.c

$(OBJ_DIR)/system.obj: $(SRC_DIR)/runtime/system.c $(SRC_DIR)/runtime/system.h
	@echo Compiling system.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/system.c

$(OBJ_DIR)/integer.obj: $(SRC_DIR)/runtime/integer.c $(SRC_DIR)/runtime/integer.h
	@echo Compiling integer.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/integer.c

$(OBJ_DIR)/inputstream.obj: $(SRC_DIR)/runtime/inputstream.c $(SRC_DIR)/runtime/inputstream.h $(SRC_DIR)/runtime/object.h
	@echo Compiling inputstream.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/inputstream.c

$(OBJ_DIR)/outputstream.obj: $(SRC_DIR)/runtime/outputstream.c $(SRC_DIR)/runtime/outputstream.h $(SRC_DIR)/runtime/object.h
	@echo Compiling outputstream.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/outputstream.c

$(OBJ_DIR)/fileinputstream.obj: $(SRC_DIR)/runtime/fileinputstream.c $(SRC_DIR)/runtime/fileinputstream.h $(SRC_DIR)/runtime/inputstream.h
	@echo Compiling fileinputstream.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/fileinputstream.c

$(OBJ_DIR)/fileoutputstream.obj: $(SRC_DIR)/runtime/fileoutputstream.c $(SRC_DIR)/runtime/fileoutputstream.h $(SRC_DIR)/runtime/outputstream.h
	@echo Compiling fileoutputstream.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/fileoutputstream.c

$(OBJ_DIR)/bufferedreader.obj: $(SRC_DIR)/runtime/bufferedreader.c $(SRC_DIR)/runtime/bufferedreader.h $(SRC_DIR)/runtime/fileinputstream.h
	@echo Compiling bufferedreader.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/bufferedreader.c

$(OBJ_DIR)/bufferedwriter.obj: $(SRC_DIR)/runtime/bufferedwriter.c $(SRC_DIR)/runtime/bufferedwriter.h $(SRC_DIR)/runtime/fileoutputstream.h
	@echo Compiling bufferedwriter.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/bufferedwriter.c

$(OBJ_DIR)/dostime.obj: $(SRC_DIR)/runtime/dostime.c $(SRC_DIR)/runtime/dostime.h
	@echo Compiling dostime.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/dostime.c

$(OBJ_DIR)/date.obj: $(SRC_DIR)/runtime/date.c $(SRC_DIR)/runtime/date.h $(SRC_DIR)/runtime/dostime.h
	@echo Compiling date.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/runtime/date.c

# Compile rules - Network
$(OBJ_DIR)/socket.obj: $(SRC_DIR)/network/socket.cpp $(SRC_DIR)/network/socket.h
	@echo Compiling socket.cpp...
	$(CXX) $(MTCP_CXXFLAGS) -i=$(SRC_DIR)/network -fo=$@ $(SRC_DIR)/network/socket.cpp

$(OBJ_DIR)/test_dostime.obj: $(TEST_DIR)/dostime/test_dostime.c $(SRC_DIR)/runtime/dostime.h
	@echo Compiling test_dostime.c...
	$(CC) $(CFLAGS) -fo=$@ $(TEST_DIR)/dostime/test_dostime.c

$(OBJ_DIR)/test_date.obj: $(TEST_DIR)/date/test_date.c $(SRC_DIR)/runtime/date.h $(SRC_DIR)/runtime/dostime.h
	@echo Compiling test_date.c...
	$(CC) $(CFLAGS) -fo=$@ $(TEST_DIR)/date/test_date.c

# Compile rules - Tests
$(OBJ_DIR)/test_memory.obj: $(SRC_DIR)/test_memory.c
	@echo Compiling test_memory.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/test_memory.c

$(OBJ_DIR)/test_interpreter.obj: $(SRC_DIR)/test_interpreter.c
	@echo Compiling test_interpreter.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/test_interpreter.c

$(OBJ_DIR)/test_stream.obj: tests/io/test_stream.c $(SRC_DIR)/runtime/inputstream.h $(SRC_DIR)/runtime/outputstream.h
	@echo Compiling test_stream.c...
	$(CC) $(CFLAGS) -fo=$@ tests/io/test_stream.c

$(OBJ_DIR)/test_fileinputstream.obj: tests/io/test_fileinputstream.c $(SRC_DIR)/runtime/fileinputstream.h
	@echo Compiling test_fileinputstream.c...
	$(CC) $(CFLAGS) -fo=$@ tests/io/test_fileinputstream.c

$(OBJ_DIR)/test_fileoutputstream.obj: tests/io/test_fileoutputstream.c $(SRC_DIR)/runtime/fileoutputstream.h
	@echo Compiling test_fileoutputstream.c...
	$(CC) $(CFLAGS) -fo=$@ tests/io/test_fileoutputstream.c

$(OBJ_DIR)/test_buffered.obj: tests/io/test_buffered.c $(SRC_DIR)/runtime/bufferedreader.h $(SRC_DIR)/runtime/bufferedwriter.h
	@echo Compiling test_buffered.c...
	$(CC) $(CFLAGS) -fo=$@ tests/io/test_buffered.c

# Compile rules - Tools
$(OBJ_DIR)/mkdjc.obj: tools/mkdjc.c
	@echo Compiling mkdjc.c...
	$(CC) $(CFLAGS) -fo=$@ tools/mkdjc.c

$(OBJ_DIR)/java2djc.obj: tools/java2djc.c tools/classfile.h
	@echo Compiling java2djc.c...
	$(CC) $(CFLAGS) -fo=$@ tools/java2djc.c

$(OBJ_DIR)/classfile.obj: tools/classfile.c tools/classfile.h
	@echo Compiling classfile.c...
	$(CC) $(CFLAGS) -fo=$@ tools/classfile.c

# Compile rules - Compiler
$(OBJ_DIR)/lexer.obj: tools/compiler/lexer.c tools/compiler/lexer.h
	@echo Compiling lexer.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/lexer.c

$(OBJ_DIR)/test_lexer.obj: tools/compiler/test_lexer.c tools/compiler/lexer.h
	@echo Compiling test_lexer.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/test_lexer.c

$(BIN_DIR)/tlex.exe: $(OBJ_DIR)/test_lexer.obj $(OBJ_DIR)/lexer.obj
	@echo Linking tlex.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_lexer.obj $(OBJ_DIR)/lexer.obj }

$(OBJ_DIR)/parser.obj: tools/compiler/parser.c tools/compiler/parser.h tools/compiler/ast.h
	@echo Compiling parser.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/parser.c

$(OBJ_DIR)/test_parser.obj: tools/compiler/test_parser.c tools/compiler/parser.h tools/compiler/lexer.h
	@echo Compiling test_parser.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/test_parser.c

$(BIN_DIR)/tpars.exe: $(OBJ_DIR)/test_parser.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj
	@echo Linking tpars.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_parser.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj }

# Compile rules - Semantic Analyzer
$(OBJ_DIR)/symtable.obj: tools/compiler/symtable.c tools/compiler/symtable.h tools/compiler/ast.h
	@echo Compiling symtable.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/symtable.c

$(OBJ_DIR)/semantic.obj: tools/compiler/semantic.c tools/compiler/semantic.h tools/compiler/symtable.h
	@echo Compiling semantic.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/semantic.c

$(OBJ_DIR)/test_semantic.obj: tools/compiler/test_semantic.c tools/compiler/semantic.h
	@echo Compiling test_semantic.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/test_semantic.c

$(BIN_DIR)/tsem.exe: $(OBJ_DIR)/test_semantic.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj
	@echo Linking tsem.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_semantic.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj }

test_semantic: $(BIN_DIR)/tsem.exe

# Compile rules - Code Generator
$(OBJ_DIR)/codegen.obj: tools/compiler/codegen.c tools/compiler/codegen.h tools/compiler/semantic.h
	@echo Compiling codegen.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/codegen.c

$(OBJ_DIR)/test_codegen.obj: tools/compiler/test_codegen.c tools/compiler/codegen.h
	@echo Compiling test_codegen.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/test_codegen.c

$(BIN_DIR)/tcgen.exe: $(OBJ_DIR)/test_codegen.obj $(OBJ_DIR)/codegen.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj $(FORMAT_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking tcgen.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_codegen.obj $(OBJ_DIR)/codegen.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj $(FORMAT_OBJS) $(OBJ_DIR)/memory.obj }

test_codegen: $(BIN_DIR)/tcgen.exe

# Compile rules - Integrated Compiler (djc)
$(OBJ_DIR)/djc_main.obj: tools/compiler/djc.c tools/compiler/djc.h
	@echo Compiling djc.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/djc.c

$(BIN_DIR)/djc.exe: $(OBJ_DIR)/djc_main.obj $(COMPILER_OBJS) $(FORMAT_OBJS) $(OBJ_DIR)/memory.obj
	@echo Linking djc.exe...
	$(LD) $(LDFLAGS) option stack=16384 name $@ file { $(OBJ_DIR)/djc_main.obj $(COMPILER_OBJS) $(FORMAT_OBJS) $(OBJ_DIR)/memory.obj }

djc: $(BIN_DIR)/djc.exe

# Clean
clean:
	@echo Cleaning build files...
	@if exist build\obj\*.obj del build\obj\*.obj
	@if exist build\bin\*.exe del build\bin\*.exe
	@if exist build\bin\*.map del build\bin\*.map
	@if exist *.err del *.err

# Create directories
dirs:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Help
help: .SYMBOLIC
	@echo DOS Java Build System
	@echo.
	@echo Targets:
	@echo   all          - Build all targets (default)
	@echo   test_memory  - Build memory manager test
	@echo   clean        - Remove build files
	@echo   dirs         - Create build directories
	@echo   help         - Show this help
	@echo.
	@echo Compiler: Open Watcom v2 C Compiler
	@echo Target: 16-bit DOS (Medium memory model)

# mTCP test (Phase 4.1 Day 1-2)
# Using mTCP source files directly (like mTCP SAMPLE app)
test_mtcp: $(BIN_DIR)/tmtcp.exe

# Socket wrapper test (Phase 4.1 Day 3-5)
test_socket: $(BIN_DIR)/tsock.exe

# HTTP client test (Phase 4.1 Day 6-7)
test_http: $(BIN_DIR)/thttp.exe

# Network diagnostics test (Phase 4.1 Day 6-7)
test_netdiag: $(BIN_DIR)/tnetdiag.exe

# Send/Recv test (Phase 4.1 Day 8-10)
test_sendrecv: $(BIN_DIR)/tsendrcv.exe

# Memory profiling test (Phase 4.1 Day 11-12)
test_memprof: $(BIN_DIR)/tmemprof.exe

# Runtime Socket test (Phase 4.2 Day 15-17)
test_sockrt: $(BIN_DIR)/tsockrt.exe

# mTCP object files needed
MTCP_OBJS = $(OBJ_DIR)/packet.obj $(OBJ_DIR)/arp.obj $(OBJ_DIR)/eth.obj $(OBJ_DIR)/ip.obj $(OBJ_DIR)/tcp.obj $(OBJ_DIR)/tcpsockm.obj $(OBJ_DIR)/udp.obj $(OBJ_DIR)/utils.obj $(OBJ_DIR)/dns.obj $(OBJ_DIR)/timer.obj $(OBJ_DIR)/ipasm.obj $(OBJ_DIR)/trace.obj

# Compile test program
$(OBJ_DIR)/test_mtcp_init.obj: tests/network/test_mtcp_init.cpp
	@echo Compiling test_mtcp_init.cpp...
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ tests/network/test_mtcp_init.cpp

# Compile mTCP C++ source files
$(OBJ_DIR)/packet.obj: $(MTCP_TCP_C_DIR)/packet.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/packet.cpp

$(OBJ_DIR)/arp.obj: $(MTCP_TCP_C_DIR)/arp.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/arp.cpp

$(OBJ_DIR)/eth.obj: $(MTCP_TCP_C_DIR)/eth.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/eth.cpp

$(OBJ_DIR)/ip.obj: $(MTCP_TCP_C_DIR)/ip.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/ip.cpp

$(OBJ_DIR)/tcp.obj: $(MTCP_TCP_C_DIR)/tcp.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/tcp.cpp

$(OBJ_DIR)/tcpsockm.obj: $(MTCP_TCP_C_DIR)/tcpsockm.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/tcpsockm.cpp

$(OBJ_DIR)/udp.obj: $(MTCP_TCP_C_DIR)/udp.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/udp.cpp

$(OBJ_DIR)/utils.obj: $(MTCP_TCP_C_DIR)/utils.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/utils.cpp

$(OBJ_DIR)/dns.obj: $(MTCP_TCP_C_DIR)/dns.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/dns.cpp

$(OBJ_DIR)/timer.obj: $(MTCP_TCP_C_DIR)/timer.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/timer.cpp

$(OBJ_DIR)/trace.obj: $(MTCP_TCP_C_DIR)/trace.cpp
	$(CXX) $(MTCP_CXXFLAGS) -fo=$@ $(MTCP_TCP_C_DIR)/trace.cpp

# Compile mTCP assembly file
$(OBJ_DIR)/ipasm.obj: $(MTCP_TCP_C_DIR)/ipasm.asm
	wasm -0 -ms -fo=$@ $(MTCP_TCP_C_DIR)/ipasm.asm

# Link everything together
$(BIN_DIR)/tmtcp.exe: $(OBJ_DIR)/test_mtcp_init.obj $(MTCP_OBJS)
	@echo Linking tmtcp.exe with mTCP objects...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_mtcp_init.obj $(MTCP_OBJS) }

# Socket wrapper test (Phase 4.1 Day 3-5)
$(OBJ_DIR)/test_socket.obj: tests/network/test_socket.c $(SRC_DIR)/network/socket.h
	@echo Compiling test_socket.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ tests/network/test_socket.c

$(BIN_DIR)/tsock.exe: $(OBJ_DIR)/test_socket.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS)
	@echo Linking tsock.exe with socket wrapper and mTCP...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_socket.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS) }

# HTTP client test (Phase 4.1 Day 6-7)
$(OBJ_DIR)/test_http.obj: tests/network/test_http.c $(SRC_DIR)/network/socket.h
	@echo Compiling test_http.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ tests/network/test_http.c

$(BIN_DIR)/thttp.exe: $(OBJ_DIR)/test_http.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS)
	@echo Linking thttp.exe with socket wrapper and mTCP...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_http.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS) }

# Network diagnostics test (Phase 4.1 Day 6-7)
$(OBJ_DIR)/test_network_diag.obj: tests/network/test_network_diag.c $(SRC_DIR)/network/socket.h
	@echo Compiling test_network_diag.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ tests/network/test_network_diag.c

$(BIN_DIR)/tnetdiag.exe: $(OBJ_DIR)/test_network_diag.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS)
	@echo Linking tnetdiag.exe with socket wrapper and mTCP...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_network_diag.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS) }

# Send/Recv test (Phase 4.1 Day 8-10)
$(OBJ_DIR)/test_send_recv.obj: tests/network/test_send_recv.c $(SRC_DIR)/network/socket.h
	@echo Compiling test_send_recv.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ tests/network/test_send_recv.c

$(BIN_DIR)/tsendrcv.exe: $(OBJ_DIR)/test_send_recv.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS)
	@echo Linking tsendrcv.exe with socket wrapper and mTCP...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_send_recv.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS) }

# Memory profiling test (Phase 4.1 Day 11-12)
$(OBJ_DIR)/test_memory_profile.obj: tests/network/test_memory_profile.c $(SRC_DIR)/network/socket.h
	@echo Compiling test_memory_profile.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ tests/network/test_memory_profile.c

$(BIN_DIR)/tmemprof.exe: $(OBJ_DIR)/test_memory_profile.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS)
	@echo Linking tmemprof.exe with socket wrapper and mTCP...
	$(LD) $(LDFLAGS) option stack=4096 name $@ file { $(OBJ_DIR)/test_memory_profile.obj $(OBJ_DIR)/socket.obj $(MTCP_OBJS) }

# Runtime Socket test (Phase 4.2 Day 15-17)
$(OBJ_DIR)/socket_rt.obj: $(SRC_DIR)/runtime/socket.c $(SRC_DIR)/runtime/socket.h
	@echo Compiling socket.c (runtime)...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/network -fo=$@ $(SRC_DIR)/runtime/socket.c

$(OBJ_DIR)/test_socket_runtime.obj: tests/runtime/test_socket_runtime.c $(SRC_DIR)/runtime/socket.h
	@echo Compiling test_socket_runtime.c...
	$(CC) $(CFLAGS) -i=$(SRC_DIR)/runtime -fo=$@ tests/runtime/test_socket_runtime.c

$(BIN_DIR)/tsockrt.exe: $(OBJ_DIR)/test_socket_runtime.obj $(OBJ_DIR)/socket_rt.obj $(OBJ_DIR)/string.obj $(OBJ_DIR)/object.obj $(OBJ_DIR)/socket.obj $(OBJ_DIR)/memory.obj $(MTCP_OBJS)
	@echo Linking tsockrt.exe with runtime socket, string, object, memory, and mTCP...
	$(LD) $(LDFLAGS) option stack=8192 name $@ file { $(OBJ_DIR)/test_socket_runtime.obj $(OBJ_DIR)/socket_rt.obj $(OBJ_DIR)/string.obj $(OBJ_DIR)/object.obj $(OBJ_DIR)/socket.obj $(OBJ_DIR)/memory.obj $(MTCP_OBJS) }

# Declare symbolic (phony) targets for wmake
all: .SYMBOLIC
test_memory: .SYMBOLIC
test_mtcp: .SYMBOLIC
test_socket: .SYMBOLIC
test_http: .SYMBOLIC
test_netdiag: .SYMBOLIC
test_sendrecv: .SYMBOLIC
test_memprof: .SYMBOLIC
test_sockrt: .SYMBOLIC
test_interpreter: .SYMBOLIC
test_stream: .SYMBOLIC
test_fileinputstream: .SYMBOLIC
test_fileoutputstream: .SYMBOLIC
test_buffered: .SYMBOLIC
test_dostime: .SYMBOLIC
test_date: .SYMBOLIC
mkdjc: .SYMBOLIC
java2djc: .SYMBOLIC
test_lexer: .SYMBOLIC
test_parser: .SYMBOLIC
test_semantic: .SYMBOLIC
test_codegen: .SYMBOLIC
djc: .SYMBOLIC
djvm: .SYMBOLIC
clean: .SYMBOLIC
dirs: .SYMBOLIC