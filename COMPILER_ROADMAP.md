# DOS Java Compiler (djc) - Implementation Roadmap

## Overview

PC-DOS上で動作するJavaソースコードから.djcバイトコードへの直接コンパイラの実装計画。

## Implementation Strategy

### Multi-Pass Compilation with Temporary Files

16-bit DOS環境のメモリ制約（64KB data segment）に対応するため、複数パスで処理し、中間結果を一時ファイルに保存します。

```
Pass 1: Lexer      → tokens.tmp
Pass 2: Parser     → ast.tmp
Pass 3: Semantic   → symbols.tmp + ast_annotated.tmp
Pass 4: CodeGen    → output.djc
```

## Phase 1: Lexer Implementation (Week 1)

### Goals
- Javaソースコードをトークン列に変換
- トークンを一時ファイルに書き込み
- メモリ効率的な実装（512バイトバッファ）

### Tasks

#### 1.1 Token Structure Definition
**File**: `tools/compiler/lexer.h`

```c
typedef enum {
    TOK_EOF = 0,
    TOK_CLASS, TOK_PUBLIC, TOK_STATIC, TOK_VOID,
    TOK_INT, TOK_BOOLEAN, TOK_IF, TOK_ELSE,
    TOK_WHILE, TOK_FOR, TOK_RETURN, TOK_NEW, TOK_THIS,
    TOK_IDENTIFIER, TOK_INTEGER, TOK_STRING,
    TOK_TRUE, TOK_FALSE,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_ASSIGN, TOK_EQ, TOK_NE, TOK_LT, TOK_LE, TOK_GT, TOK_GE,
    TOK_AND, TOK_OR, TOK_NOT,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET, TOK_SEMICOLON, TOK_COMMA, TOK_DOT
} TokenType;

typedef struct {
    TokenType type;
    uint16_t line;
    uint16_t column;
    union {
        int16_t int_value;
        uint16_t str_offset;
    } value;
} Token;
```

#### 1.2 Lexer State Machine
**File**: `tools/compiler/lexer.c`

**Functions to implement**:
- `lexer_init(const char* filename)` - Initialize lexer with source file
- `lexer_next_token(Token* token)` - Get next token
- `lexer_peek_char()` - Look ahead one character
- `lexer_consume_char()` - Consume current character
- `lexer_skip_whitespace()` - Skip whitespace and comments
- `lexer_read_identifier()` - Read identifier or keyword
- `lexer_read_number()` - Read integer literal
- `lexer_read_string()` - Read string literal
- `lexer_write_token(Token* token)` - Write token to temp file
- `lexer_cleanup()` - Close files and cleanup

**Implementation details**:
```c
typedef struct {
    FILE* source;           /* Source file */
    FILE* tokens;           /* Token output file */
    char buffer[512];       /* Input buffer */
    uint16_t buf_pos;       /* Current position in buffer */
    uint16_t buf_len;       /* Valid data in buffer */
    uint16_t line;          /* Current line number */
    uint16_t column;        /* Current column number */
    char string_pool[2048]; /* String pool for identifiers */
    uint16_t pool_size;     /* Current pool size */
} Lexer;
```

#### 1.3 Keyword Recognition
**Keywords to support**:
```c
static const struct {
    const char* name;
    TokenType type;
} keywords[] = {
    {"class", TOK_CLASS},
    {"public", TOK_PUBLIC},
    {"static", TOK_STATIC},
    {"void", TOK_VOID},
    {"int", TOK_INT},
    {"boolean", TOK_BOOLEAN},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {"for", TOK_FOR},
    {"return", TOK_RETURN},
    {"new", TOK_NEW},
    {"this", TOK_THIS},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {NULL, TOK_EOF}
};
```

#### 1.4 Testing
**Test files**:
- `tests/lexer/test_keywords.java` - Keyword recognition
- `tests/lexer/test_operators.java` - Operator recognition
- `tests/lexer/test_literals.java` - Literal parsing
- `tests/lexer/test_identifiers.java` - Identifier parsing

**Test program**: `tools/compiler/test_lexer.c`

### Deliverables
- [ ] `tools/compiler/lexer.h` - Lexer interface
- [ ] `tools/compiler/lexer.c` - Lexer implementation
- [ ] `tools/compiler/test_lexer.c` - Lexer test program
- [ ] Test cases and validation

---

## Phase 2: Parser Implementation (Week 2)

### Goals
- トークン列からAST（抽象構文木）を構築
- 構文エラーの検出
- ASTノードを一時ファイルに書き込み

### Tasks

#### 2.1 AST Node Structure
**File**: `tools/compiler/ast.h`

```c
typedef enum {
    NODE_CLASS, NODE_METHOD, NODE_FIELD, NODE_BLOCK,
    NODE_IF, NODE_WHILE, NODE_FOR, NODE_RETURN,
    NODE_EXPR_STMT, NODE_VAR_DECL, NODE_ASSIGN,
    NODE_BINARY_OP, NODE_UNARY_OP, NODE_CALL,
    NODE_NEW, NODE_FIELD_ACCESS, NODE_ARRAY_ACCESS,
    NODE_LITERAL, NODE_IDENTIFIER
} NodeType;

typedef struct {
    NodeType type;
    uint16_t line;
    uint16_t data_offset;
    uint16_t child_count;
    uint16_t first_child;
} ASTNode;
```

#### 2.2 Parser Implementation
**File**: `tools/compiler/parser.c`

**Functions to implement**:
- `parser_init(const char* token_file)` - Initialize parser
- `parser_parse()` - Parse entire program
- `parse_class()` - Parse class declaration
- `parse_method()` - Parse method declaration
- `parse_field()` - Parse field declaration
- `parse_block()` - Parse statement block
- `parse_statement()` - Parse statement
- `parse_expression()` - Parse expression (precedence climbing)
- `parse_primary()` - Parse primary expression
- `parser_write_node(ASTNode* node)` - Write AST node to file
- `parser_cleanup()` - Cleanup

**Parser state**:
```c
typedef struct {
    FILE* tokens;           /* Token input file */
    FILE* ast;              /* AST output file */
    Token current;          /* Current token */
    Token lookahead;        /* Lookahead token */
    ASTNode nodes[128];     /* Node buffer */
    uint16_t node_count;    /* Nodes in buffer */
    uint16_t total_nodes;   /* Total nodes written */
} Parser;
```

#### 2.3 Grammar Implementation

**Recursive descent functions**:
```c
/* Top-level */
ASTNode* parse_program();
ASTNode* parse_class();

/* Declarations */
ASTNode* parse_member_decl();
ASTNode* parse_field_decl();
ASTNode* parse_method_decl();
ASTNode* parse_param_list();

/* Statements */
ASTNode* parse_statement();
ASTNode* parse_var_decl();
ASTNode* parse_if_stmt();
ASTNode* parse_while_stmt();
ASTNode* parse_for_stmt();
ASTNode* parse_return_stmt();
ASTNode* parse_expr_stmt();
ASTNode* parse_block();

/* Expressions (precedence climbing) */
ASTNode* parse_expression();
ASTNode* parse_assignment();
ASTNode* parse_logical_or();
ASTNode* parse_logical_and();
ASTNode* parse_equality();
ASTNode* parse_relational();
ASTNode* parse_additive();
ASTNode* parse_multiplicative();
ASTNode* parse_unary();
ASTNode* parse_postfix();
ASTNode* parse_primary();
```

#### 2.4 Error Recovery
- Synchronization points: `;`, `}`, `class`, `public`
- Error messages with line/column information
- Continue parsing after errors when possible

#### 2.5 Testing
**Test files**:
- `tests/parser/test_class.java` - Class parsing
- `tests/parser/test_method.java` - Method parsing
- `tests/parser/test_statements.java` - Statement parsing
- `tests/parser/test_expressions.java` - Expression parsing

**Test program**: `tools/compiler/test_parser.c`

### Deliverables
- [ ] `tools/compiler/ast.h` - AST definitions
- [ ] `tools/compiler/parser.h` - Parser interface
- [ ] `tools/compiler/parser.c` - Parser implementation
- [ ] `tools/compiler/test_parser.c` - Parser test program
- [ ] Test cases and validation

---

## Phase 3: Semantic Analysis (Week 3)

### Goals
- シンボルテーブルの構築
- 型チェック
- 識別子の解決
- 意味エラーの検出

### Tasks

#### 3.1 Symbol Table Structure
**File**: `tools/compiler/symtable.h`

```c
typedef enum {
    SYM_CLASS, SYM_METHOD, SYM_FIELD,
    SYM_LOCAL, SYM_PARAM
} SymbolKind;

typedef enum {
    TYPE_VOID, TYPE_INT, TYPE_BOOLEAN, TYPE_CLASS
} TypeKind;

typedef struct {
    SymbolKind kind;
    uint16_t name_offset;
    TypeKind type;
    uint16_t type_class;
    uint16_t scope_level;
    union {
        struct { uint16_t local_index; } local;
        struct { uint16_t field_index; } field;
        struct { uint16_t param_count; uint16_t code_offset; } method;
    } data;
} Symbol;
```

#### 3.2 Symbol Table Implementation
**File**: `tools/compiler/symtable.c`

**Functions to implement**:
- `symtable_init()` - Initialize symbol table
- `symtable_enter_scope()` - Enter new scope
- `symtable_exit_scope()` - Exit current scope
- `symtable_add_symbol(Symbol* sym)` - Add symbol
- `symtable_lookup(const char* name)` - Lookup symbol
- `symtable_lookup_in_scope(const char* name, uint16_t scope)` - Scope-specific lookup
- `symtable_write()` - Write to file
- `symtable_cleanup()` - Cleanup

#### 3.3 Semantic Analyzer
**File**: `tools/compiler/semantic.c`

**Functions to implement**:
- `semantic_init(const char* ast_file, const char* symbol_file)` - Initialize
- `semantic_analyze()` - Perform semantic analysis
- `analyze_class(ASTNode* node)` - Analyze class
- `analyze_method(ASTNode* node)` - Analyze method
- `analyze_statement(ASTNode* node)` - Analyze statement
- `analyze_expression(ASTNode* node)` - Analyze expression
- `check_type(ASTNode* node, TypeKind expected)` - Type checking
- `resolve_identifier(ASTNode* node)` - Resolve identifier
- `semantic_cleanup()` - Cleanup

**Two-pass approach**:
```c
/* Pass 3a: Collect declarations */
void collect_class_symbols(ASTNode* class_node);
void collect_method_symbols(ASTNode* method_node);
void collect_field_symbols(ASTNode* field_node);

/* Pass 3b: Type check and resolve */
void check_method_body(ASTNode* method_node);
void check_statement(ASTNode* stmt_node);
TypeKind check_expression(ASTNode* expr_node);
```

#### 3.4 Type Checking Rules
```c
/* Binary operations */
int + int → int
int - int → int
int * int → int
int / int → int
int % int → int
int == int → boolean
int != int → boolean
int < int → boolean
int <= int → boolean
int > int → boolean
int >= int → boolean
boolean && boolean → boolean
boolean || boolean → boolean

/* Unary operations */
-int → int
!boolean → boolean

/* Assignment */
int = int → valid
boolean = boolean → valid
Class = Class → valid (same class)
```

#### 3.5 Error Detection
- Undefined variables/methods
- Type mismatches
- Duplicate declarations
- Invalid return types
- Missing return statements
- Invalid field/method access

#### 3.6 Testing
**Test files**:
- `tests/semantic/test_types.java` - Type checking
- `tests/semantic/test_scopes.java` - Scope resolution
- `tests/semantic/test_errors.java` - Error detection

**Test program**: `tools/compiler/test_semantic.c`

### Deliverables
- [ ] `tools/compiler/symtable.h` - Symbol table interface
- [ ] `tools/compiler/symtable.c` - Symbol table implementation
- [ ] `tools/compiler/semantic.h` - Semantic analyzer interface
- [ ] `tools/compiler/semantic.c` - Semantic analyzer implementation
- [ ] `tools/compiler/test_semantic.c` - Test program
- [ ] Test cases and validation

---

## Phase 4: Code Generation (Week 4)

### Goals
- ASTから.djcバイトコードを生成
- 定数プールの構築
- メソッドテーブルの生成
- 最適化（基本的なもの）

### Tasks

#### 4.1 Code Generation Context
**File**: `tools/compiler/codegen.h`

```c
typedef struct {
    uint16_t max_stack;
    uint16_t max_locals;
    uint16_t current_stack;
    ByteBuffer* code;
    ConstantPool* constants;
    LabelList* labels;
} CodeGenContext;

typedef struct {
    uint16_t offset;
    int16_t target;  /* -1 if not resolved */
} Label;
```

#### 4.2 Code Generator Implementation
**File**: `tools/compiler/codegen.c`

**Functions to implement**:
- `codegen_init(const char* ast_file, const char* symbol_file, const char* output_file)` - Initialize
- `codegen_generate()` - Generate code
- `generate_class(ASTNode* node)` - Generate class
- `generate_method(ASTNode* node)` - Generate method
- `generate_statement(ASTNode* node)` - Generate statement code
- `generate_expression(ASTNode* node)` - Generate expression code
- `emit_opcode(uint8_t opcode)` - Emit instruction
- `emit_u1(uint8_t value)` - Emit byte
- `emit_u2(uint16_t value)` - Emit word
- `add_constant(Constant* c)` - Add to constant pool
- `create_label()` - Create new label
- `emit_label(Label* label)` - Mark label position
- `emit_jump(uint8_t opcode, Label* label)` - Emit jump with backpatching
- `codegen_cleanup()` - Cleanup

#### 4.3 Expression Code Generation

**Stack-based evaluation**:
```c
/* Example: a + b * c */
LOAD_LOCAL 0    /* a */
LOAD_LOCAL 1    /* b */
LOAD_LOCAL 2    /* c */
MUL             /* b * c */
ADD             /* a + (b * c) */
```

**Optimization opportunities**:
```c
/* Use optimized opcodes */
LOAD_LOCAL 0 → LOAD_0
LOAD_LOCAL 1 → LOAD_1
LOAD_LOCAL 2 → LOAD_2

/* Constant folding */
PUSH_INT 5
PUSH_INT 3
ADD
→ PUSH_INT 8
```

#### 4.4 Statement Code Generation

**If statement**:
```c
/* if (condition) { then_block } else { else_block } */
<condition code>
IF_FALSE else_label
<then_block code>
GOTO end_label
else_label:
<else_block code>
end_label:
```

**While loop**:
```c
/* while (condition) { body } */
loop_start:
<condition code>
IF_FALSE loop_end
<body code>
GOTO loop_start
loop_end:
```

**For loop**:
```c
/* for (init; condition; update) { body } */
<init code>
loop_start:
<condition code>
IF_FALSE loop_end
<body code>
<update code>
GOTO loop_start
loop_end:
```

#### 4.5 Method Call Generation
```c
/* Static call: Math.add(5, 3) */
PUSH_INT 5
PUSH_INT 3
INVOKE_STATIC <method_index>

/* Virtual call: obj.method(arg) */
LOAD_LOCAL 0    /* obj */
LOAD_LOCAL 1    /* arg */
INVOKE_VIRTUAL <method_index>
```

#### 4.6 .djc File Generation

**File structure**:
```c
/* Write header */
write_djc_header(file, &header);

/* Write constant pool */
for (i = 0; i < constant_count; i++) {
    write_constant(file, &constants[i]);
}

/* Write method table */
for (i = 0; i < method_count; i++) {
    write_method_info(file, &methods[i]);
}

/* Write field table */
for (i = 0; i < field_count; i++) {
    write_field_info(file, &fields[i]);
}

/* Write bytecode */
fwrite(code_buffer, 1, code_size, file);
```

#### 4.7 Testing
**Test files**:
- `tests/codegen/test_arithmetic.java` - Arithmetic operations
- `tests/codegen/test_control.java` - Control flow
- `tests/codegen/test_methods.java` - Method calls
- `tests/codegen/test_objects.java` - Object operations

**Test program**: `tools/compiler/test_codegen.c`

### Deliverables
- [ ] `tools/compiler/codegen.h` - Code generator interface
- [ ] `tools/compiler/codegen.c` - Code generator implementation
- [ ] `tools/compiler/test_codegen.c` - Test program
- [ ] Test cases and validation

---

## Phase 5: Integration & CLI (Week 5)

### Goals
- 全フェーズの統合
- コマンドラインインターフェース
- エラーメッセージの改善
- ドキュメント作成

### Tasks

#### 5.1 Main Compiler Driver
**File**: `tools/compiler/djc.c`

```c
int main(int argc, char* argv[]) {
    /* Parse command line */
    parse_args(argc, argv);
    
    /* Phase 1: Lexical analysis */
    if (!run_lexer(input_file, "tokens.tmp")) {
        return 1;
    }
    
    /* Phase 2: Parsing */
    if (!run_parser("tokens.tmp", "ast.tmp")) {
        return 1;
    }
    
    /* Phase 3: Semantic analysis */
    if (!run_semantic("ast.tmp", "symbols.tmp")) {
        return 1;
    }
    
    /* Phase 4: Code generation */
    if (!run_codegen("ast.tmp", "symbols.tmp", output_file)) {
        return 1;
    }
    
    /* Cleanup temp files */
    if (!keep_temps) {
        cleanup_temp_files();
    }
    
    return 0;
}
```

#### 5.2 Command Line Interface
```bash
Usage: djc [options] <input.java>

Options:
  -o <file>     Specify output file (default: <input>.djc)
  -v            Verbose output
  -k            Keep temporary files
  -d            Debug mode (show all phases)
  -h            Show this help

Examples:
  djc HelloWorld.java
  djc -o hello.djc HelloWorld.java
  djc -v -k Test.java
```

#### 5.3 Error Message Improvements
```c
/* Error reporting structure */
typedef struct {
    const char* filename;
    uint16_t line;
    uint16_t column;
    const char* message;
    const char* source_line;
} ErrorInfo;

/* Error display */
void report_error(ErrorInfo* error) {
    fprintf(stderr, "%s:%d:%d: error: %s\n",
            error->filename, error->line, error->column,
            error->message);
    fprintf(stderr, "    %s\n", error->source_line);
    fprintf(stderr, "    ");
    for (int i = 0; i < error->column - 1; i++) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");
}
```

#### 5.4 Integration Testing

**End-to-end test suite**:
```bash
# Test 1: Hello World
djc tests/e2e/HelloWorld.java
dosjava HelloWorld.djc
# Expected: "Hello, DOS!"

# Test 2: Arithmetic
djc tests/e2e/Calculator.java
dosjava Calculator.djc
# Expected: "30"

# Test 3: Control Flow
djc tests/e2e/Counter.java
dosjava Counter.djc
# Expected: "1\n2\n3\n...\n10"

# Test 4: Methods
djc tests/e2e/Math.java
dosjava Math.djc
# Expected: "8"

# Test 5: Objects
djc tests/e2e/Point.java
dosjava Point.djc
# Expected: "10\n20"
```

#### 5.5 Documentation

**Files to create**:
- `docs/COMPILER_USAGE.md` - User guide
- `docs/COMPILER_INTERNALS.md` - Developer guide
- `docs/SUPPORTED_JAVA.md` - Supported Java subset
- `docs/ERROR_MESSAGES.md` - Error message reference

#### 5.6 Build System Integration

**Update Makefile**:
```makefile
# Add compiler to all target
all: test_memory test_interpreter mkdjc java2djc djc

# Compiler object files
COMPILER_OBJS = $(OBJ_DIR)/djc.obj \
                $(OBJ_DIR)/lexer.obj \
                $(OBJ_DIR)/parser.obj \
                $(OBJ_DIR)/semantic.obj \
                $(OBJ_DIR)/codegen.obj \
                $(OBJ_DIR)/symtable.obj

# djc compiler target
djc: $(BIN_DIR)/djc.exe

$(BIN_DIR)/djc.exe: $(COMPILER_OBJS) $(FORMAT_OBJS)
	@echo Linking djc.exe...
	$(LD) $(LDFLAGS) name $@ file { $(COMPILER_OBJS) $(FORMAT_OBJS) }

# Compile rules for compiler
$(OBJ_DIR)/djc.obj: tools/compiler/djc.c
	@echo Compiling djc.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/djc.c

$(OBJ_DIR)/lexer.obj: tools/compiler/lexer.c tools/compiler/lexer.h
	@echo Compiling lexer.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/lexer.c

$(OBJ_DIR)/parser.obj: tools/compiler/parser.c tools/compiler/parser.h
	@echo Compiling parser.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/parser.c

$(OBJ_DIR)/semantic.obj: tools/compiler/semantic.c tools/compiler/semantic.h
	@echo Compiling semantic.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/semantic.c

$(OBJ_DIR)/codegen.obj: tools/compiler/codegen.c tools/compiler/codegen.h
	@echo Compiling codegen.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/codegen.c

$(OBJ_DIR)/symtable.obj: tools/compiler/symtable.c tools/compiler/symtable.h
	@echo Compiling symtable.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/symtable.c
```

### Deliverables
- [ ] `tools/compiler/djc.c` - Main compiler driver
- [ ] Integration tests
- [ ] Documentation
- [ ] Updated Makefile
- [ ] Release notes

---

## Testing Strategy

### Unit Tests
Each phase has its own test program:
- `test_lexer.exe` - Test lexical analysis
- `test_parser.exe` - Test parsing
- `test_semantic.exe` - Test semantic analysis
- `test_codegen.exe` - Test code generation

### Integration Tests
End-to-end compilation and execution:
```bash
# Run all integration tests
wmake test_compiler

# Individual test
djc tests/e2e/HelloWorld.java
dosjava HelloWorld.djc
```

### Regression Tests
Maintain test suite for bug fixes:
- `tests/regression/` - Known bug fixes
- Automated test runner

---

## Timeline

| Week | Phase | Deliverables |
|------|-------|--------------|
| 1 | Lexer | Token structure, lexer implementation, tests |
| 2 | Parser | AST structure, parser implementation, tests |
| 3 | Semantic | Symbol table, type checker, tests |
| 4 | CodeGen | Code generator, .djc writer, tests |
| 5 | Integration | CLI, documentation, integration tests |

**Total**: 5 weeks for initial implementation

---

## Success Criteria

### Minimum Viable Product (MVP)
- [ ] Compile simple Java programs (HelloWorld, Calculator)
- [ ] Support basic data types (int, boolean)
- [ ] Support control flow (if/else, while)
- [ ] Support methods (static only)
- [ ] Generate valid .djc files
- [ ] Run on DOS Java VM

### Extended Features (Post-MVP)
- [ ] Object-oriented features (classes, fields, methods)
- [ ] For loops
- [ ] Arrays
- [ ] String support
- [ ] Better error messages
- [ ] Optimization passes

---

## Risk Mitigation

### Memory Constraints
**Risk**: 64KB data segment too small
**Mitigation**: 
- Use temporary files for intermediate data
- Process in small chunks
- Optimize data structures

### Complexity
**Risk**: Full Java parser too complex
**Mitigation**:
- Start with minimal subset
- Incremental feature addition
- Focus on correctness over optimization

### Testing
**Risk**: Insufficient test coverage
**Mitigation**:
- Unit tests for each phase
- Integration tests for end-to-end
- Regression tests for bug fixes

---

## Future Enhancements

1. **Optimization**
   - Constant folding
   - Dead code elimination
   - Peephole optimization

2. **Language Features**
   - Inheritance
   - Interfaces
   - Arrays
   - String operations

3. **Tooling**
   - Debugger integration
   - IDE support
   - Language server protocol

4. **Performance**
   - Faster compilation
   - Better memory usage
   - Parallel processing (if possible)

---

**Document Version**: 1.0
**Last Updated**: 2026-04-19
**Status**: Planning Phase