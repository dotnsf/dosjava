# Phase 5: Documentation Plan

## Documentation Structure

```
dosjava/
├── README.md                    # Project overview and quick start
├── QUICKSTART.md               # 5-minute getting started guide
├── COMPILER_USAGE.md           # Detailed compiler usage
├── LANGUAGE_SPEC.md            # Supported Java subset
├── BYTECODE_SPEC.md            # .djc bytecode format
├── EXAMPLES.md                 # Example programs
├── TROUBLESHOOTING.md          # Common issues and solutions
├── ARCHITECTURE.md             # System architecture (existing)
├── BUILD.md                    # Build instructions (existing)
└── docs/
    ├── api/
    │   ├── compiler_api.md     # Compiler API reference
    │   ├── vm_api.md           # VM API reference
    │   └── runtime_api.md      # Runtime library API
    ├── internals/
    │   ├── lexer.md            # Lexer internals
    │   ├── parser.md           # Parser internals
    │   ├── semantic.md         # Semantic analyzer internals
    │   └── codegen.md          # Code generator internals
    └── tutorials/
        ├── hello_world.md      # Hello World tutorial
        ├── control_flow.md     # Control flow tutorial
        └── methods.md          # Methods tutorial
```

## 1. README.md (Project Overview)

### Content Outline

```markdown
# DOS Java Compiler

16-bit PC-DOS上で動作するJavaコンパイラとランタイム環境

## Overview
- What is DOS Java?
- Key features
- System requirements
- Project status

## Quick Start
```batch
# Compile Java source
djc Hello.java

# Run bytecode
djvm Hello.djc
```

## Features
- Java subset to .djc bytecode compilation
- Stack-based virtual machine
- Optimized for 16-bit DOS
- Complete toolchain

## Supported Java Subset
- Primitive types: int, boolean
- Control flow: if, while, return
- Operators: arithmetic, relational, logical
- Methods: static methods
- Classes: single class per file

## System Requirements
- 16-bit PC-DOS or compatible
- 640KB RAM minimum
- Open Watcom v2 (for building)

## Building
```batch
wmake all
```

## Usage
```batch
# Compile
djc [options] source.java

# Run
djvm bytecode.djc
```

## Documentation
- [Quick Start Guide](QUICKSTART.md)
- [Compiler Usage](COMPILER_USAGE.md)
- [Language Specification](LANGUAGE_SPEC.md)
- [Examples](EXAMPLES.md)

## Project Structure
- `src/` - VM and runtime source
- `tools/` - Compiler and utilities
- `tests/` - Test suite
- `docs/` - Documentation

## License
MIT License

## Contributing
See CONTRIBUTING.md

## Authors
- Your Name

## Acknowledgments
- Open Watcom Project
- Java Language Specification
```

## 2. QUICKSTART.md (5-Minute Guide)

### Content Outline

```markdown
# Quick Start Guide

Get started with DOS Java in 5 minutes!

## Prerequisites
- DOS or DOSBox
- djc.exe and djvm.exe in PATH

## Step 1: Write Your First Program

Create `Hello.java`:
```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

## Step 2: Compile

```batch
djc Hello.java
```

Output:
```
Compiled: Hello.java -> Hello.djc
```

## Step 3: Run

```batch
djvm Hello.djc
```

## Step 4: Try More Examples

### Arithmetic
```java
class Math {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b * 2;
        return;
    }
}
```

### Control Flow
```java
class Loop {
    public static void main() {
        int i = 0;
        while (i < 10) {
            i = i + 1;
        }
        return;
    }
}
```

## Next Steps
- Read [Compiler Usage](COMPILER_USAGE.md)
- Explore [Examples](EXAMPLES.md)
- Learn [Language Specification](LANGUAGE_SPEC.md)
```

## 3. COMPILER_USAGE.md (Detailed Usage)

### Content Outline

```markdown
# Compiler Usage Guide

Complete guide to using the DOS Java Compiler (djc)

## Command-Line Interface

### Basic Usage
```batch
djc [options] <source.java>
```

### Options

#### Output File (-o)
```batch
djc -o output.djc source.java
```
Specify output file name (default: source.djc)

#### Keep Intermediates (-k)
```batch
djc -k source.java
```
Keep intermediate files (.tok, .ast, .sym)

#### Verbose (-v)
```batch
djc -v source.java
```
Show detailed compilation progress

#### Dump Tokens (--dump-tokens)
```batch
djc --dump-tokens source.java
```
Save token stream to .tok file

#### Dump AST (--dump-ast)
```batch
djc --dump-ast source.java
```
Save abstract syntax tree to .ast file

#### Dump Symbols (--dump-symbols)
```batch
djc --dump-symbols source.java
```
Save symbol table to .sym file

#### Help (-h, --help)
```batch
djc --help
```
Show usage information

#### Version (--version)
```batch
djc --version
```
Show version information

## Compilation Process

### Phase 1: Lexical Analysis
- Reads source file
- Generates tokens
- Detects lexical errors

### Phase 2: Parsing
- Builds abstract syntax tree
- Checks syntax
- Reports parse errors

### Phase 3: Semantic Analysis
- Type checking
- Symbol resolution
- Scope validation

### Phase 4: Code Generation
- Generates bytecode
- Optimizes code
- Writes .djc file

## Error Messages

### Lexical Errors
```
Error: Lexical analysis failed at line 5, column 10
Unterminated string literal
```

### Parse Errors
```
Error: Parse error at line 3, column 16
Expected ';' after expression
```

### Semantic Errors
```
Error: Semantic analysis failed
Type mismatch: cannot assign int to boolean
```

### Code Generation Errors
```
Error: Code generation failed
Failed to write output file: disk full
```

## Best Practices

### File Organization
- One class per file
- File name matches class name
- Use .java extension

### Compilation
- Compile with -v for debugging
- Use -k to inspect intermediates
- Check error messages carefully

### Performance
- Keep files small (<1000 lines)
- Minimize nested scopes
- Use local variables efficiently

## Troubleshooting

### Compilation Fails
1. Check syntax
2. Verify file exists
3. Check disk space
4. Review error message

### Output File Not Created
1. Check write permissions
2. Verify disk space
3. Check output path

### Slow Compilation
1. Reduce file size
2. Simplify expressions
3. Check available memory

## Examples

### Simple Compilation
```batch
djc Hello.java
```

### Custom Output
```batch
djc -o test.djc Test.java
```

### Debug Compilation
```batch
djc -k -v --dump-ast Hello.java
```

### Batch Compilation
```batch
for %%f in (*.java) do djc %%f
```
```

## 4. LANGUAGE_SPEC.md (Language Specification)

### Content Outline

```markdown
# DOS Java Language Specification

Supported subset of Java language

## Overview
DOS Java supports a minimal but useful subset of Java, optimized for 16-bit DOS environment.

## Lexical Elements

### Keywords
- `class` - Class declaration
- `public` - Public modifier
- `static` - Static modifier
- `void` - Void return type
- `int` - Integer type
- `boolean` - Boolean type
- `if` - Conditional statement
- `while` - Loop statement
- `return` - Return statement
- `true` - Boolean literal
- `false` - Boolean literal

### Operators
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Relational: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`

### Literals
- Integer: `0`, `42`, `-10`
- Boolean: `true`, `false`

### Identifiers
- Start with letter or underscore
- Followed by letters, digits, underscores
- Case-sensitive

## Syntax

### Class Declaration
```java
class ClassName {
    // members
}
```

### Method Declaration
```java
public static ReturnType methodName(Parameters) {
    // body
}
```

### Variable Declaration
```java
Type variableName = initializer;
```

### If Statement
```java
if (condition) {
    // then-block
}
```

### While Statement
```java
while (condition) {
    // body
}
```

### Return Statement
```java
return;           // void return
return expression; // value return
```

## Type System

### Primitive Types
- `int` - 16-bit signed integer (-32768 to 32767)
- `boolean` - Boolean value (true or false)
- `void` - No return value

### Type Conversion
- No implicit conversions
- No casting supported

## Scoping Rules

### Block Scope
Variables declared in a block are visible only within that block.

### Method Scope
Parameters and local variables are visible within method.

### Class Scope
Static methods are visible within class.

## Limitations

### Not Supported
- Objects and object creation
- Arrays
- Strings (as objects)
- Inheritance
- Interfaces
- Exceptions
- Generics
- Annotations
- Packages
- Imports
- Float/double types
- Long type
- Char type
- For loops
- Do-while loops
- Switch statements
- Break/continue
- Multiple classes per file

### Memory Limits
- Maximum 256 local variables per method
- Maximum 256 methods per class
- Maximum 64KB bytecode per class

## Examples

### Valid Programs

#### Hello World
```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

#### Fibonacci
```java
class Fibonacci {
    public static void main() {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        return;
    }
}
```

### Invalid Programs

#### Multiple Classes
```java
// NOT SUPPORTED
class A {
}
class B {
}
```

#### Arrays
```java
// NOT SUPPORTED
int[] array = new int[10];
```

#### Objects
```java
// NOT SUPPORTED
Object obj = new Object();
```
```

## 5. BYTECODE_SPEC.md (Bytecode Format)

### Content Outline

```markdown
# .djc Bytecode Format Specification

Complete specification of the .djc bytecode format

## File Format

### Header
```c
struct DJCHeader {
    uint32_t magic;        // 0x444A4300 ("DJC\0")
    uint16_t version;      // Format version (1)
    uint16_t flags;        // Reserved (0)
};
```

### Constant Pool
```c
struct ConstantPool {
    uint16_t count;        // Number of entries
    ConstantEntry entries[]; // Constant entries
};

struct ConstantEntry {
    uint8_t tag;          // Entry type
    uint8_t data[];       // Entry data
};
```

### Method Table
```c
struct MethodTable {
    uint16_t count;       // Number of methods
    MethodEntry methods[]; // Method entries
};

struct MethodEntry {
    uint16_t name_index;  // Name in constant pool
    uint16_t code_offset; // Offset to bytecode
    uint16_t code_length; // Bytecode length
    uint8_t param_count;  // Parameter count
    uint8_t local_count;  // Local variable count
};
```

## Instruction Set

### Stack Operations
- `ICONST <value>` - Push integer constant
- `ILOAD <index>` - Load integer from local
- `ISTORE <index>` - Store integer to local

### Arithmetic
- `IADD` - Add two integers
- `ISUB` - Subtract two integers
- `IMUL` - Multiply two integers
- `IDIV` - Divide two integers
- `IREM` - Remainder of division

### Comparison
- `IF_ICMPEQ <offset>` - Jump if equal
- `IF_ICMPNE <offset>` - Jump if not equal
- `IF_ICMPLT <offset>` - Jump if less than
- `IF_ICMPGT <offset>` - Jump if greater than
- `IF_ICMPLE <offset>` - Jump if less or equal
- `IF_ICMPGE <offset>` - Jump if greater or equal

### Control Flow
- `GOTO <offset>` - Unconditional jump
- `RETURN` - Return from method
- `IRETURN` - Return integer value

### Method Invocation
- `INVOKESTATIC <index>` - Call static method

## Encoding

### Instruction Format
```
[opcode] [operand1] [operand2] ...
```

### Operand Types
- `uint8_t` - 8-bit unsigned integer
- `int16_t` - 16-bit signed integer
- `uint16_t` - 16-bit unsigned integer

## Example

### Java Source
```java
class Add {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b;
        return;
    }
}
```

### Bytecode
```
ICONST 10      ; Push 10
ISTORE 0       ; Store to local 0 (a)
ICONST 20      ; Push 20
ISTORE 1       ; Store to local 1 (b)
ILOAD 0        ; Load a
ILOAD 1        ; Load b
IADD           ; Add
ISTORE 2       ; Store to local 2 (c)
RETURN         ; Return
```

### Binary Format
```
00 0A          ; ICONST 10
01 00          ; ISTORE 0
00 14          ; ICONST 20
01 01          ; ISTORE 1
02 00          ; ILOAD 0
02 01          ; ILOAD 1
03             ; IADD
01 02          ; ISTORE 2
04             ; RETURN
```
```

## 6. EXAMPLES.md (Example Programs)

### Content Outline

```markdown
# DOS Java Examples

Collection of example programs

## Basic Examples

### 1. Hello World
```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

**Compile**: `djc Hello.java`
**Run**: `djvm Hello.djc`

### 2. Arithmetic
```java
class Arithmetic {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        int diff = a - b;
        int prod = a * b;
        int quot = b / a;
        return;
    }
}
```

## Control Flow Examples

### 3. If Statement
```java
class IfExample {
    public static void main() {
        int x = 10;
        int result = 0;
        
        if (x > 5) {
            result = 100;
        }
        
        return;
    }
}
```

### 4. While Loop
```java
class WhileExample {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        return;
    }
}
```

## Algorithm Examples

### 5. Fibonacci
```java
class Fibonacci {
    public static void main() {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        return;
    }
}
```

### 6. Factorial
```java
class Factorial {
    public static void main() {
        int n = 5;
        int result = 1;
        int i = 1;
        
        while (i <= n) {
            result = result * i;
            i = i + 1;
        }
        
        return;
    }
}
```

### 7. Prime Numbers
```java
class Prime {
    public static void main() {
        int n = 20;
        int i = 2;
        
        while (i <= n) {
            int isPrime = 1;
            int j = 2;
            
            while (j < i) {
                if (i / j * j == i) {
                    isPrime = 0;
                }
                j = j + 1;
            }
            
            i = i + 1;
        }
        
        return;
    }
}
```

## Method Examples

### 8. Simple Method
```java
class MethodExample {
    public static int getValue() {
        return 42;
    }
    
    public static void main() {
        int x = getValue();
        return;
    }
}
```

### 9. Method with Parameters
```java
class ParamExample {
    public static int add(int a, int b) {
        return a + b;
    }
    
    public static void main() {
        int result = add(10, 20);
        return;
    }
}
```

### 10. Recursive Method
```java
class RecursiveExample {
    public static int factorial(int n) {
        if (n <= 1) {
            return 1;
        }
        return n * factorial(n - 1);
    }
    
    public static void main() {
        int result = factorial(5);
        return;
    }
}
```
```

## 7. TROUBLESHOOTING.md

### Content Outline

```markdown
# Troubleshooting Guide

Common issues and solutions

## Compilation Issues

### Error: File not found
**Problem**: Source file doesn't exist
**Solution**: Check file path and name

### Error: Out of memory
**Problem**: Not enough memory to compile
**Solution**: 
- Close other programs
- Simplify source code
- Split into multiple files

### Error: Syntax error
**Problem**: Invalid Java syntax
**Solution**: Check syntax against language spec

## Runtime Issues

### Error: Cannot load .djc file
**Problem**: Invalid bytecode file
**Solution**: Recompile source file

### Error: Stack overflow
**Problem**: Too many nested calls
**Solution**: Reduce recursion depth

### Error: Out of memory
**Problem**: Not enough heap memory
**Solution**: Reduce program complexity

## Performance Issues

### Slow compilation
**Problem**: Large source file
**Solution**: Split into smaller files

### Slow execution
**Problem**: Complex algorithms
**Solution**: Optimize algorithm

## DOS-Specific Issues

### Error: Not enough conventional memory
**Problem**: DOS memory limit
**Solution**: Use memory manager

### Error: Cannot run in Windows
**Problem**: 16-bit program
**Solution**: Use DOSBox

## Getting Help

### Check Documentation
- README.md
- COMPILER_USAGE.md
- LANGUAGE_SPEC.md

### Report Issues
- Include error message
- Provide source code
- Describe environment
```

## Documentation Maintenance

### Update Schedule
- After each release
- When features added
- When bugs fixed
- When questions arise

### Review Process
1. Technical accuracy
2. Clarity and completeness
3. Example validity
4. Link verification

### Version Control
- Track changes in git
- Tag documentation versions
- Maintain changelog

## Success Criteria

- ✓ All documents complete
- ✓ Examples work correctly
- ✓ Clear and concise
- ✓ Technically accurate
- ✓ Easy to navigate
- ✓ Well-organized
- ✓ Up-to-date