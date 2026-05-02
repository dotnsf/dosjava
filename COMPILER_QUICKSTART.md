# DOS Java Compiler (djc) - Quick Start Guide

## Overview

PC-DOS上でJavaソースコードから直接.djcバイトコードファイルを生成するコンパイラの開発クイックスタートガイド。

## Why Direct Compilation?

### Current Workflow (2-step)
```
Java Source (.java)
    ↓ javac (requires JDK)
Java Bytecode (.class)
    ↓ java2djc (DOS tool)
DOS Java Bytecode (.djc)
    ↓ dosjava (DOS VM)
Output
```

### New Workflow (1-step)
```
Java Source (.java)
    ↓ djc (DOS tool)
DOS Java Bytecode (.djc)
    ↓ dosjava (DOS VM)
Output
```

### Benefits
1. **DOS上で完結**: JDKが不要、DOS環境だけで開発可能
2. **シンプル**: 1ステップでコンパイル
3. **最適化**: .djc形式に直接最適化されたコード生成
4. **教育的**: コンパイラの仕組みを学べる

## Architecture Summary

```
┌─────────────────────────────────────────────────────────┐
│                  djc Compiler (DOS)                      │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  Java Source (.java)                                     │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 1: Lex  │ → tokens.tmp                          │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 2: Parse│ → ast.tmp                             │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 3: Sem  │ → symbols.tmp                         │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 4: Code │ → output.djc                          │
│  └──────────────┘                                        │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

## Implementation Priority

### Phase 1: Lexer (HIGHEST PRIORITY)
**Why first**: Foundation for all other phases
**Complexity**: Low
**Time**: 1 week
**Dependencies**: None

**Key files**:
- `tools/compiler/lexer.h` - Token definitions
- `tools/compiler/lexer.c` - Lexer implementation
- `tools/compiler/test_lexer.c` - Test program

**Success criteria**:
```bash
# Compile lexer
wmake lexer

# Test lexer
test_lexer.exe tests/lexer/hello.java
# Should output: tokens.tmp with correct tokens
```

### Phase 2: Parser (HIGH PRIORITY)
**Why second**: Builds on lexer, needed for semantic analysis
**Complexity**: Medium
**Time**: 1-2 weeks
**Dependencies**: Lexer

**Key files**:
- `tools/compiler/ast.h` - AST node definitions
- `tools/compiler/parser.h` - Parser interface
- `tools/compiler/parser.c` - Parser implementation
- `tools/compiler/test_parser.c` - Test program

**Success criteria**:
```bash
# Compile parser
wmake parser

# Test parser
test_parser.exe tokens.tmp
# Should output: ast.tmp with correct AST structure
```

### Phase 3: Semantic Analyzer (MEDIUM PRIORITY)
**Why third**: Validates correctness before code generation
**Complexity**: Medium-High
**Time**: 1-2 weeks
**Dependencies**: Parser

**Key files**:
- `tools/compiler/symtable.h` - Symbol table definitions
- `tools/compiler/symtable.c` - Symbol table implementation
- `tools/compiler/semantic.h` - Semantic analyzer interface
- `tools/compiler/semantic.c` - Semantic analyzer implementation
- `tools/compiler/test_semantic.c` - Test program

**Success criteria**:
```bash
# Compile semantic analyzer
wmake semantic

# Test semantic analyzer
test_semantic.exe ast.tmp
# Should output: symbols.tmp and detect semantic errors
```

### Phase 4: Code Generator (MEDIUM PRIORITY)
**Why fourth**: Final step, generates executable code
**Complexity**: Medium-High
**Time**: 1-2 weeks
**Dependencies**: Semantic analyzer

**Key files**:
- `tools/compiler/codegen.h` - Code generator interface
- `tools/compiler/codegen.c` - Code generator implementation
- `tools/compiler/test_codegen.c` - Test program

**Success criteria**:
```bash
# Compile code generator
wmake codegen

# Test code generator
test_codegen.exe ast.tmp symbols.tmp output.djc
# Should output: valid .djc file

# Run generated code
dosjava output.djc
# Should execute correctly
```

### Phase 5: Integration (LOW PRIORITY)
**Why last**: Combines all phases
**Complexity**: Low
**Time**: 1 week
**Dependencies**: All previous phases

**Key files**:
- `tools/compiler/djc.c` - Main compiler driver

**Success criteria**:
```bash
# Compile full compiler
wmake djc

# Test end-to-end
djc.exe HelloWorld.java
dosjava HelloWorld.djc
# Should output: "Hello, DOS!"
```

## Minimal Viable Product (MVP)

### MVP Scope
Focus on the absolute minimum to get a working compiler:

**Supported Java Features**:
- Single class per file
- Static methods only (no objects initially)
- Primitive types: `int`, `boolean`
- Control flow: `if/else`, `while`
- Operators: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `<=`, `>`, `>=`
- System.out.println() for output

**NOT in MVP**:
- Objects and classes (instance methods/fields)
- For loops (use while instead)
- Arrays
- Strings (except println)
- Multiple classes
- Inheritance

### MVP Test Program

```java
// HelloWorld.java - MVP Test 1
class HelloWorld {
    public static void main() {
        System.out.println(42);
    }
}
```

```java
// Calculator.java - MVP Test 2
class Calculator {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        System.out.println(sum);
    }
}
```

```java
// Counter.java - MVP Test 3
class Counter {
    public static void main() {
        int i = 1;
        while (i <= 5) {
            System.out.println(i);
            i = i + 1;
        }
    }
}
```

```java
// Fibonacci.java - MVP Test 4
class Fibonacci {
    public static void main() {
        int a = 0;
        int b = 1;
        int count = 0;
        
        while (count < 10) {
            System.out.println(a);
            int temp = a + b;
            a = b;
            b = temp;
            count = count + 1;
        }
    }
}
```

## Development Workflow

### Step 1: Setup Development Environment

```bash
# Create compiler directory
cd dosjava/tools
mkdir compiler
cd compiler

# Create initial files
touch lexer.h lexer.c
touch parser.h parser.c ast.h
touch semantic.h semantic.c symtable.h symtable.c
touch codegen.h codegen.c
touch djc.c
```

### Step 2: Implement Lexer

```bash
# Edit lexer.h - Define token types
# Edit lexer.c - Implement tokenization
# Edit test_lexer.c - Create test program

# Compile
cd ../..
wmake test_lexer

# Test
build\bin\test_lexer.exe tests\lexer\hello.java

# Verify tokens.tmp is created correctly
```

### Step 3: Implement Parser

```bash
# Edit ast.h - Define AST node types
# Edit parser.h - Define parser interface
# Edit parser.c - Implement recursive descent parser
# Edit test_parser.c - Create test program

# Compile
wmake test_parser

# Test
build\bin\test_parser.exe tokens.tmp

# Verify ast.tmp is created correctly
```

### Step 4: Implement Semantic Analyzer

```bash
# Edit symtable.h - Define symbol table
# Edit symtable.c - Implement symbol table
# Edit semantic.h - Define semantic analyzer interface
# Edit semantic.c - Implement type checking
# Edit test_semantic.c - Create test program

# Compile
wmake test_semantic

# Test
build\bin\test_semantic.exe ast.tmp

# Verify symbols.tmp is created correctly
```

### Step 5: Implement Code Generator

```bash
# Edit codegen.h - Define code generator interface
# Edit codegen.c - Implement bytecode generation
# Edit test_codegen.c - Create test program

# Compile
wmake test_codegen

# Test
build\bin\test_codegen.exe ast.tmp symbols.tmp output.djc

# Verify output.djc is valid
build\bin\dosjava.exe output.djc
```

### Step 6: Integrate All Phases

```bash
# Edit djc.c - Main compiler driver

# Compile
wmake djc

# Test end-to-end
build\bin\djc.exe tests\e2e\HelloWorld.java
build\bin\dosjava.exe HelloWorld.djc
```

## Testing Strategy

### Unit Testing
Each phase has its own test program:

```bash
# Test lexer
test_lexer.exe input.java
# Outputs: tokens.tmp

# Test parser
test_parser.exe tokens.tmp
# Outputs: ast.tmp

# Test semantic
test_semantic.exe ast.tmp
# Outputs: symbols.tmp

# Test codegen
test_codegen.exe ast.tmp symbols.tmp output.djc
# Outputs: output.djc
```

### Integration Testing
Full compilation pipeline:

```bash
# Compile and run
djc.exe HelloWorld.java
dosjava.exe HelloWorld.djc

# Expected output
42
```

### Regression Testing
Maintain test suite:

```bash
# Run all tests
wmake test_compiler

# Individual test
djc.exe tests\regression\bug001.java
dosjava.exe bug001.djc
```

## Memory Management Strategy

### Problem
16-bit DOS Small memory model: 64KB data segment limit

### Solution
Multi-pass compilation with temporary files:

```
Pass 1: Source → tokens.tmp (512 byte buffer)
Pass 2: tokens.tmp → ast.tmp (4KB node buffer)
Pass 3: ast.tmp → symbols.tmp (8KB symbol table)
Pass 4: ast.tmp + symbols.tmp → output.djc (4KB code buffer)
```

### Memory Budget

```
Component               Size        Notes
-------------------------------------------------
Lexer buffer           512 bytes   Source input
Token buffer           2 KB        64 tokens
String pool            2 KB        Identifiers
Parser stack           1 KB        Recursion depth
AST node buffer        4 KB        128 nodes
Symbol table           8 KB        256 symbols
Code buffer            4 KB        Bytecode output
Constant pool          2 KB        64 constants
Temp file buffers      2 KB        I/O buffering
Stack/heap overhead    8 KB        Runtime
-------------------------------------------------
Total                  ~33 KB      Leaves ~31KB free
```

## Common Pitfalls and Solutions

### Pitfall 1: Running Out of Memory
**Problem**: 64KB data segment fills up
**Solution**: 
- Use temporary files for intermediate data
- Process in small chunks
- Flush buffers frequently

### Pitfall 2: Complex Grammar
**Problem**: Java grammar is complex
**Solution**:
- Start with minimal subset
- Add features incrementally
- Focus on MVP first

### Pitfall 3: Error Recovery
**Problem**: Parser stops at first error
**Solution**:
- Implement synchronization points
- Continue parsing after errors
- Collect multiple errors

### Pitfall 4: Type Checking
**Problem**: Complex type system
**Solution**:
- Start with primitives only (int, boolean)
- Add objects later
- Keep type rules simple

## Debugging Tips

### Enable Verbose Mode
```bash
djc.exe -v HelloWorld.java
```

Output:
```
Phase 1: Lexical analysis...
  Tokens: 15
  Time: 0.1s
Phase 2: Parsing...
  AST nodes: 8
  Time: 0.2s
Phase 3: Semantic analysis...
  Symbols: 3
  Time: 0.1s
Phase 4: Code generation...
  Bytecode size: 42 bytes
  Time: 0.1s
Total time: 0.5s
```

### Keep Temporary Files
```bash
djc.exe -k HelloWorld.java
```

Files created:
- `tokens.tmp` - Token stream
- `ast.tmp` - AST nodes
- `symbols.tmp` - Symbol table
- `HelloWorld.djc` - Output

### Debug Mode
```bash
djc.exe -d HelloWorld.java
```

Shows detailed information for each phase.

## Next Steps

### After MVP
1. **Add For Loops**: Extend control flow
2. **Add Objects**: Instance methods and fields
3. **Add Arrays**: Basic array support
4. **Add Strings**: String literals and operations
5. **Optimize**: Constant folding, dead code elimination

### Long Term
1. **Inheritance**: Class hierarchies
2. **Interfaces**: Abstract types
3. **Exceptions**: Try/catch (if feasible)
4. **Standard Library**: Expand java.lang package
5. **IDE Support**: Language server protocol

## Resources

### Documentation
- `COMPILER_DESIGN.md` - Detailed design document
- `COMPILER_ROADMAP.md` - Implementation roadmap
- `TECHNICAL_SPEC.md` - Technical specifications
- `README.md` - Project overview

### Code Examples
- `tests/lexer/` - Lexer test cases
- `tests/parser/` - Parser test cases
- `tests/semantic/` - Semantic test cases
- `tests/codegen/` - Code generation test cases
- `tests/e2e/` - End-to-end test cases

### References
- Java Language Specification (subset)
- Compiler Construction (Dragon Book)
- Open Watcom C Compiler Documentation
- DOS Programming Guide

## FAQ

### Q: Why not use an existing Java compiler?
**A**: Existing compilers (javac) require JDK and generate standard .class files. We want a DOS-native tool that generates optimized .djc files directly.

### Q: Can I use modern Java features?
**A**: No, only a minimal Java subset is supported. See MVP scope above.

### Q: How long will implementation take?
**A**: MVP: ~5 weeks. Full featured: ~3-6 months.

### Q: Can I contribute?
**A**: Yes! Start with MVP implementation, then add features incrementally.

### Q: What if I run out of memory?
**A**: Use temporary files, process in chunks, optimize data structures.

### Q: How do I debug compiler issues?
**A**: Use verbose mode (-v), keep temp files (-k), and debug mode (-d).

---

**Document Version**: 1.0
**Last Updated**: 2026-04-19
**Status**: Planning Phase
**Next Action**: Start Phase 1 (Lexer Implementation)