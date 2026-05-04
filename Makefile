# DOS Java Makefile for Open Watcom v2
# Target: 16-bit DOS executable (Small memory model)

# Compiler and linker
CC = wcc
LD = wlink
AR = wlib

# Compiler flags
# -ms: Small memory model
# -0: 8086 instructions
# -w4: Warning level 4
# -zq: Quiet mode
# -od: Disable optimizations (for debugging)
# -d2: Full debugging info
# -i: Include path
CFLAGS = -ms -0 -w4 -zq -os -s -i=C:\WATCOM\h

# Linker flags
LDFLAGS = system dos

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Source files
VM_SRCS = $(SRC_DIR)/vm/memory.c $(SRC_DIR)/vm/stack.c $(SRC_DIR)/vm/interpreter.c
FORMAT_SRCS = $(SRC_DIR)/format/djc.c $(SRC_DIR)/format/opcodes.c
RUNTIME_SRCS = $(SRC_DIR)/runtime/object.c $(SRC_DIR)/runtime/string.c $(SRC_DIR)/runtime/system.c $(SRC_DIR)/runtime/integer.c
TEST_SRCS = $(SRC_DIR)/test_memory.c

# Object files
VM_OBJS = $(OBJ_DIR)/memory.obj $(OBJ_DIR)/stack.obj $(OBJ_DIR)/interpreter.obj
FORMAT_OBJS = $(OBJ_DIR)/djc.obj $(OBJ_DIR)/opcodes.obj
RUNTIME_OBJS = $(OBJ_DIR)/object.obj $(OBJ_DIR)/string.obj $(OBJ_DIR)/system.obj $(OBJ_DIR)/integer.obj
TEST_OBJS = $(OBJ_DIR)/test_memory.obj

# Compiler object files
COMPILER_OBJS = $(OBJ_DIR)/lexer.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/codegen.obj

# Targets
all: test_memory test_interpreter mkdjc java2djc test_lexer test_parser test_semantic test_codegen djc djvm

# Test memory program
test_memory: $(BIN_DIR)/test_mem.exe

# Test interpreter program
test_interpreter: $(BIN_DIR)/test_int.exe

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

$(OBJ_DIR)/interpreter.obj: $(SRC_DIR)/vm/interpreter.c $(SRC_DIR)/vm/interpreter.h
	@echo Compiling interpreter.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/vm/interpreter.c

$(OBJ_DIR)/djvm.obj: $(SRC_DIR)/vm/djvm.c $(SRC_DIR)/vm/interpreter.h
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

# Compile rules - Tests
$(OBJ_DIR)/test_memory.obj: $(SRC_DIR)/test_memory.c
	@echo Compiling test_memory.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/test_memory.c

$(OBJ_DIR)/test_interpreter.obj: $(SRC_DIR)/test_interpreter.c
	@echo Compiling test_interpreter.c...
	$(CC) $(CFLAGS) -fo=$@ $(SRC_DIR)/test_interpreter.c

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
	@echo Target: 16-bit DOS (Small memory model)

# Declare symbolic (phony) targets for wmake
all: .SYMBOLIC
test_memory: .SYMBOLIC
test_interpreter: .SYMBOLIC
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