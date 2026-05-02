# DOS Java Architecture

## System Overview

DOS Java is a lightweight Java runtime for 16-bit PC-DOS systems, consisting of three main components:

1. **Preprocessor** - Converts Java .class files to optimized .djc format
2. **Virtual Machine** - Executes .djc bytecode on DOS
3. **Runtime Library** - Provides java.lang classes in C

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Development Flow                          │
└─────────────────────────────────────────────────────────────────┘
                                 │
                                 ▼
                    ┌────────────────────────┐
                    │   Java Source Code     │
                    │     (.java files)      │
                    └───────────┬────────────┘
                                │
                                │ javac (standard Java compiler)
                                ▼
                    ┌────────────────────────┐
                    │  Java Class Files      │
                    │    (.class files)      │
                    └───────────┬────────────┘
                                │
                                │ djpreproc (our preprocessor)
                                ▼
                    ┌────────────────────────┐
                    │  DOS Java Bytecode     │
                    │     (.djc files)       │
                    └───────────┬────────────┘
                                │
                                │ dosjava.exe (our VM)
                                ▼
                    ┌────────────────────────┐
                    │    Program Output      │
                    └────────────────────────┘
```

## Component Architecture

### 1. Preprocessor (djpreproc.exe)

```
┌─────────────────────────────────────────────────────────────┐
│                    Preprocessor Pipeline                     │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  Input: HelloWorld.class                                     │
│     │                                                         │
│     ▼                                                         │
│  ┌──────────────────────┐                                   │
│  │  Class File Parser   │  Parse .class file format         │
│  │  - Read header       │  Extract all sections             │
│  │  - Parse const pool  │                                   │
│  │  - Parse methods     │                                   │
│  │  - Parse fields      │                                   │
│  └──────────┬───────────┘                                   │
│             │                                                 │
│             ▼                                                 │
│  ┌──────────────────────┐                                   │
│  │ Bytecode Translator  │  JVM bytecode → DJC bytecode      │
│  │  - Map instructions  │  Optimize for 16-bit              │
│  │  - Validate support  │  Remove unsupported ops           │
│  │  - Optimize          │                                   │
│  └──────────┬───────────┘                                   │
│             │                                                 │
│             ▼                                                 │
│  ┌──────────────────────┐                                   │
│  │   DJC File Writer    │  Generate .djc file               │
│  │  - Write header      │  Compact format                   │
│  │  - Write const pool  │  16-bit aligned                   │
│  │  - Write methods     │                                   │
│  │  - Write bytecode    │                                   │
│  └──────────┬───────────┘                                   │
│             │                                                 │
│             ▼                                                 │
│  Output: HelloWorld.djc                                      │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

### 2. Virtual Machine (dosjava.exe)

```
┌─────────────────────────────────────────────────────────────┐
│                   Virtual Machine Core                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              VM Initialization                       │   │
│  │  - Initialize memory manager                        │   │
│  │  - Set up heap and stack                            │   │
│  │  - Initialize GC                                    │   │
│  └─────────────────┬───────────────────────────────────┘   │
│                    │                                         │
│                    ▼                                         │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Class Loader                            │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │ 1. Open .djc file                            │  │   │
│  │  │ 2. Read and validate header                  │  │   │
│  │  │ 3. Load constant pool                        │  │   │
│  │  │ 4. Load method descriptors                   │  │   │
│  │  │ 5. Load field descriptors                    │  │   │
│  │  │ 6. Load bytecode section                     │  │   │
│  │  │ 7. Create ClassInfo structure                │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  └─────────────────┬───────────────────────────────────┘   │
│                    │                                         │
│                    ▼                                         │
│  ┌─────────────────────────────────────────────────────┐   │
│  │          Bytecode Interpreter                        │   │
│  │  ┌──────────────────────────────────────────────┐  │   │
│  │  │ Fetch-Decode-Execute Loop:                   │  │   │
│  │  │                                               │  │   │
│  │  │ while (running) {                            │  │   │
│  │  │   opcode = *pc++;                            │  │   │
│  │  │   switch (opcode) {                          │  │   │
│  │  │     case PUSH_CONST: ...                     │  │   │
│  │  │     case ADD: ...                            │  │   │
│  │  │     case INVOKE_STATIC: ...                  │  │   │
│  │  │     case RETURN: ...                         │  │   │
│  │  │   }                                           │  │   │
│  │  │ }                                             │  │   │
│  │  └──────────────────────────────────────────────┘  │   │
│  └─────────────────┬───────────────────────────────────┘   │
│                    │                                         │
│                    ▼                                         │
│  ┌─────────────────────────────────────────────────────┐   │
│  │           Runtime Library (Native)                   │   │
│  │  - java.lang.Object                                 │   │
│  │  - java.lang.String                                 │   │
│  │  - java.lang.System                                 │   │
│  │  - java.lang.Integer                                │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Memory Architecture

### Small Memory Model Layout

```
┌─────────────────────────────────────────────────────────────┐
│                    Code Segment (64KB)                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  0x0000  ┌──────────────────────────────────────┐           │
│          │  VM Core Code                        │  ~25KB    │
│          │  - Interpreter loop                  │           │
│          │  - Class loader                      │           │
│          │  - Memory manager                    │           │
│          ├──────────────────────────────────────┤           │
│          │  Runtime Library Code                │  ~15KB    │
│          │  - Object methods                    │           │
│          │  - String methods                    │           │
│          │  - System methods                    │           │
│          ├──────────────────────────────────────┤           │
│          │  Format Handling Code                │  ~5KB     │
│          │  - DJC file I/O                      │           │
│          │  - Constant pool access              │           │
│          ├──────────────────────────────────────┤           │
│          │  Utility Code                        │  ~5KB     │
│          │  - Debug functions                   │           │
│          │  - Helper functions                  │           │
│          ├──────────────────────────────────────┤           │
│          │  Reserve                             │  ~14KB    │
│  0xFFFF  └──────────────────────────────────────┘           │
│                                                               │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    Data Segment (64KB)                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  0x0000  ┌──────────────────────────────────────┐           │
│          │  Initialized Data                    │  ~2KB     │
│          │  - Global variables                  │           │
│          │  - String literals                   │           │
│          ├──────────────────────────────────────┤           │
│          │  BSS (Uninitialized)                 │  ~2KB     │
│          │  - Zero-initialized data             │           │
│          ├──────────────────────────────────────┤           │
│          │  Static Data                         │  ~10KB    │
│          │  - VM state                          │           │
│          │  - Loaded classes                    │           │
│          │  - Constant pools                    │           │
│          ├──────────────────────────────────────┤           │
│          │  Heap (grows up →)                   │  ~30KB    │
│          │  - Objects                           │           │
│          │  - Strings                           │           │
│          │  - Arrays                            │           │
│          │                                      │           │
│          │  ← Free Space →                      │           │
│          │                                      │           │
│          │  Stack (grows down ←)                │  ~20KB    │
│          │  - Method frames                     │           │
│          │  - Local variables                   │           │
│          │  - Operand stack                     │           │
│  0xFFFF  └──────────────────────────────────────┘           │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Execution Flow

### Method Invocation

```
┌─────────────────────────────────────────────────────────────┐
│                    Method Call Sequence                      │
└─────────────────────────────────────────────────────────────┘

1. Caller prepares arguments
   ┌──────────────┐
   │ Push arg1    │
   │ Push arg2    │
   │ ...          │
   └──────┬───────┘
          │
          ▼
2. INVOKE_STATIC/INVOKE_VIRTUAL
   ┌──────────────────────────────┐
   │ - Look up method             │
   │ - Save return PC             │
   │ - Create stack frame         │
   │ - Copy args to locals        │
   │ - Jump to method code        │
   └──────┬───────────────────────┘
          │
          ▼
3. Method execution
   ┌──────────────────────────────┐
   │ Execute bytecode             │
   │ Access local variables       │
   │ Use operand stack            │
   └──────┬───────────────────────┘
          │
          ▼
4. RETURN instruction
   ┌──────────────────────────────┐
   │ - Push return value (if any) │
   │ - Pop stack frame            │
   │ - Restore PC                 │
   │ - Continue caller            │
   └──────────────────────────────┘
```

### Object Lifecycle

```
┌─────────────────────────────────────────────────────────────┐
│                    Object Lifecycle                          │
└─────────────────────────────────────────────────────────────┘

1. Object Creation (NEW instruction)
   ┌──────────────────────────────┐
   │ - Allocate from heap         │
   │ - Initialize ref_count = 1   │
   │ - Set class_id               │
   │ - Initialize fields to 0     │
   │ - Push reference to stack    │
   └──────┬───────────────────────┘
          │
          ▼
2. Object Usage
   ┌──────────────────────────────┐
   │ - GET_FIELD / PUT_FIELD      │
   │ - INVOKE_VIRTUAL             │
   │ - Pass as argument           │
   └──────┬───────────────────────┘
          │
          ▼
3. Reference Management
   ┌──────────────────────────────┐
   │ Assignment: ref_count++      │
   │ Scope exit: ref_count--      │
   └──────┬───────────────────────┘
          │
          ▼
4. Garbage Collection
   ┌──────────────────────────────┐
   │ if (ref_count == 0) {        │
   │   - Free object memory       │
   │   - Update heap stats        │
   │ }                             │
   └──────────────────────────────┘
```

## Data Structures

### Core VM Structures

```c
// Virtual Machine state
typedef struct {
    uint8_t* heap_start;        // Heap base address
    uint16_t heap_size;         // Total heap size
    uint16_t heap_used;         // Bytes allocated
    ClassInfo** classes;        // Loaded classes
    uint16_t class_count;       // Number of classes
    ExecutionContext* context;  // Current execution context
} VM;

// Execution context
typedef struct {
    uint8_t* pc;                // Program counter
    uint16_t* stack;            // Operand stack
    uint16_t sp;                // Stack pointer
    uint16_t* locals;           // Local variables
    ClassInfo* current_class;   // Current class
    uint16_t current_method;    // Current method index
} ExecutionContext;

// Class information
typedef struct {
    uint16_t class_id;          // Unique class ID
    char* class_name;           // Class name
    DJCMethod* methods;         // Method array
    DJCField* fields;           // Field array
    uint16_t method_count;      // Number of methods
    uint16_t field_count;       // Number of fields
    uint8_t* bytecode;          // Bytecode section
    uint16_t bytecode_size;     // Bytecode size
} ClassInfo;

// Object base structure
typedef struct {
    uint16_t class_id;          // Class identifier
    uint16_t ref_count;         // Reference count for GC
} Object;

// String object
typedef struct {
    Object base;                // Base object
    uint16_t length;            // String length
    char* data;                 // Character data
} String;
```

## Bytecode Instruction Set

### Instruction Format

```
┌────────────────────────────────────────────────────────┐
│  Opcode (1 byte)  │  Operands (0-4 bytes)             │
└────────────────────────────────────────────────────────┘

Examples:
  RETURN          : 0x00
  PUSH_CONST idx  : 0x01 [idx:2]
  ADD             : 0x02
  LOAD_LOCAL idx  : 0x03 [idx:1]
  INVOKE_STATIC m : 0x10 [method:2]
```

### Instruction Categories

```
Stack Operations (0x00-0x0F):
  0x00  RETURN          - Return from method
  0x01  PUSH_CONST      - Push constant
  0x02  POP             - Pop stack
  0x03  DUP             - Duplicate top
  0x04  LOAD_LOCAL      - Load local variable
  0x05  STORE_LOCAL     - Store local variable

Arithmetic (0x10-0x1F):
  0x10  ADD             - Integer addition
  0x11  SUB             - Integer subtraction
  0x12  MUL             - Integer multiplication
  0x13  DIV             - Integer division
  0x14  MOD             - Integer modulo
  0x15  NEG             - Negate

Comparison (0x20-0x2F):
  0x20  CMP_EQ          - Compare equal
  0x21  CMP_NE          - Compare not equal
  0x22  CMP_LT          - Compare less than
  0x23  CMP_LE          - Compare less or equal
  0x24  CMP_GT          - Compare greater than
  0x25  CMP_GE          - Compare greater or equal

Control Flow (0x30-0x3F):
  0x30  GOTO            - Unconditional jump
  0x31  IF_TRUE         - Jump if true
  0x32  IF_FALSE        - Jump if false

Method Invocation (0x40-0x4F):
  0x40  INVOKE_STATIC   - Static method call
  0x41  INVOKE_VIRTUAL  - Virtual method call

Object Operations (0x50-0x5F):
  0x50  NEW             - Create object
  0x51  GET_FIELD       - Read field
  0x52  PUT_FIELD       - Write field
```

## Integration Points

### DOS System Calls

```c
// Console output (INT 21h, AH=02h)
void dos_putchar(char c) {
    _asm {
        mov ah, 02h
        mov dl, c
        int 21h
    }
}

// File operations (INT 21h)
int dos_open(const char* filename, int mode);
int dos_read(int handle, void* buffer, int count);
int dos_close(int handle);
```

### Open Watcom Integration

```c
// Memory model specification
#pragma aux memory_model "ms"  // Small model

// Far pointers (if needed for future expansion)
char far* far_ptr;

// Inline assembly for performance-critical code
void fast_memcpy(void* dest, void* src, int count) {
    _asm {
        push ds
        push es
        mov cx, count
        lds si, src
        les di, dest
        rep movsb
        pop es
        pop ds
    }
}
```

## Performance Considerations

### Optimization Strategies

1. **Interpreter Loop**
   - Use switch statement (compiler optimizes to jump table)
   - Inline common operations
   - Minimize function calls in hot path

2. **Memory Access**
   - Keep frequently accessed data in near memory
   - Align structures to 2-byte boundaries
   - Use local variables instead of globals

3. **Method Calls**
   - Cache method lookups
   - Inline trivial methods
   - Minimize stack frame overhead

4. **String Operations**
   - Intern constant strings
   - Avoid unnecessary allocations
   - Use efficient string comparison

## Error Handling

### Error Categories

```
1. Load-time Errors:
   - File not found
   - Invalid .djc format
   - Unsupported bytecode version
   - Memory allocation failure

2. Runtime Errors:
   - Stack overflow
   - Heap exhausted
   - Invalid bytecode instruction
   - Null pointer access
   - Array index out of bounds

3. System Errors:
   - DOS I/O errors
   - Insufficient memory
   - Invalid system call
```

### Error Reporting

```c
typedef enum {
    ERR_NONE = 0,
    ERR_FILE_NOT_FOUND,
    ERR_INVALID_FORMAT,
    ERR_OUT_OF_MEMORY,
    ERR_STACK_OVERFLOW,
    ERR_INVALID_BYTECODE,
    ERR_NULL_POINTER
} ErrorCode;

void vm_error(ErrorCode code, const char* message);
```

## Testing Architecture

### Test Levels

```
1. Unit Tests (C code):
   - Memory manager
   - Stack operations
   - Bytecode decoder
   - GC functions

2. Integration Tests (.djc files):
   - Class loading
   - Method invocation
   - Object creation
   - String operations

3. System Tests (Java programs):
   - HelloWorld
   - Arithmetic
   - Loops
   - Objects
```

## Future Expansion Paths

### Possible Enhancements

1. **Memory Models**
   - Compact model (64KB code, multiple data segments)
   - Large model (multiple code and data segments)

2. **Protected Mode**
   - DPMI support for more memory
   - 32-bit operations

3. **Advanced Features**
   - Basic exception handling
   - Simple java.io package
   - Primitive graphics support

4. **Performance**
   - JIT compilation for hot methods
   - Better garbage collection
   - Bytecode optimization