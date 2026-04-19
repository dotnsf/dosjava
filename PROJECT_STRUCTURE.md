# DOS Java Project Structure

## Directory Layout

```
dosjava/
├── README.md                    # Project overview and quick start
├── TECHNICAL_SPEC.md           # Detailed technical specification
├── PROJECT_STRUCTURE.md        # This file
├── IMPLEMENTATION_ROADMAP.md   # Step-by-step implementation guide
├── BUILD.md                    # Build instructions
│
├── src/                        # Source code
│   ├── vm/                     # Virtual machine core
│   │   ├── vm.h               # Main VM header
│   │   ├── vm.c               # VM initialization and main loop
│   │   ├── memory.h           # Memory management
│   │   ├── memory.c
│   │   ├── classloader.h      # Class loading
│   │   ├── classloader.c
│   │   ├── interpreter.h      # Bytecode interpreter
│   │   ├── interpreter.c
│   │   ├── stack.h            # Operand stack management
│   │   ├── stack.c
│   │   ├── gc.h               # Garbage collector
│   │   └── gc.c
│   │
│   ├── runtime/               # Java runtime library (C implementation)
│   │   ├── object.h          # java.lang.Object
│   │   ├── object.c
│   │   ├── string.h          # java.lang.String
│   │   ├── string.c
│   │   ├── system.h          # java.lang.System
│   │   ├── system.c
│   │   ├── integer.h         # java.lang.Integer
│   │   ├── integer.c
│   │   ├── boolean.h         # java.lang.Boolean
│   │   └── boolean.c
│   │
│   ├── format/                # .djc file format handling
│   │   ├── djc.h             # .djc format definitions
│   │   ├── djc.c             # .djc file I/O
│   │   └── constants.h       # Constant pool handling
│   │
│   ├── util/                  # Utility functions
│   │   ├── util.h
│   │   ├── util.c
│   │   ├── debug.h           # Debug output
│   │   └── debug.c
│   │
│   └── main.c                 # Entry point
│
├── preprocessor/              # .class to .djc converter
│   ├── src/
│   │   ├── main.c            # Preprocessor entry point
│   │   ├── classfile.h       # Java .class file parser
│   │   ├── classfile.c
│   │   ├── bytecode.h        # Bytecode translator
│   │   ├── bytecode.c
│   │   ├── writer.h          # .djc file writer
│   │   └── writer.c
│   ├── Makefile              # Build for host system
│   └── README.md
│
├── tests/                     # Test programs
│   ├── java/                 # Java source files
│   │   ├── HelloWorld.java
│   │   ├── Arithmetic.java
│   │   ├── Loops.java
│   │   ├── Objects.java
│   │   └── Strings.java
│   ├── expected/             # Expected output
│   │   ├── HelloWorld.txt
│   │   ├── Arithmetic.txt
│   │   └── ...
│   └── run_tests.bat         # Test runner script
│
├── build/                     # Build output (generated)
│   ├── obj/                  # Object files
│   └── bin/                  # Executables
│
├── docs/                      # Additional documentation
│   ├── BYTECODE_SPEC.md      # Bytecode instruction reference
│   ├── API_REFERENCE.md      # Java API reference
│   ├── MEMORY_LAYOUT.md      # Memory management details
│   └── PORTING_GUIDE.md      # Guide for porting to other platforms
│
├── examples/                  # Example programs
│   ├── hello/
│   │   ├── Hello.java
│   │   └── README.md
│   ├── calculator/
│   │   ├── Calc.java
│   │   └── README.md
│   └── ...
│
├── tools/                     # Development tools
│   ├── disasm.c              # .djc disassembler
│   ├── hexdump.c             # Hex dump utility
│   └── Makefile
│
└── Makefile                   # Main build file (Open Watcom)
```

## Key Files Description

### Core VM Files

**`src/vm/vm.h` & `vm.c`**
- VM initialization and shutdown
- Main execution loop
- Global VM state management

**`src/vm/memory.h` & `memory.c`**
- Heap allocation/deallocation
- Memory pool management
- Memory statistics and debugging

**`src/vm/classloader.h` & `classloader.c`**
- Load .djc files from disk
- Parse class structure
- Maintain loaded class registry

**`src/vm/interpreter.h` & `interpreter.c`**
- Bytecode fetch-decode-execute loop
- Instruction implementations
- Method invocation logic

**`src/vm/stack.h` & `stack.c`**
- Operand stack operations
- Local variable access
- Stack frame management

**`src/vm/gc.h` & `gc.c`**
- Reference counting
- Object lifecycle management
- Memory reclamation

### Runtime Library Files

**`src/runtime/object.h` & `object.c`**
- Base Object structure
- equals(), hashCode(), toString()
- Object allocation helpers

**`src/runtime/string.h` & `string.c`**
- String structure and operations
- String pool (if space allows)
- String manipulation functions

**`src/runtime/system.h` & `system.c`**
- System.out.print/println
- DOS console I/O
- System utilities

**`src/runtime/integer.h` & `integer.c`**
- Integer wrapper class
- Boxing/unboxing
- Integer utilities

### Format Handling Files

**`src/format/djc.h` & `djc.c`**
- .djc file format structures
- File reading/writing
- Format validation

**`src/format/constants.h`**
- Constant pool definitions
- Constant types and access

### Preprocessor Files

**`preprocessor/src/classfile.h` & `classfile.c`**
- Parse Java .class file format
- Extract constant pool
- Read method and field descriptors

**`preprocessor/src/bytecode.h` & `bytecode.c`**
- Translate JVM bytecode to .djc bytecode
- Optimize instruction sequences
- Validate bytecode compatibility

**`preprocessor/src/writer.h` & `writer.c`**
- Generate .djc file format
- Write constant pool
- Write method code

## Build Targets

### Primary Targets

1. **dosjava.exe** - Main VM executable for DOS
   - Compiled with Open Watcom v2
   - Small memory model
   - Runs on 8086+

2. **djpreproc.exe** - Preprocessor tool
   - Compiled on host system (Windows/Linux)
   - Converts .class to .djc

3. **djdisasm.exe** - Disassembler tool (optional)
   - Debug tool for .djc files
   - Shows bytecode in readable format

### Test Targets

- **test_memory** - Memory manager unit tests
- **test_stack** - Stack operations tests
- **test_gc** - Garbage collector tests
- **test_loader** - Class loader tests

## File Naming Conventions

- **Headers**: `.h` extension
- **C Source**: `.c` extension
- **Java Source**: `.java` extension
- **Java Bytecode**: `.class` extension
- **DOS Java Bytecode**: `.djc` extension
- **Makefiles**: `Makefile` or `makefile`
- **Documentation**: `.md` extension (Markdown)

## Coding Standards

### C Code Style

```c
// Function naming: lowercase with underscores
void load_class(const char* filename);

// Type naming: PascalCase with typedef
typedef struct {
    uint16_t value;
} Integer;

// Constants: UPPERCASE with underscores
#define MAX_STACK_SIZE 256

// Pointer declarations: asterisk with type
char* str;
Object* obj;

// Braces: K&R style
if (condition) {
    // code
} else {
    // code
}
```

### Header Guards

```c
#ifndef VM_H
#define VM_H

// declarations

#endif /* VM_H */
```

### Comments

```c
/**
 * Load a class from a .djc file
 * 
 * @param filename Path to .djc file
 * @return Pointer to loaded ClassInfo, or NULL on error
 */
ClassInfo* load_class(const char* filename);
```

## Dependencies

### Required Tools

- **Open Watcom v2** - C compiler for DOS target
- **Java Development Kit (JDK)** - For compiling test programs
- **Make** - Build automation
- **Text Editor** - For development

### Optional Tools

- **DOSBox** or **QEMU** - For testing on modern systems
- **Git** - Version control
- **Doxygen** - Documentation generation

## Build Process Overview

```
┌─────────────────┐
│  Java Source    │
│  (.java)        │
└────────┬────────┘
         │ javac
         ▼
┌─────────────────┐
│  Java Bytecode  │
│  (.class)       │
└────────┬────────┘
         │ djpreproc
         ▼
┌─────────────────┐
│  DOS Java BC    │
│  (.djc)         │
└────────┬────────┘
         │
         │  ┌──────────────┐
         │  │  VM Source   │
         │  │  (.c, .h)    │
         │  └──────┬───────┘
         │         │ wcc (Open Watcom)
         │         ▼
         │  ┌──────────────┐
         │  │  Object Files│
         │  │  (.obj)      │
         │  └──────┬───────┘
         │         │ wlink
         │         ▼
         │  ┌──────────────┐
         └─>│  dosjava.exe │
            └──────┬───────┘
                   │ Execute on DOS
                   ▼
            ┌──────────────┐
            │    Output    │
            └──────────────┘
```

## Memory Budget

### Code Segment (~64KB)
- VM Core: ~25KB
- Runtime Library: ~15KB
- Format Handling: ~5KB
- Utilities: ~5KB
- **Reserve**: ~14KB

### Data Segment (~64KB)
- Static Data: ~10KB
- Stack: ~20KB
- Heap: ~30KB
- **Reserve**: ~4KB

## Version Control Strategy

### Branch Structure
- `main` - Stable releases
- `develop` - Active development
- `feature/*` - Feature branches
- `bugfix/*` - Bug fixes

### Commit Message Format
```
[component] Brief description

Detailed explanation if needed

Fixes: #issue_number
```

Example:
```
[vm] Implement basic bytecode interpreter

Added fetch-decode-execute loop with support for
arithmetic and control flow instructions.

Fixes: #12
```

## Testing Strategy

### Unit Tests
- Test individual components in isolation
- Run on host system (faster development)

### Integration Tests
- Test VM with actual .djc files
- Run on DOSBox/QEMU

### Regression Tests
- Maintain test suite of working programs
- Run after each significant change

### Performance Tests
- Measure execution speed
- Monitor memory usage
- Profile hot paths

## Documentation Requirements

Each major component should have:
1. Header comment explaining purpose
2. Function documentation
3. Usage examples
4. Known limitations
5. Future improvements

## Release Checklist

- [ ] All tests pass
- [ ] Documentation updated
- [ ] Version number incremented
- [ ] CHANGELOG.md updated
- [ ] Build on clean system
- [ ] Test on real DOS (if possible)
- [ ] Create release archive
- [ ] Tag release in git