# DOS Java Compiler (djc) - Plan Summary

## Executive Summary

PC-DOS上でJavaソースコード(.java)から直接.djcバイトコードファイルを生成するコンパイラ「djc」の開発計画。

### Key Benefits
1. **DOS上で完結**: JDK不要、DOS環境だけで開発可能
2. **シンプルなワークフロー**: 1ステップでコンパイル（現在は2ステップ）
3. **最適化**: .djc形式に直接最適化されたコード生成
4. **教育的価値**: コンパイラの仕組みを学べる

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│              djc Compiler (4-Pass Design)                │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  Java Source (.java)                                     │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 1: Lex  │ → tokens.tmp (Token stream)           │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 2: Parse│ → ast.tmp (AST nodes)                 │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 3: Sem  │ → symbols.tmp (Symbol table)          │
│  └──────────────┘                                        │
│         │                                                 │
│         ▼                                                 │
│  ┌──────────────┐                                        │
│  │ Pass 4: Code │ → output.djc (Bytecode)               │
│  └──────────────┘                                        │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

## Implementation Phases

### Phase 1: Lexer (Week 1) - HIGHEST PRIORITY
**Goal**: Javaソースコードをトークン列に変換

**Deliverables**:
- `tools/compiler/lexer.h` - Token definitions
- `tools/compiler/lexer.c` - Lexer implementation
- `tools/compiler/test_lexer.c` - Test program

**Key Features**:
- Keyword recognition (class, public, static, void, int, boolean, if, else, while, for, return, new, this)
- Operator tokenization (+, -, *, /, %, ==, !=, <, <=, >, >=, &&, ||, !)
- Literal parsing (integers, strings, true, false)
- Identifier recognition
- Comment handling (// and /* */)

**Memory Strategy**:
- 512-byte input buffer
- 2KB string pool for identifiers
- Write tokens to temp file immediately

### Phase 2: Parser (Week 2) - HIGH PRIORITY
**Goal**: トークン列からAST（抽象構文木）を構築

**Deliverables**:
- `tools/compiler/ast.h` - AST node definitions
- `tools/compiler/parser.h` - Parser interface
- `tools/compiler/parser.c` - Recursive descent parser
- `tools/compiler/test_parser.c` - Test program

**Key Features**:
- Class declaration parsing
- Method declaration parsing
- Statement parsing (if/else, while, for, return, variable declaration)
- Expression parsing with operator precedence
- Syntax error detection and recovery

**Memory Strategy**:
- 4KB AST node buffer (128 nodes)
- Write nodes to temp file in batches
- Minimal parser state in memory

### Phase 3: Semantic Analyzer (Week 3) - MEDIUM PRIORITY
**Goal**: 型チェックとシンボル解決

**Deliverables**:
- `tools/compiler/symtable.h` - Symbol table definitions
- `tools/compiler/symtable.c` - Symbol table implementation
- `tools/compiler/semantic.h` - Semantic analyzer interface
- `tools/compiler/semantic.c` - Type checker and resolver
- `tools/compiler/test_semantic.c` - Test program

**Key Features**:
- Symbol table construction (classes, methods, fields, locals)
- Type checking (int, boolean, void, class types)
- Identifier resolution
- Method signature validation
- Control flow validation (return statements)

**Memory Strategy**:
- 8KB symbol table (256 symbols)
- Two-pass approach: collect declarations, then type check
- Write symbol table to temp file

### Phase 4: Code Generator (Week 4) - MEDIUM PRIORITY
**Goal**: ASTから.djcバイトコードを生成

**Deliverables**:
- `tools/compiler/codegen.h` - Code generator interface
- `tools/compiler/codegen.c` - Bytecode generator
- `tools/compiler/test_codegen.c` - Test program

**Key Features**:
- Expression code generation (stack-based)
- Statement code generation (if/while/for with jumps)
- Method code generation
- Constant pool construction
- .djc file writing

**Memory Strategy**:
- 4KB bytecode buffer
- 2KB constant pool
- Backpatching for forward jumps

### Phase 5: Integration (Week 5) - LOW PRIORITY
**Goal**: 全フェーズの統合とCLI

**Deliverables**:
- `tools/compiler/djc.c` - Main compiler driver
- Integration tests
- Documentation
- Updated Makefile

**Key Features**:
- Command-line interface
- Error message formatting
- Verbose and debug modes
- Temporary file management
- End-to-end testing

## Minimal Viable Product (MVP)

### Supported Java Subset
```java
// MVP Example
class Calculator {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        
        if (sum > 25) {
            System.out.println(sum);
        }
        
        int i = 0;
        while (i < 5) {
            System.out.println(i);
            i = i + 1;
        }
    }
}
```

**Supported Features**:
- ✅ Single class per file
- ✅ Static methods only
- ✅ Primitive types: int, boolean
- ✅ Control flow: if/else, while
- ✅ Operators: arithmetic, comparison, logical
- ✅ Local variables
- ✅ System.out.println(int)

**NOT Supported in MVP**:
- ❌ Objects and instance methods
- ❌ For loops (use while)
- ❌ Arrays
- ❌ Strings (except println)
- ❌ Multiple classes
- ❌ Inheritance

## Memory Budget (64KB Data Segment)

```
Component               Size        Percentage
-------------------------------------------------
Lexer buffer           512 bytes   0.8%
Token buffer           2 KB        3.1%
String pool            2 KB        3.1%
Parser stack           1 KB        1.6%
AST node buffer        4 KB        6.3%
Symbol table           8 KB        12.5%
Code buffer            4 KB        6.3%
Constant pool          2 KB        3.1%
Temp file buffers      2 KB        3.1%
Stack/heap overhead    8 KB        12.5%
-------------------------------------------------
Total Used             ~33 KB      51.6%
Available              ~31 KB      48.4%
```

## File Structure

```
dosjava/
├── tools/
│   ├── compiler/
│   │   ├── djc.c              # Main compiler driver
│   │   ├── lexer.h            # Token definitions
│   │   ├── lexer.c            # Lexical analyzer
│   │   ├── ast.h              # AST node definitions
│   │   ├── parser.h           # Parser interface
│   │   ├── parser.c           # Syntax analyzer
│   │   ├── symtable.h         # Symbol table definitions
│   │   ├── symtable.c         # Symbol table implementation
│   │   ├── semantic.h         # Semantic analyzer interface
│   │   ├── semantic.c         # Type checker
│   │   ├── codegen.h          # Code generator interface
│   │   ├── codegen.c          # Bytecode generator
│   │   ├── test_lexer.c       # Lexer test program
│   │   ├── test_parser.c      # Parser test program
│   │   ├── test_semantic.c    # Semantic test program
│   │   └── test_codegen.c     # Codegen test program
│   ├── classfile.c            # (existing)
│   ├── classfile.h
│   ├── java2djc.c             # (existing)
│   └── mkdjc.c                # (existing)
├── tests/
│   ├── lexer/                 # Lexer test cases
│   ├── parser/                # Parser test cases
│   ├── semantic/              # Semantic test cases
│   ├── codegen/               # Codegen test cases
│   └── e2e/                   # End-to-end test cases
└── docs/
    ├── COMPILER_DESIGN.md     # Detailed design
    ├── COMPILER_ROADMAP.md    # Implementation roadmap
    ├── COMPILER_QUICKSTART.md # Quick start guide
    └── COMPILER_PLAN_SUMMARY.md # This file
```

## Timeline

| Week | Phase | Focus | Deliverables |
|------|-------|-------|--------------|
| 1 | Lexer | Tokenization | lexer.h, lexer.c, test_lexer.c |
| 2 | Parser | AST building | ast.h, parser.h, parser.c, test_parser.c |
| 3 | Semantic | Type checking | symtable.h/c, semantic.h/c, test_semantic.c |
| 4 | CodeGen | Bytecode generation | codegen.h, codegen.c, test_codegen.c |
| 5 | Integration | CLI & testing | djc.c, integration tests, docs |

**Total**: 5 weeks for MVP

## Success Criteria

### MVP Success
- [ ] Compile HelloWorld.java to HelloWorld.djc
- [ ] Run HelloWorld.djc on DOS Java VM
- [ ] Output: "Hello, DOS!" or similar
- [ ] Compile Calculator.java (arithmetic operations)
- [ ] Compile Counter.java (while loop)
- [ ] All MVP test cases pass

### Extended Success (Post-MVP)
- [ ] Support for loops
- [ ] Support objects (instance methods/fields)
- [ ] Support arrays
- [ ] Support strings
- [ ] Better error messages
- [ ] Optimization passes

## Risk Mitigation

### Risk 1: Memory Constraints
**Risk**: 64KB data segment too small for complex programs
**Mitigation**: 
- Multi-pass compilation with temp files
- Process in small chunks
- Optimize data structures
- Test with memory profiling

### Risk 2: Complexity
**Risk**: Full Java parser too complex for 5-week timeline
**Mitigation**:
- Focus on MVP subset only
- Defer advanced features
- Incremental implementation
- Extensive testing at each phase

### Risk 3: Testing
**Risk**: Insufficient test coverage leads to bugs
**Mitigation**:
- Unit tests for each phase
- Integration tests for end-to-end
- Regression tests for bug fixes
- Test-driven development approach

### Risk 4: DOS Environment
**Risk**: Limited debugging tools on DOS
**Mitigation**:
- Develop on modern OS with cross-compiler
- Test on DOS emulator (DOSBox)
- Verbose logging and debug modes
- Keep temp files for inspection

## Development Workflow

### Daily Workflow
```bash
# 1. Edit source files
vim tools/compiler/lexer.c

# 2. Compile
wmake lexer

# 3. Test
build\bin\test_lexer.exe tests\lexer\hello.java

# 4. Debug if needed
build\bin\test_lexer.exe -v tests\lexer\hello.java

# 5. Commit changes
git add tools/compiler/lexer.c
git commit -m "Implement keyword recognition"
```

### Testing Workflow
```bash
# Unit tests
wmake test_lexer
wmake test_parser
wmake test_semantic
wmake test_codegen

# Integration tests
wmake test_compiler

# Manual testing
djc.exe tests\e2e\HelloWorld.java
dosjava.exe HelloWorld.djc
```

## Next Steps

### Immediate Actions
1. **Create directory structure**: `mkdir tools/compiler`
2. **Create initial files**: lexer.h, lexer.c, test_lexer.c
3. **Update Makefile**: Add compiler targets
4. **Start Phase 1**: Implement lexer

### Week 1 Goals
- [ ] Define token types in lexer.h
- [ ] Implement character reading with buffering
- [ ] Implement keyword recognition
- [ ] Implement operator/delimiter tokenization
- [ ] Implement identifier/literal parsing
- [ ] Write tokens to temp file
- [ ] Create test_lexer.c
- [ ] Test with sample Java files

### Week 2 Goals
- [ ] Define AST node types in ast.h
- [ ] Implement recursive descent parser
- [ ] Parse class declarations
- [ ] Parse method declarations
- [ ] Parse statements
- [ ] Parse expressions
- [ ] Write AST to temp file
- [ ] Create test_parser.c
- [ ] Test with sample Java files

## Resources

### Documentation
- [`COMPILER_DESIGN.md`](COMPILER_DESIGN.md) - Detailed technical design
- [`COMPILER_ROADMAP.md`](COMPILER_ROADMAP.md) - Phase-by-phase implementation guide
- [`COMPILER_QUICKSTART.md`](COMPILER_QUICKSTART.md) - Quick start guide
- [`TECHNICAL_SPEC.md`](TECHNICAL_SPEC.md) - Overall technical specifications

### References
- Java Language Specification (JLS) - Subset for reference
- Compilers: Principles, Techniques, and Tools (Dragon Book)
- Open Watcom v2 C Compiler Documentation
- DOS Programming Guide

### Test Cases
- `tests/lexer/` - Lexer test cases
- `tests/parser/` - Parser test cases
- `tests/semantic/` - Semantic test cases
- `tests/codegen/` - Code generation test cases
- `tests/e2e/` - End-to-end test cases

## Conclusion

This plan provides a comprehensive roadmap for implementing a DOS-native Java compiler that generates .djc bytecode files. The multi-pass design with temporary files addresses the 64KB memory constraint, while the phased approach ensures steady progress toward a working MVP.

### Key Takeaways
1. **Feasible**: 5-week timeline for MVP is realistic
2. **Incremental**: Each phase builds on previous work
3. **Testable**: Unit and integration tests at each phase
4. **Practical**: Focus on essential features first
5. **Educational**: Learn compiler construction principles

### Success Factors
- ✅ Clear architecture and design
- ✅ Well-defined phases and deliverables
- ✅ Realistic scope (MVP subset)
- ✅ Memory-efficient implementation
- ✅ Comprehensive testing strategy
- ✅ Risk mitigation plans

---

**Document Version**: 1.0
**Last Updated**: 2026-04-19
**Status**: Planning Complete - Ready for Implementation
**Next Action**: Create `tools/compiler/` directory and start Phase 1 (Lexer)