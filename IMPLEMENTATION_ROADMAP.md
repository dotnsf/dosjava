# DOS Java Implementation Roadmap

## Overview

This document provides a step-by-step guide for implementing the DOS Java system. Follow these phases sequentially to build a working Java runtime for 16-bit PC-DOS.

## Phase 1: Project Setup and Foundation (Week 1)

### 1.1 Environment Setup

**Tasks:**
- [ ] Install Open Watcom v2 compiler
- [ ] Set up DOS development environment (DOSBox or real hardware)
- [ ] Create project directory structure
- [ ] Initialize version control (git)
- [ ] Create basic Makefile

**Deliverables:**
- Working build environment
- Empty project structure
- Basic Makefile that compiles a "Hello World" C program

**Validation:**
```bash
# Test Open Watcom installation
wcc -? 

# Test basic compilation
wcc -ms hello.c
wlink system dos file hello.obj
```

### 1.2 Define Core Data Structures

**Files to Create:**
- [`src/vm/vm.h`](src/vm/vm.h) - Core VM structures
- [`src/format/djc.h`](src/format/djc.h) - .djc file format
- [`src/runtime/object.h`](src/runtime/object.h) - Object base structure

**Key Structures:**

```c
// vm.h
typedef struct {
    uint8_t* heap_start;
    uint16_t heap_size;
    ClassInfo** loaded_classes;
    uint16_t class_count;
} VM;

// djc.h
typedef struct {
    uint16_t magic;
    uint16_t version;
    uint16_t constant_pool_count;
    uint16_t method_count;
} DJCHeader;

// object.h
typedef struct {
    uint16_t class_id;
    uint16_t ref_count;
} Object;
```

**Validation:**
- All headers compile without errors
- No circular dependencies
- Proper header guards in place

### 1.3 Implement Memory Manager

**Files to Create:**
- [`src/vm/memory.h`](src/vm/memory.h)
- [`src/vm/memory.c`](src/vm/memory.c)

**Functions to Implement:**

```c
void memory_init(uint16_t heap_size);
void* memory_alloc(uint16_t size);
void memory_free(void* ptr);
uint16_t memory_available();
void memory_stats();
```

**Implementation Strategy:**
1. Simple bump allocator initially
2. Free list for released memory
3. Alignment to 2-byte boundaries
4. Track allocation statistics

**Test Program:**
```c
// test_memory.c
int main() {
    memory_init(1024);
    void* p1 = memory_alloc(100);
    void* p2 = memory_alloc(200);
    memory_free(p1);
    memory_stats();
    return 0;
}
```

**Validation:**
- Allocate and free memory successfully
- No memory leaks in test program
- Statistics show correct values

## Phase 2: Bytecode Format and Loader (Week 2)

### 2.1 Implement .djc File I/O

**Files to Create:**
- [`src/format/djc.c`](src/format/djc.c)
- [`src/format/constants.h`](src/format/constants.h)

**Functions to Implement:**

```c
DJCFile* djc_open(const char* filename);
void djc_close(DJCFile* file);
DJCHeader* djc_read_header(DJCFile* file);
DJCConstant* djc_read_constant(DJCFile* file, uint16_t index);
```

**Implementation Notes:**
- Use DOS file I/O functions (open, read, close)
- Read in binary mode
- Validate magic number (0x444A)
- Handle file errors gracefully

**Test Program:**
Create a minimal .djc file manually (hex editor) and test reading it.

### 2.2 Implement Class Loader

**Files to Create:**
- [`src/vm/classloader.h`](src/vm/classloader.h)
- [`src/vm/classloader.c`](src/vm/classloader.c)

**Functions to Implement:**

```c
ClassInfo* load_class(const char* filename);
void unload_class(ClassInfo* cls);
DJCMethod* find_method(ClassInfo* cls, const char* name);
DJCField* find_field(ClassInfo* cls, const char* name);
```

**Implementation Strategy:**
1. Open .djc file
2. Read and validate header
3. Load constant pool
4. Load method descriptors
5. Load field descriptors
6. Load bytecode section
7. Store in ClassInfo structure

**Validation:**
- Load a simple test .djc file
- Verify all sections loaded correctly
- Print class information for debugging

### 2.3 Create Simple .djc File Generator

**Purpose:** Create test .djc files without full preprocessor

**Files to Create:**
- [`tools/mkdjc.c`](tools/mkdjc.c)

**Functionality:**
- Generate .djc file from simple text description
- Useful for testing before preprocessor is ready

**Example Usage:**
```
mkdjc test.txt test.djc
```

Where test.txt contains:
```
CLASS HelloWorld
METHOD main
  PUSH_CONST "Hello, DOS!"
  INVOKE_STATIC System.out.println
  RETURN
END
```

## Phase 3: Bytecode Interpreter (Week 3)

### 3.1 Implement Operand Stack

**Files to Create:**
- [`src/vm/stack.h`](src/vm/stack.h)
- [`src/vm/stack.c`](src/vm/stack.c)

**Functions to Implement:**

```c
void stack_init(Stack* stack, uint16_t size);
void stack_push(Stack* stack, uint16_t value);
uint16_t stack_pop(Stack* stack);
uint16_t stack_peek(Stack* stack);
void stack_dup(Stack* stack);
```

**Implementation:**
- Fixed-size array for stack
- Stack pointer (sp) tracking
- Overflow/underflow checking
- Support for 16-bit values

### 3.2 Implement Basic Interpreter

**Files to Create:**
- [`src/vm/interpreter.h`](src/vm/interpreter.h)
- [`src/vm/interpreter.c`](src/vm/interpreter.c)

**Core Interpreter Loop:**

```c
void execute_method(ExecutionContext* ctx, uint16_t method_index) {
    uint8_t* pc = ctx->pc;
    
    while (1) {
        uint8_t opcode = *pc++;
        
        switch (opcode) {
            case OP_PUSH_CONST:
                // Implementation
                break;
            case OP_ADD:
                // Implementation
                break;
            // ... more opcodes
            case OP_RETURN:
                return;
        }
    }
}
```

**Initial Opcodes to Implement:**
1. `PUSH_CONST` - Push constant to stack
2. `POP` - Pop from stack
3. `ADD`, `SUB`, `MUL`, `DIV` - Arithmetic
4. `LOAD_LOCAL`, `STORE_LOCAL` - Local variables
5. `RETURN` - Return from method

**Test Program:**
```java
// Arithmetic.java (conceptual)
class Arithmetic {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b;  // Should be 30
    }
}
```

### 3.3 Add Control Flow Instructions

**Opcodes to Implement:**
1. `GOTO` - Unconditional jump
2. `IF_TRUE` - Jump if true
3. `IF_FALSE` - Jump if false
4. `CMP_EQ`, `CMP_LT`, etc. - Comparisons

**Test Program:**
```java
// Loops.java (conceptual)
class Loops {
    public static void main() {
        int i = 0;
        while (i < 10) {
            i = i + 1;
        }
    }
}
```

## Phase 4: Runtime Library (Week 4)

### 4.1 Implement java.lang.Object

**Files to Create:**
- [`src/runtime/object.c`](src/runtime/object.c)

**Functions to Implement:**

```c
Object* object_new(uint16_t class_id);
void object_delete(Object* obj);
uint8_t object_equals(Object* obj1, Object* obj2);
uint16_t object_hashcode(Object* obj);
String* object_tostring(Object* obj);
```

**Implementation:**
- Allocate from heap
- Initialize ref_count to 1
- Store class_id
- Default implementations for methods

### 4.2 Implement java.lang.String

**Files to Create:**
- [`src/runtime/string.h`](src/runtime/string.h)
- [`src/runtime/string.c`](src/runtime/string.c)

**Functions to Implement:**

```c
String* string_new(const char* cstr);
String* string_concat(String* s1, String* s2);
uint16_t string_length(String* str);
char string_charat(String* str, uint16_t index);
uint8_t string_equals(String* s1, String* s2);
```

**Memory Management:**
- Store string data in heap
- Reference counting for GC
- Immutable strings

### 4.3 Implement java.lang.System

**Files to Create:**
- [`src/runtime/system.h`](src/runtime/system.h)
- [`src/runtime/system.c`](src/runtime/system.c)

**Functions to Implement:**

```c
void system_print(String* str);
void system_println(String* str);
```

**Implementation:**
- Use DOS INT 21h for console output
- Convert String to C string
- Handle newlines properly

**Test Program:**
```java
// HelloWorld.java
class HelloWorld {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### 4.4 Implement java.lang.Integer

**Files to Create:**
- [`src/runtime/integer.h`](src/runtime/integer.h)
- [`src/runtime/integer.c`](src/runtime/integer.c)

**Functions to Implement:**

```c
Integer* integer_new(int16_t value);
int16_t integer_intvalue(Integer* obj);
String* integer_tostring(Integer* obj);
```

## Phase 5: Method Invocation (Week 5)

### 5.1 Implement Method Call Stack

**Enhancements to stack.h/stack.c:**

```c
typedef struct {
    uint8_t* return_pc;
    uint16_t* locals;
    uint16_t local_count;
} StackFrame;

void stack_push_frame(Stack* stack, StackFrame* frame);
void stack_pop_frame(Stack* stack);
```

### 5.2 Implement Method Invocation

**Opcodes to Implement:**
1. `INVOKE_STATIC` - Static method call
2. `INVOKE_VIRTUAL` - Instance method call
3. `RETURN` - Return from method (enhanced)

**Implementation:**
1. Save current PC
2. Push stack frame
3. Copy arguments to local variables
4. Jump to method code
5. On return, restore PC and pop frame

**Test Program:**
```java
class Methods {
    public static int add(int a, int b) {
        return a + b;
    }
    
    public static void main() {
        int result = add(10, 20);
        System.out.println(result);
    }
}
```

### 5.3 Implement Object Creation

**Opcodes to Implement:**
1. `NEW` - Create new object
2. `GET_FIELD` - Read instance field
3. `PUT_FIELD` - Write instance field

**Implementation:**
1. Allocate object from heap
2. Initialize fields to default values
3. Return object reference

**Test Program:**
```java
class Point {
    int x;
    int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
}

class TestObjects {
    public static void main() {
        Point p = new Point(10, 20);
    }
}
```

## Phase 6: Garbage Collection (Week 6)

### 6.1 Implement Reference Counting

**Enhancements to object.h/object.c:**

```c
void object_addref(Object* obj);
void object_release(Object* obj);
```

**Implementation:**
1. Increment ref_count on assignment
2. Decrement ref_count on scope exit
3. Free object when ref_count reaches 0

### 6.2 Integrate GC with VM

**Files to Create:**
- [`src/vm/gc.h`](src/vm/gc.h)
- [`src/vm/gc.c`](src/vm/gc.c)

**Functions to Implement:**

```c
void gc_init();
void gc_register_root(Object** root);
void gc_collect();
void gc_stats();
```

**Integration Points:**
- Object creation
- Variable assignment
- Method return
- Stack frame pop

## Phase 7: Preprocessor (Week 7)

### 7.1 Implement Java .class File Parser

**Files to Create:**
- [`preprocessor/src/classfile.h`](preprocessor/src/classfile.h)
- [`preprocessor/src/classfile.c`](preprocessor/src/classfile.c)

**Functions to Implement:**

```c
ClassFile* parse_classfile(const char* filename);
void free_classfile(ClassFile* cf);
```

**Implementation:**
1. Read .class file header
2. Parse constant pool
3. Parse methods
4. Parse fields
5. Parse bytecode

### 7.2 Implement Bytecode Translator

**Files to Create:**
- [`preprocessor/src/bytecode.h`](preprocessor/src/bytecode.h)
- [`preprocessor/src/bytecode.c`](preprocessor/src/bytecode.c)

**Translation Strategy:**

```
JVM Bytecode          DOS Java Bytecode
--------------        ------------------
iconst_1       →      PUSH_CONST 1
iadd           →      ADD
istore_1       →      STORE_LOCAL 1
iload_1        →      LOAD_LOCAL 1
invokestatic   →      INVOKE_STATIC
return         →      RETURN
```

**Unsupported Instructions:**
- Long/float/double operations
- Exception handling
- Synchronization
- Reflection

### 7.3 Implement .djc File Writer

**Files to Create:**
- [`preprocessor/src/writer.h`](preprocessor/src/writer.h)
- [`preprocessor/src/writer.c`](preprocessor/src/writer.c)

**Functions to Implement:**

```c
DJCWriter* writer_create(const char* filename);
void writer_write_header(DJCWriter* w, DJCHeader* header);
void writer_write_constant_pool(DJCWriter* w, ConstantPool* pool);
void writer_write_methods(DJCWriter* w, Method* methods, uint16_t count);
void writer_close(DJCWriter* w);
```

### 7.4 Test Preprocessor

**Test Cases:**
1. Simple HelloWorld.class → HelloWorld.djc
2. Arithmetic operations
3. Method calls
4. Object creation

**Validation:**
- Generated .djc files are valid
- VM can load and execute them
- Output matches expected results

## Phase 8: Testing and Optimization (Week 8)

### 8.1 Create Test Suite

**Test Programs:**
1. HelloWorld - Basic output
2. Arithmetic - Math operations
3. Loops - Control flow
4. Methods - Method calls
5. Objects - Object creation and fields
6. Strings - String operations

**Test Runner:**
```batch
@echo off
echo Running DOS Java Test Suite
echo.

djpreproc tests\java\HelloWorld.class tests\HelloWorld.djc
dosjava tests\HelloWorld.djc > output.txt
fc output.txt tests\expected\HelloWorld.txt

REM ... more tests
```

### 8.2 Performance Profiling

**Metrics to Measure:**
- Execution time for test programs
- Memory usage (heap, stack)
- Class loading time
- Method invocation overhead

**Tools:**
- DOS timing functions
- Memory statistics from VM
- Custom profiling code

### 8.3 Optimization

**Areas to Optimize:**
1. **Interpreter Loop**
   - Direct threading if space allows
   - Inline common operations
   - Reduce function call overhead

2. **Memory Management**
   - Pool allocation for common sizes
   - Reduce fragmentation
   - Optimize GC triggers

3. **String Operations**
   - String interning for constants
   - Optimize concatenation
   - Reduce allocations

4. **Method Calls**
   - Cache method lookups
   - Inline trivial methods
   - Optimize stack frame operations

### 8.4 Documentation

**Documents to Complete:**
- User manual
- API reference
- Bytecode specification
- Porting guide
- Known limitations

## Phase 9: Polish and Release (Week 9)

### 9.1 Bug Fixes

- Fix all known bugs
- Handle edge cases
- Improve error messages
- Add input validation

### 9.2 Code Cleanup

- Remove debug code
- Add comments
- Consistent formatting
- Remove unused code

### 9.3 Build Release Version

```makefile
# Release build with optimizations
release:
    wcc -ms -ox -zq src\*.c
    wlink system dos file *.obj name dosjava.exe
```

### 9.4 Create Distribution

**Package Contents:**
- dosjava.exe - VM executable
- djpreproc.exe - Preprocessor
- README.txt - Quick start guide
- MANUAL.txt - User manual
- examples/ - Example programs
- LICENSE.txt - License information

## Success Criteria

### Minimum Viable Product (MVP)
- [ ] VM runs on DOS (real or emulated)
- [ ] Can execute HelloWorld program
- [ ] Basic arithmetic works
- [ ] System.out.println works
- [ ] Simple methods work

### Full Feature Set
- [ ] All planned opcodes implemented
- [ ] java.lang package complete
- [ ] Object creation and fields work
- [ ] Garbage collection functional
- [ ] Preprocessor converts .class files
- [ ] Test suite passes
- [ ] Documentation complete

## Risk Mitigation

### Technical Risks

**Risk:** Memory constraints too limiting
**Mitigation:** Implement memory-efficient data structures, consider Compact model

**Risk:** Performance too slow
**Mitigation:** Profile early, optimize hot paths, consider JIT for critical code

**Risk:** Open Watcom compatibility issues
**Mitigation:** Test frequently, use standard C features, avoid compiler-specific code

### Schedule Risks

**Risk:** Underestimated complexity
**Mitigation:** Implement MVP first, add features incrementally

**Risk:** Debugging difficulties
**Mitigation:** Build comprehensive logging, create debug tools early

## Next Steps After Completion

### Possible Enhancements
1. Support for more Java features
2. Larger memory models (Compact/Large)
3. Protected mode support (DPMI)
4. Basic java.io package
5. Simple AWT-like graphics
6. Network support (if DOS TCP/IP available)
7. JIT compilation for hot methods
8. Better garbage collection algorithm

### Community Engagement
- Release source code
- Create documentation
- Build example programs
- Gather feedback
- Accept contributions