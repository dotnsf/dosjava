# Phase 3: Semantic Analyzer Implementation Plan

## Overview
Phase 3では、パーサーが生成したASTに対して意味解析を行います。主な目的は：
1. **シンボルテーブルの構築** - クラス、メソッド、フィールド、変数の宣言を収集
2. **型チェック** - 式や文の型が正しいか検証
3. **識別子の解決** - 変数やメソッドの参照が正しいか確認
4. **意味エラーの検出** - 未定義変数、型不一致、重複宣言などを検出

## Architecture

### Two-Pass Approach
意味解析は2パスで実行します：

**Pass 1: Declaration Collection (宣言収集)**
- クラス、メソッド、フィールドの宣言を収集
- シンボルテーブルに登録
- スコープ構造を構築

**Pass 2: Type Checking & Resolution (型チェックと解決)**
- メソッド本体を解析
- 式の型チェック
- 識別子の解決
- 意味エラーの検出

### Memory Constraints (16-bit DOS)
- シンボルテーブル: 最大256エントリ (各32バイト = 8KB)
- スコープスタック: 最大16レベル (512バイト)
- エラーバッファ: 2KB
- 合計: 約11KB (64KBデータセグメント内で十分)

## Components

### 1. Symbol Table (symtable.h/c)

#### Data Structures

```c
/* Symbol kinds */
typedef enum {
    SYM_CLASS,      /* Class symbol */
    SYM_METHOD,     /* Method symbol */
    SYM_FIELD,      /* Field symbol */
    SYM_LOCAL,      /* Local variable */
    SYM_PARAM       /* Method parameter */
} SymbolKind;

/* Symbol entry */
typedef struct {
    SymbolKind kind;
    uint16_t name_offset;       /* Offset in string pool */
    TypeInfo type;              /* Symbol type */
    uint16_t scope_level;       /* Scope nesting level */
    
    /* Kind-specific data */
    union {
        struct {
            uint16_t member_count;  /* Number of members */
        } class_data;
        
        struct {
            uint16_t param_count;   /* Number of parameters */
            uint16_t local_count;   /* Number of local variables */
        } method_data;
        
        struct {
            uint16_t index;         /* Local variable index */
        } local_data;
        
        struct {
            uint16_t index;         /* Parameter index */
        } param_data;
    } data;
} Symbol;

/* Symbol table */
typedef struct {
    Symbol symbols[256];        /* Symbol entries (8KB) */
    uint16_t symbol_count;
    
    uint16_t scope_stack[16];   /* Scope start indices */
    uint16_t scope_level;       /* Current scope level */
    
    char string_pool[2048];     /* Shared string pool */
    uint16_t pool_size;
} SymbolTable;
```

#### Functions

```c
/* Initialization */
int symtable_init(SymbolTable* table);
void symtable_cleanup(SymbolTable* table);

/* Scope management */
void symtable_enter_scope(SymbolTable* table);
void symtable_exit_scope(SymbolTable* table);

/* Symbol operations */
int symtable_add_symbol(SymbolTable* table, const Symbol* sym);
Symbol* symtable_lookup(SymbolTable* table, const char* name);
Symbol* symtable_lookup_in_scope(SymbolTable* table, const char* name, uint16_t scope);

/* String pool */
uint16_t symtable_add_string(SymbolTable* table, const char* str);
const char* symtable_get_string(SymbolTable* table, uint16_t offset);

/* I/O */
int symtable_write(SymbolTable* table, const char* filename);
int symtable_read(SymbolTable* table, const char* filename);
```

### 2. Semantic Analyzer (semantic.h/c)

#### Data Structures

```c
/* Semantic analyzer state */
typedef struct {
    /* Input */
    FILE* ast_file;             /* AST input file */
    ASTNode nodes[128];         /* Node buffer */
    uint16_t node_count;
    uint16_t total_nodes;
    
    /* Symbol table */
    SymbolTable* symtable;
    
    /* Current context */
    Symbol* current_class;      /* Current class being analyzed */
    Symbol* current_method;     /* Current method being analyzed */
    TypeInfo expected_return;   /* Expected return type */
    
    /* Error tracking */
    int has_error;
    uint16_t error_count;
    char error_buffer[2048];
} SemanticAnalyzer;
```

#### Functions

```c
/* Initialization */
int semantic_init(SemanticAnalyzer* analyzer, const char* ast_file, const char* symbol_file);
void semantic_cleanup(SemanticAnalyzer* analyzer);

/* Main analysis */
int semantic_analyze(SemanticAnalyzer* analyzer);

/* Pass 1: Declaration collection */
int collect_declarations(SemanticAnalyzer* analyzer);
int collect_class_symbols(SemanticAnalyzer* analyzer, ASTNode* class_node);
int collect_method_symbols(SemanticAnalyzer* analyzer, ASTNode* method_node);
int collect_field_symbols(SemanticAnalyzer* analyzer, ASTNode* field_node);

/* Pass 2: Type checking */
int check_semantics(SemanticAnalyzer* analyzer);
int check_method_body(SemanticAnalyzer* analyzer, ASTNode* method_node);
int check_statement(SemanticAnalyzer* analyzer, ASTNode* stmt_node);
TypeInfo check_expression(SemanticAnalyzer* analyzer, ASTNode* expr_node);

/* Type checking helpers */
int types_compatible(TypeInfo t1, TypeInfo t2);
int is_boolean_type(TypeInfo type);
int is_numeric_type(TypeInfo type);

/* Error reporting */
void semantic_error(SemanticAnalyzer* analyzer, uint16_t line, uint16_t col, const char* message);
```

## Type Checking Rules

### Binary Operations

| Operation | Left Type | Right Type | Result Type | Valid |
|-----------|-----------|------------|-------------|-------|
| `+`, `-`, `*`, `/`, `%` | int | int | int | ✓ |
| `==`, `!=` | int | int | boolean | ✓ |
| `==`, `!=` | boolean | boolean | boolean | ✓ |
| `<`, `<=`, `>`, `>=` | int | int | boolean | ✓ |
| `&&`, `||` | boolean | boolean | boolean | ✓ |

### Unary Operations

| Operation | Operand Type | Result Type | Valid |
|-----------|--------------|-------------|-------|
| `-` | int | int | ✓ |
| `!` | boolean | boolean | ✓ |

### Assignment

| Target Type | Value Type | Valid |
|-------------|------------|-------|
| int | int | ✓ |
| boolean | boolean | ✓ |
| Class | Same Class | ✓ |

## Error Detection

### Semantic Errors to Detect

1. **Undefined symbols**
   - Undefined variable reference
   - Undefined method call
   - Undefined class reference

2. **Type mismatches**
   - Incompatible assignment types
   - Invalid operand types for operators
   - Wrong argument types in method calls
   - Wrong return type

3. **Duplicate declarations**
   - Duplicate class name
   - Duplicate method name (same signature)
   - Duplicate field name
   - Duplicate local variable name

4. **Scope violations**
   - Variable used before declaration
   - Variable used outside its scope

5. **Return statement issues**
   - Missing return in non-void method
   - Return with value in void method
   - Return without value in non-void method

## Implementation Steps

### Step 1: Symbol Table Header (symtable.h)
- Define data structures
- Declare all functions
- Add documentation

**Estimated size**: ~150 lines

### Step 2: Symbol Table Implementation (symtable.c)
- Implement initialization/cleanup
- Implement scope management
- Implement symbol operations
- Implement string pool operations
- Implement I/O operations

**Estimated size**: ~400 lines

### Step 3: Semantic Analyzer Header (semantic.h)
- Define data structures
- Declare all functions
- Add documentation

**Estimated size**: ~100 lines

### Step 4: Semantic Analyzer Implementation (semantic.c)
- Implement initialization/cleanup
- Implement Pass 1 (declaration collection)
- Implement Pass 2 (type checking)
- Implement type checking helpers
- Implement error reporting

**Estimated size**: ~800 lines

### Step 5: Test Program (test_semantic.c)
- Read AST file
- Run semantic analysis
- Report errors
- Write symbol table

**Estimated size**: ~200 lines

### Step 6: Test Cases
Create test Java files:
- `tests/semantic/test_types.java` - Type checking
- `tests/semantic/test_scopes.java` - Scope resolution
- `tests/semantic/test_errors.java` - Error detection

### Step 7: Integration Testing
- Test with existing parser output
- Verify error detection
- Verify symbol table generation

## Testing Strategy

### Unit Tests
1. Symbol table operations
   - Add/lookup symbols
   - Scope management
   - String pool operations

2. Type checking
   - Binary operations
   - Unary operations
   - Assignments

### Integration Tests
1. Simple class with methods
2. Multiple scopes (nested blocks)
3. Type errors (intentional)
4. Undefined references (intentional)

### Test Files

**test_types.java** - Valid type usage
```java
class TypeTest {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b * 2;
        boolean flag = a < b;
        if (flag) {
            int d = c - a;
        }
    }
}
```

**test_scopes.java** - Scope resolution
```java
class ScopeTest {
    int field = 5;
    
    public void method(int param) {
        int local = param + field;
        if (local > 10) {
            int inner = local * 2;
        }
        // inner not accessible here
    }
}
```

**test_errors.java** - Error detection
```java
class ErrorTest {
    public void method() {
        int x = y;          // Error: undefined variable
        boolean b = x + 5;  // Error: type mismatch
        int z = z;          // Error: self-reference
    }
}
```

## Build System Integration

### Makefile Updates

Add to makefile:
```makefile
# Semantic analyzer
$(OBJ_DIR)/symtable.obj: tools/compiler/symtable.c tools/compiler/symtable.h
	@echo Compiling symtable.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/symtable.c

$(OBJ_DIR)/semantic.obj: tools/compiler/semantic.c tools/compiler/semantic.h
	@echo Compiling semantic.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/semantic.c

$(OBJ_DIR)/test_semantic.obj: tools/compiler/test_semantic.c
	@echo Compiling test_semantic.c...
	$(CC) $(CFLAGS) -fo=$@ tools/compiler/test_semantic.c

$(BIN_DIR)/test_semantic.exe: $(OBJ_DIR)/test_semantic.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj
	@echo Linking test_semantic.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/test_semantic.obj $(OBJ_DIR)/semantic.obj $(OBJ_DIR)/symtable.obj $(OBJ_DIR)/parser.obj $(OBJ_DIR)/lexer.obj }

test_semantic: $(BIN_DIR)/test_semantic.exe
```

## Success Criteria

Phase 3 is complete when:
1. ✓ Symbol table can store and retrieve symbols
2. ✓ Scope management works correctly
3. ✓ Type checking detects all specified errors
4. ✓ All test cases pass
5. ✓ Symbol table file is generated correctly
6. ✓ Error messages are clear and helpful
7. ✓ Memory usage stays within 16-bit DOS limits

## Next Phase

After Phase 3 completion, proceed to:
- **Phase 4**: Code Generator - Generate .djc bytecode from analyzed AST

## Notes

- Keep memory usage minimal (target: <12KB for semantic analyzer)
- Use heap allocation for large structures (Lexer, Parser, SemanticAnalyzer)
- Maintain 1-based node indexing (0 = error)
- Follow C89 standard for Open Watcom compatibility
- Add comprehensive error messages with line/column information