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
CFLAGS = -ms -0 -w4 -zq -od -d2

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

# Targets
all: test_memory test_interpreter mkdjc java2djc

# Test memory program
test_memory: $(BIN_DIR)/test_mem.exe

# Test interpreter program
test_interpreter: $(BIN_DIR)/test_int.exe

# .djc file generator tool
mkdjc: $(BIN_DIR)/mkdjc.exe

# Java to .djc converter
java2djc: $(BIN_DIR)/java2djc.exe

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
clean: .SYMBOLIC
dirs: .SYMBOLIC