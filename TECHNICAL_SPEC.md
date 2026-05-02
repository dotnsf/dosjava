# DOS Java Technical Specification

## Project Overview

A lightweight Java subset implementation for 16-bit PC-DOS, cross-compiled with Open Watcom v2 C compiler.

## Target Environment

- **Platform**: 16-bit PC-DOS (MS-DOS compatible)
- **Memory Model**: Small (64KB code segment + 64KB data segment)
- **Mode**: Real mode (640KB memory limit)
- **Compiler**: Open Watcom v2 C compiler
- **Target CPU**: 8086/8088 or higher

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                    DOS Java System                       │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌──────────────┐         ┌──────────────────────────┐  │
│  │ Preprocessor │────────>│  Lightweight Bytecode    │  │
│  │ (.class →    │         │  Format (.djc)           │  │
│  │  .djc)       │         └──────────────────────────┘  │
│  └──────────────┘                    │                   │
│                                      ▼                   │
│  ┌───────────────────────────────────────────────────┐  │
│  │           DOS Java Virtual Machine                │  │
│  ├───────────────────────────────────────────────────┤  │
│  │  Class Loader  │  Bytecode Interpreter            │  │
│  │  Memory Mgr    │  Method Invocation               │  │
│  │  GC (Simple)   │  Stack Management                │  │
│  └───────────────────────────────────────────────────┘  │
│                          │                               │
│                          ▼                               │
│  ┌───────────────────────────────────────────────────┐  │
│  │        Minimal java.lang Package (C impl)         │  │
│  │  Object, String, System, Integer, Boolean, etc.   │  │
│  └───────────────────────────────────────────────────┘  │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

## Memory Layout (Small Model)

```
┌──────────────────┐ 0xFFFF
│   Stack (grows   │
│      down)       │
├──────────────────┤
│   Heap (grows    │
│       up)        │
├──────────────────┤
│   Static Data    │
│   (VM state,     │
│    class data)   │
├──────────────────┤
│   BSS            │
├──────────────────┤
│   Initialized    │
│   Data           │
└──────────────────┘ 0x0000

Code Segment (separate 64KB):
┌──────────────────┐
│   VM Code        │
│   Native Methods │
└──────────────────┘
```

## Lightweight Bytecode Format (.djc)

### Design Goals
- Minimize memory footprint
- Optimize for 16-bit operations
- Reduce parsing overhead
- Support essential Java operations only

### File Format Structure

```c
// .djc file header
struct DJCHeader {
    uint16_t magic;           // 0x444A ('DJ')
    uint16_t version;         // Format version
    uint16_t constant_pool_count;
    uint16_t method_count;
    uint16_t field_count;
    uint16_t code_size;       // Total bytecode size
};

// Constant pool entry (simplified)
struct DJCConstant {
    uint8_t tag;              // Type: UTF8, INT, STRING_REF, etc.
    uint16_t length;          // Data length
    // Followed by data
};

// Method descriptor
struct DJCMethod {
    uint16_t name_index;      // Index to constant pool
    uint16_t descriptor_index;
    uint16_t code_offset;     // Offset in code section
    uint16_t code_length;
    uint8_t max_stack;
    uint8_t max_locals;
};

// Field descriptor
struct DJCField {
    uint16_t name_index;
    uint16_t descriptor_index;
    uint8_t flags;            // static, final, etc.
};
```

### Supported Bytecode Instructions (Subset)

**Stack Operations:**
- `PUSH_CONST` - Push constant to stack
- `POP` - Pop from stack
- `DUP` - Duplicate top of stack
- `LOAD_LOCAL` - Load local variable
- `STORE_LOCAL` - Store to local variable

**Arithmetic:**
- `ADD`, `SUB`, `MUL`, `DIV`, `MOD` (int only)
- `NEG` - Negate

**Comparison:**
- `CMP_EQ`, `CMP_NE`, `CMP_LT`, `CMP_LE`, `CMP_GT`, `CMP_GE`

**Control Flow:**
- `GOTO` - Unconditional jump
- `IF_TRUE`, `IF_FALSE` - Conditional jumps
- `CALL` - Method invocation
- `RETURN` - Return from method

**Object Operations:**
- `NEW` - Create object
- `GET_FIELD`, `PUT_FIELD` - Field access
- `INVOKE_VIRTUAL` - Virtual method call
- `INVOKE_STATIC` - Static method call

## Java Language Subset

### Supported Features

**Data Types:**
- Primitives: `int` (16-bit), `boolean`, `char`
- Reference: `Object`, `String`, arrays (limited)

**Language Constructs:**
- Classes (single inheritance only)
- Methods (instance and static)
- Fields (instance and static)
- Control flow: if/else, while, for
- Basic operators: arithmetic, comparison, logical

### NOT Supported (Initial Version)

- `long`, `float`, `double` types
- Interfaces
- Abstract classes
- Exception handling (try/catch)
- Multithreading
- Reflection
- Inner classes
- Generics
- Annotations

## Minimal java.lang Package

### Core Classes to Implement

```c
// java.lang.Object
typedef struct {
    uint16_t class_id;
    uint16_t ref_count;  // For simple GC
} Object;

// java.lang.String
typedef struct {
    Object base;
    uint16_t length;
    char* data;  // Far pointer if needed
} String;

// java.lang.System
// Static methods only, no object representation

// java.lang.Integer
typedef struct {
    Object base;
    int16_t value;
} Integer;

// java.lang.Boolean
typedef struct {
    Object base;
    uint8_t value;
} Boolean;
```

### Key Methods

**Object:**
- `equals(Object)` - Reference equality
- `toString()` - Return string representation
- `hashCode()` - Simple hash

**String:**
- `length()` - Get string length
- `charAt(int)` - Get character at index
- `concat(String)` - Concatenate strings
- `equals(Object)` - String comparison

**System:**
- `System.out.print(String)` - Print to stdout
- `System.out.println(String)` - Print with newline

**Integer:**
- `Integer(int)` - Constructor
- `intValue()` - Get primitive value
- `toString()` - Convert to string

## Virtual Machine Components

### 1. Class Loader

```c
typedef struct {
    uint16_t class_id;
    char* class_name;
    DJCMethod* methods;
    DJCField* fields;
    uint16_t method_count;
    uint16_t field_count;
    uint8_t* bytecode;
} ClassInfo;

// Load .djc file into memory
ClassInfo* load_class(const char* filename);
```

### 2. Memory Manager

```c
// Simple heap allocator for Small model
typedef struct {
    uint8_t* heap_start;
    uint8_t* heap_ptr;
    uint16_t heap_size;
    uint16_t bytes_allocated;
} MemoryManager;

void* djvm_alloc(uint16_t size);
void djvm_free(void* ptr);
```

### 3. Bytecode Interpreter

```c
typedef struct {
    uint16_t* stack;      // Operand stack
    uint16_t sp;          // Stack pointer
    uint16_t* locals;     // Local variables
    uint8_t* pc;          // Program counter
    ClassInfo* current_class;
} ExecutionContext;

void execute_method(ExecutionContext* ctx, uint16_t method_index);
```

### 4. Simple Garbage Collector

```c
// Reference counting GC (simple but suitable for 16-bit)
void gc_add_ref(Object* obj);
void gc_release(Object* obj);
void gc_collect();  // Manual collection if needed
```

## Preprocessor Tool

### Purpose
Convert standard Java `.class` files to lightweight `.djc` format.

### Implementation
- Written in C (can run on modern systems)
- Reads Java class file format
- Extracts essential information
- Generates optimized `.djc` file
- Validates bytecode compatibility

### Usage
```
djpreproc HelloWorld.class -o HelloWorld.djc
```

## Build System

### Open Watcom v2 Makefile Structure

```makefile
# Compiler settings
CC = wcc
CFLAGS = -ms -0 -w4 -zq -od
LDFLAGS = -ms

# Source files
OBJS = main.obj vm.obj classloader.obj interpreter.obj \
       memory.obj gc.obj string.obj system.obj

# Build target
dosjava.exe: $(OBJS)
    wlink system dos file {$(OBJS)}

# Pattern rules
.c.obj:
    $(CC) $(CFLAGS) $<

clean:
    del *.obj
    del dosjava.exe
```

## Performance Considerations

### Memory Optimization
- Use 16-bit integers instead of 32-bit where possible
- Minimize pointer indirection
- Pack structures to reduce padding
- Use near pointers within segments

### Execution Optimization
- Direct threaded interpreter (if space allows)
- Inline common operations
- Cache method lookups
- Minimize DOS system calls

### Size Constraints
- Total VM code: ~40KB (leaving room for data)
- Runtime data: ~50KB (leaving room for heap)
- Heap space: ~10-14KB for objects

## Testing Strategy

### Test Programs

1. **Hello World**
```java
class Hello {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

2. **Arithmetic Test**
```java
class Math {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b;
        System.out.println(c);
    }
}
```

3. **Object Test**
```java
class Point {
    int x;
    int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
}
```

## Development Phases

### Phase 1: Foundation (Weeks 1-2)
- Set up Open Watcom v2 build environment
- Implement basic memory manager
- Create .djc file format specification
- Implement simple class loader

### Phase 2: Core VM (Weeks 3-4)
- Implement bytecode interpreter
- Add stack management
- Implement method invocation
- Basic control flow support

### Phase 3: Java Runtime (Weeks 5-6)
- Implement Object class
- Implement String class
- Implement System.out
- Implement Integer wrapper

### Phase 4: Preprocessor (Week 7)
- Build .class to .djc converter
- Test with javac-compiled classes
- Validate bytecode translation

### Phase 5: Testing & Optimization (Week 8)
- Run test suite
- Profile memory usage
- Optimize critical paths
- Fix bugs

## Limitations and Trade-offs

### Known Limitations
- No floating-point support (8086 has no FPU)
- Limited heap size (~10-14KB)
- No exception handling
- Single-threaded only
- No dynamic class loading
- Limited string operations

### Design Trade-offs
- Reference counting GC (simple but may leak cycles)
- Custom bytecode format (not Java-compatible)
- 16-bit integers (may overflow easily)
- No JIT compilation (interpreter only)

## Future Enhancements

### Possible Improvements
- Support for 80286+ protected mode (more memory)
- Compact/Large memory model support
- Basic exception handling
- More complete java.lang package
- Simple java.io package
- Optimization passes in preprocessor

## References

- Java Virtual Machine Specification (for bytecode reference)
- Open Watcom v2 Documentation
- DOS Programming Reference
- 8086 Assembly Language Reference