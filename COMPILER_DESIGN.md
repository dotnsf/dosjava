# DOS Java Compiler (djc) - Design Document

## Overview

PC-DOS上でJavaソースコード(.java)から直接.djcバイトコードファイルを生成するコンパイラの設計書。

## Goals

1. **DOS上で動作**: 16-bit PC-DOS環境で実行可能
2. **メモリ効率**: Small memory model (64KB制約)内で動作
3. **シンプルな実装**: 複雑な最適化は避け、基本機能に集中
4. **段階的コンパイル**: メモリ制約のため、複数パスで処理

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    djc Compiler Pipeline                     │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  Java Source (.java)                                         │
│         │                                                     │
│         ▼                                                     │
│  ┌──────────────────┐                                        │
│  │  Lexer (Pass 1)  │  Tokenize source code                 │
│  │  - Tokenization  │  Output: Token stream to temp file    │
│  └──────────────────┘                                        │
│         │                                                     │
│         ▼                                                     │
│  ┌──────────────────┐                                        │
│  │  Parser (Pass 2) │  Build AST (simplified)               │
│  │  - Syntax check  │  Output: AST nodes to temp file       │
│  │  - AST building  │                                        │
│  └──────────────────┘                                        │
│         │                                                     │
│         ▼                                                     │
│  ┌──────────────────┐                                        │
│  │ Semantic (Pass 3)│  Type checking & symbol resolution    │
│  │  - Type check    │  Output: Symbol table + annotated AST │
│  │  - Symbol table  │                                        │
│  └──────────────────┘                                        │
│         │                                                     │
│         ▼                                                     │
│  ┌──────────────────┐                                        │
│  │ CodeGen (Pass 4) │  Generate .djc bytecode               │
│  │  - Bytecode emit │  Output: .djc file                    │
│  │  - Optimization  │                                        │
│  └──────────────────┘                                        │
│         │                                                     │
│         ▼                                                     │
│  .djc Bytecode File                                          │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Memory Strategy

### Problem
- Small memory model: 64KB data segment limit
- Large Java files cannot fit entirely in memory
- AST structures consume significant memory

### Solution: Multi-Pass with Temporary Files

1. **Pass 1 (Lexer)**: Read source, write tokens to temp file
2. **Pass 2 (Parser)**: Read tokens, write AST nodes to temp file
3. **Pass 3 (Semantic)**: Read AST, build symbol table, write annotated AST
4. **Pass 4 (CodeGen)**: Read annotated AST, generate .djc file

### Temporary File Format

```
tokens.tmp    - Token stream (binary format)
ast.tmp       - AST nodes (binary format)
symbols.tmp   - Symbol table (binary format)
```

## Component Design

### 1. Lexer (Tokenizer)

**Input**: Java source file (.java)
**Output**: Token stream (tokens.tmp)

**Supported Tokens**:
```c
typedef enum {
    TOK_EOF = 0,
    
    /* Keywords */
    TOK_CLASS,      /* class */
    TOK_PUBLIC,     /* public */
    TOK_STATIC,     /* static */
    TOK_VOID,       /* void */
    TOK_INT,        /* int */
    TOK_BOOLEAN,    /* boolean */
    TOK_IF,         /* if */
    TOK_ELSE,       /* else */
    TOK_WHILE,      /* while */
    TOK_FOR,        /* for */
    TOK_RETURN,     /* return */
    TOK_NEW,        /* new */
    TOK_THIS,       /* this */
    
    /* Literals */
    TOK_IDENTIFIER, /* variable/method/class names */
    TOK_INTEGER,    /* integer literal */
    TOK_STRING,     /* string literal */
    TOK_TRUE,       /* true */
    TOK_FALSE,      /* false */
    
    /* Operators */
    TOK_PLUS,       /* + */
    TOK_MINUS,      /* - */
    TOK_STAR,       /* * */
    TOK_SLASH,      /* / */
    TOK_PERCENT,    /* % */
    TOK_ASSIGN,     /* = */
    TOK_EQ,         /* == */
    TOK_NE,         /* != */
    TOK_LT,         /* < */
    TOK_LE,         /* <= */
    TOK_GT,         /* > */
    TOK_GE,         /* >= */
    TOK_AND,        /* && */
    TOK_OR,         /* || */
    TOK_NOT,        /* ! */
    
    /* Delimiters */
    TOK_LPAREN,     /* ( */
    TOK_RPAREN,     /* ) */
    TOK_LBRACE,     /* { */
    TOK_RBRACE,     /* } */
    TOK_LBRACKET,   /* [ */
    TOK_RBRACKET,   /* ] */
    TOK_SEMICOLON,  /* ; */
    TOK_COMMA,      /* , */
    TOK_DOT         /* . */
} TokenType;

typedef struct {
    TokenType type;
    uint16_t line;
    uint16_t column;
    union {
        int16_t int_value;
        uint16_t str_offset;  /* Offset in string pool */
    } value;
} Token;
```

**Implementation Strategy**:
- Read source file in 512-byte chunks
- Process character by character
- Write tokens to temp file immediately
- Maintain small string pool (max 2KB) for identifiers/strings

### 2. Parser (AST Builder)

**Input**: Token stream (tokens.tmp)
**Output**: AST nodes (ast.tmp)

**Simplified AST Node Types**:
```c
typedef enum {
    NODE_CLASS,         /* Class declaration */
    NODE_METHOD,        /* Method declaration */
    NODE_FIELD,         /* Field declaration */
    NODE_BLOCK,         /* Statement block */
    NODE_IF,            /* If statement */
    NODE_WHILE,         /* While loop */
    NODE_FOR,           /* For loop */
    NODE_RETURN,        /* Return statement */
    NODE_EXPR_STMT,     /* Expression statement */
    NODE_VAR_DECL,      /* Variable declaration */
    NODE_ASSIGN,        /* Assignment */
    NODE_BINARY_OP,     /* Binary operation */
    NODE_UNARY_OP,      /* Unary operation */
    NODE_CALL,          /* Method call */
    NODE_NEW,           /* Object creation */
    NODE_FIELD_ACCESS,  /* Field access */
    NODE_ARRAY_ACCESS,  /* Array access */
    NODE_LITERAL,       /* Literal value */
    NODE_IDENTIFIER     /* Variable/field reference */
} NodeType;

typedef struct {
    NodeType type;
    uint16_t line;
    uint16_t data_offset;   /* Offset to node-specific data */
    uint16_t child_count;
    uint16_t first_child;   /* Index of first child node */
} ASTNode;
```

**Grammar (Simplified Java Subset)**:
```
Program      → ClassDecl
ClassDecl    → 'class' ID '{' MemberDecl* '}'
MemberDecl   → FieldDecl | MethodDecl
FieldDecl    → Type ID ';'
MethodDecl   → 'public' 'static'? Type ID '(' ParamList? ')' Block
ParamList    → Param (',' Param)*
Param        → Type ID
Block        → '{' Statement* '}'
Statement    → VarDecl | IfStmt | WhileStmt | ForStmt | ReturnStmt | ExprStmt
VarDecl      → Type ID ('=' Expr)? ';'
IfStmt       → 'if' '(' Expr ')' Statement ('else' Statement)?
WhileStmt    → 'while' '(' Expr ')' Statement
ForStmt      → 'for' '(' VarDecl Expr ';' Expr ')' Statement
ReturnStmt   → 'return' Expr? ';'
ExprStmt     → Expr ';'
Expr         → Assignment
Assignment   → LogicalOr ('=' Assignment)?
LogicalOr    → LogicalAnd ('||' LogicalAnd)*
LogicalAnd   → Equality ('&&' Equality)*
Equality     → Relational (('==' | '!=') Relational)*
Relational   → Additive (('<' | '<=' | '>' | '>=') Additive)*
Additive     → Multiplicative (('+' | '-') Multiplicative)*
Multiplicative → Unary (('*' | '/' | '%') Unary)*
Unary        → ('!' | '-')? Postfix
Postfix      → Primary ('.' ID | '[' Expr ']' | '(' ArgList? ')')*
Primary      → INTEGER | STRING | 'true' | 'false' | ID | 'this' | 'new' ID '(' ')' | '(' Expr ')'
ArgList      → Expr (',' Expr)*
Type         → 'int' | 'boolean' | 'void' | ID
```

**Implementation Strategy**:
- Recursive descent parser
- Read tokens from temp file in batches (64 tokens at a time)
- Write AST nodes to temp file immediately after parsing each construct
- Keep minimal parser state in memory

### 3. Semantic Analyzer

**Input**: AST nodes (ast.tmp)
**Output**: Symbol table (symbols.tmp) + annotated AST

**Symbol Table Structure**:
```c
typedef enum {
    SYM_CLASS,
    SYM_METHOD,
    SYM_FIELD,
    SYM_LOCAL,
    SYM_PARAM
} SymbolKind;

typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_BOOLEAN,
    TYPE_CLASS
} TypeKind;

typedef struct {
    SymbolKind kind;
    uint16_t name_offset;   /* Offset in string pool */
    TypeKind type;
    uint16_t type_class;    /* For TYPE_CLASS */
    uint16_t scope_level;
    union {
        struct {
            uint16_t local_index;
        } local;
        struct {
            uint16_t field_index;
        } field;
        struct {
            uint16_t param_count;
            uint16_t code_offset;
        } method;
    } data;
} Symbol;
```

**Tasks**:
1. Build symbol table (classes, methods, fields, locals)
2. Type checking
3. Resolve identifiers to symbols
4. Check method signatures
5. Validate control flow (return statements)

**Implementation Strategy**:
- Two-pass approach:
  - Pass 3a: Collect all class/method/field declarations
  - Pass 3b: Type check and resolve references
- Write symbol table to temp file
- Annotate AST nodes with symbol references

### 4. Code Generator

**Input**: Annotated AST (ast.tmp) + Symbol table (symbols.tmp)
**Output**: .djc bytecode file

**Code Generation Strategy**:
```c
/* Method code generation context */
typedef struct {
    uint16_t max_stack;      /* Maximum stack depth */
    uint16_t max_locals;     /* Number of local variables */
    uint16_t current_stack;  /* Current stack depth */
    ByteBuffer* code;        /* Generated bytecode */
} CodeGenContext;
```

**Expression Code Generation**:
- Use stack-based evaluation
- Track stack depth for max_stack calculation
- Generate optimized opcodes (e.g., LOAD_0 instead of LOAD_LOCAL 0)

**Statement Code Generation**:
- If/While/For: Generate conditional jumps with backpatching
- Method calls: Resolve method references to constant pool indices
- Field access: Resolve field references

**Implementation Strategy**:
- Read AST nodes in order
- Generate bytecode for each node
- Write .djc file sections:
  1. Header
  2. Constant pool
  3. Method table
  4. Field table
  5. Bytecode

## File Structure

```
dosjava/
├── tools/
│   ├── compiler/
│   │   ├── djc.c           # Main compiler driver
│   │   ├── lexer.c         # Lexical analyzer
│   │   ├── lexer.h
│   │   ├── parser.c        # Syntax analyzer
│   │   ├── parser.h
│   │   ├── semantic.c      # Semantic analyzer
│   │   ├── semantic.h
│   │   ├── codegen.c       # Code generator
│   │   ├── codegen.h
│   │   ├── symtable.c      # Symbol table
│   │   ├── symtable.h
│   │   └── ast.h           # AST definitions
│   ├── classfile.c         # (existing)
│   ├── classfile.h
│   ├── java2djc.c          # (existing)
│   └── mkdjc.c             # (existing)
```

## Build Integration

Update Makefile to add djc compiler:

```makefile
# Compiler sources
COMPILER_SRCS = tools/compiler/djc.c \
                tools/compiler/lexer.c \
                tools/compiler/parser.c \
                tools/compiler/semantic.c \
                tools/compiler/codegen.c \
                tools/compiler/symtable.c

COMPILER_OBJS = $(OBJ_DIR)/djc.obj \
                $(OBJ_DIR)/lexer.obj \
                $(OBJ_DIR)/parser.obj \
                $(OBJ_DIR)/semantic.obj \
                $(OBJ_DIR)/codegen.obj \
                $(OBJ_DIR)/symtable.obj

# djc compiler target
djc: $(BIN_DIR)/djc.exe

$(BIN_DIR)/djc.exe: $(COMPILER_OBJS) $(FORMAT_OBJS)
	$(LD) $(LDFLAGS) name $@ file { $(COMPILER_OBJS) $(FORMAT_OBJS) }
```

## Usage

```bash
# Compile Java source to .djc
djc HelloWorld.java

# With output file specification
djc HelloWorld.java -o hello.djc

# Verbose mode
djc HelloWorld.java -v

# Keep temporary files for debugging
djc HelloWorld.java -k
```

## Limitations

### Supported Java Features
- Single class per file
- Static and instance methods
- Instance fields (no static fields initially)
- Local variables
- Primitive types: int, boolean
- Control flow: if/else, while, for
- Operators: arithmetic, comparison, logical
- Method calls (static and virtual)
- Object creation (new)
- Field access

### NOT Supported
- Multiple classes per file
- Inheritance (initially)
- Interfaces
- Arrays (initially)
- String literals (limited support)
- Exception handling
- Generics
- Annotations
- Inner classes
- Static initializers
- Constructors with parameters (initially)

## Implementation Phases

### Phase 1: Lexer (Week 1)
- [x] Design token structure
- [ ] Implement character reading with buffering
- [ ] Implement keyword recognition
- [ ] Implement operator/delimiter recognition
- [ ] Implement identifier/literal parsing
- [ ] Write tokens to temp file
- [ ] Test with sample Java files

### Phase 2: Parser (Week 2)
- [x] Design AST node structure
- [ ] Implement recursive descent parser
- [ ] Parse class declarations
- [ ] Parse method declarations
- [ ] Parse statements (if/while/for/return)
- [ ] Parse expressions (precedence climbing)
- [ ] Write AST to temp file
- [ ] Test with sample Java files

### Phase 3: Semantic Analysis (Week 3)
- [x] Design symbol table structure
- [ ] Implement symbol table builder
- [ ] Implement type checker
- [ ] Implement identifier resolver
- [ ] Validate method signatures
- [ ] Check control flow
- [ ] Write symbol table to temp file
- [ ] Test with sample Java files

### Phase 4: Code Generation (Week 4)
- [x] Design code generation context
- [ ] Implement expression code generation
- [ ] Implement statement code generation
- [ ] Implement method code generation
- [ ] Generate constant pool
- [ ] Write .djc file
- [ ] Test with sample Java files

### Phase 5: Integration & Testing (Week 5)
- [ ] Integrate all phases
- [ ] End-to-end testing
- [ ] Error message improvements
- [ ] Performance optimization
- [ ] Documentation

## Testing Strategy

### Unit Tests
- Lexer: Test tokenization of various constructs
- Parser: Test AST generation for valid/invalid syntax
- Semantic: Test type checking and symbol resolution
- CodeGen: Test bytecode generation for expressions/statements

### Integration Tests
```java
// Test 1: Hello World
class HelloWorld {
    public static void main() {
        System.out.println("Hello");
    }
}

// Test 2: Arithmetic
class Calculator {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        System.out.println(sum);
    }
}

// Test 3: Control Flow
class Counter {
    public static void main() {
        int i = 0;
        while (i < 10) {
            System.out.println(i);
            i = i + 1;
        }
    }
}

// Test 4: Methods
class Math {
    public static int add(int a, int b) {
        return a + b;
    }
    
    public static void main() {
        int result = add(5, 3);
        System.out.println(result);
    }
}

// Test 5: Objects
class Point {
    int x;
    int y;
    
    public int getX() {
        return this.x;
    }
}
```

## Error Handling

### Lexical Errors
- Invalid characters
- Unterminated strings
- Invalid number format

### Syntax Errors
- Missing semicolons
- Unmatched braces/parentheses
- Invalid statement structure

### Semantic Errors
- Undefined variables/methods
- Type mismatches
- Duplicate declarations
- Invalid return statements

### Error Message Format
```
filename.java:line:column: error: message
    source line
    ^~~~~
```

## Memory Budget (64KB Data Segment)

```
Component               Size        Notes
-------------------------------------------------
Lexer buffer           512 bytes   Source input buffer
Token buffer           2 KB        64 tokens × 32 bytes
String pool            2 KB        Identifiers/strings
Parser stack           1 KB        Recursion depth ~32
AST node buffer        4 KB        128 nodes × 32 bytes
Symbol table           8 KB        256 symbols × 32 bytes
Code generation        4 KB        Bytecode buffer
Constant pool          2 KB        64 constants
Temp file buffers      2 KB        I/O buffering
Stack/heap overhead    8 KB        Runtime overhead
-------------------------------------------------
Total                  ~33 KB      Leaves ~31KB free
```

## Performance Considerations

### Optimization Opportunities
1. **Peephole optimization**: Combine adjacent instructions
2. **Constant folding**: Evaluate constant expressions at compile time
3. **Dead code elimination**: Remove unreachable code
4. **Register allocation**: Optimize local variable usage

### Initial Implementation
- Focus on correctness, not optimization
- Generate straightforward bytecode
- Add optimizations in later phases

## Future Enhancements

1. **Inheritance support**: Extend to support class inheritance
2. **Array support**: Add array creation and access
3. **String support**: Full string literal and concatenation
4. **Constructor support**: Add constructor with parameters
5. **Static fields**: Support static field declarations
6. **Better error recovery**: Continue parsing after errors
7. **Optimization passes**: Add peephole and other optimizations
8. **IDE integration**: Create language server protocol support

## References

- Java Language Specification (subset)
- Compiler design principles (Dragon Book)
- Open Watcom C compiler documentation
- DOS file I/O programming

---

**Document Version**: 1.0
**Last Updated**: 2026-04-19
**Status**: Design Phase