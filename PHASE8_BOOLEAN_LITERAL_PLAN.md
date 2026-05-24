# Phase 8: Boolean Literal Support Implementation Plan

## Overview

DOSJavaコンパイラ（djc.exe）とVM（djvm.exe）に `true` / `false` boolean リテラルのサポートを追加する実装計画。

**現状**: boolean リテラルは字句解析器（lexer）でトークン化されているが、パーサー、意味解析器、コード生成器では処理されていないため、コンパイルエラーが発生する。

**目標**: `true` / `false` を正式にサポートし、比較式 `(1 == 1)` / `(1 == 0)` の回避策を不要にする。

## Current Status Analysis

### ✅ Already Implemented

1. **Lexer (lexer.c)**
   - `TOK_TRUE` と `TOK_FALSE` トークンが定義済み
   - キーワードテーブルに "true" と "false" が登録済み
   - トークン化は正常に動作

### ❌ Not Implemented

1. **Parser (parser.c)**
   - `parse_primary()` 関数で `TOK_TRUE` / `TOK_FALSE` を処理していない
   - 現在は `TOK_INTEGER` と `TOK_STRING` のみ対応

2. **Semantic Analyzer (semantic.c)**
   - boolean リテラルノードの型チェックが未実装

3. **Code Generator (codegen.c)**
   - boolean リテラルのバイトコード生成が未実装

## Implementation Plan

### Phase 8.1: Parser Enhancement

**File**: `dosjava/tools/compiler/parser.c`

**Location**: `parse_primary()` 関数（約2057行目）

**Changes**:
```c
uint16_t parse_primary(Parser* parser) {
    uint16_t node;
    
    /* Integer literal */
    if (parser_match(parser, TOK_INTEGER)) {
        node = parser_alloc_node(parser, NODE_LITERAL_INT);
        if (node == 0) return 0;
        parser->nodes[node].data.literal_int.value = parser->previous.value.int_value;
        return node;
    }
    
    /* Boolean literals - NEW */
    if (parser_match(parser, TOK_TRUE)) {
        node = parser_alloc_node(parser, NODE_LITERAL_BOOL);
        if (node == 0) return 0;
        parser->nodes[node].data.literal_bool.value = 1;  /* true = 1 */
        return node;
    }
    
    if (parser_match(parser, TOK_FALSE)) {
        node = parser_alloc_node(parser, NODE_LITERAL_BOOL);
        if (node == 0) return 0;
        parser->nodes[node].data.literal_bool.value = 0;  /* false = 0 */
        return node;
    }
    
    /* String literal */
    if (parser_match(parser, TOK_STRING)) {
        // ... existing code ...
    }
    
    // ... rest of function ...
}
```

**AST Node Type**: 既存の `NODE_LITERAL_BOOL` を使用（ast.h で既に定義済みの可能性が高い）

### Phase 8.2: AST Node Definition (if needed)

**File**: `dosjava/tools/compiler/ast.h`

**Check**: `NODE_LITERAL_BOOL` が定義されているか確認

**If not defined, add**:
```c
typedef enum {
    // ... existing node types ...
    NODE_LITERAL_INT,
    NODE_LITERAL_BOOL,    /* Boolean literal (true/false) - NEW */
    NODE_LITERAL_STRING,
    // ... rest ...
} NodeType;
```

**Node Data Structure**:
```c
typedef struct ASTNode {
    NodeType type;
    uint16_t line;
    uint16_t column;
    
    union {
        struct {
            int32_t value;
        } literal_int;
        
        struct {
            uint8_t value;  /* 0 = false, 1 = true */
        } literal_bool;     /* NEW */
        
        struct {
            uint16_t str_index;
        } literal_string;
        
        // ... other node data ...
    } data;
    
    // ... children, etc ...
} ASTNode;
```

### Phase 8.3: Semantic Analyzer Enhancement

**File**: `dosjava/tools/compiler/semantic.c`

**Function**: `analyze_expression()` または類似の型チェック関数

**Changes**:
```c
Type analyze_expression(SemanticAnalyzer* analyzer, uint16_t node_index) {
    ASTNode* node = &analyzer->parser->nodes[node_index];
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            return TYPE_INT;
        
        case NODE_LITERAL_BOOL:  /* NEW */
            return TYPE_BOOLEAN;
        
        case NODE_LITERAL_STRING:
            return TYPE_STRING;
        
        // ... other cases ...
    }
}
```

**Validation**: boolean リテラルが boolean 型として正しく認識されることを確認

### Phase 8.4: Code Generator Enhancement

**File**: `dosjava/tools/compiler/codegen.c`

**Function**: `generate_expression()` または類似のコード生成関数

**Changes**:
```c
void generate_expression(CodeGenerator* gen, uint16_t node_index) {
    ASTNode* node = &gen->parser->nodes[node_index];
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            emit_load_int(gen, node->data.literal_int.value);
            break;
        
        case NODE_LITERAL_BOOL:  /* NEW */
            /* Generate bytecode to push boolean value onto stack */
            if (node->data.literal_bool.value) {
                emit_byte(gen, OP_ICONST_1);  /* true */
            } else {
                emit_byte(gen, OP_ICONST_0);  /* false */
            }
            break;
        
        case NODE_LITERAL_STRING:
            emit_load_string(gen, node->data.literal_string.str_index);
            break;
        
        // ... other cases ...
    }
}
```

**Bytecode**: boolean値は内部的に整数（0/1）として扱われるため、既存の `OP_ICONST_0` / `OP_ICONST_1` オペコードを使用

### Phase 8.5: VM Support Verification

**File**: `dosjava/src/vm/interpreter.c`

**Status**: VMは既にboolean値を整数として扱っているため、追加の変更は不要

**Verification**:
- `OP_ICONST_0` と `OP_ICONST_1` が正しく実装されていることを確認
- boolean演算（`&&`, `||`, `!`）が整数値（0/1）で正しく動作することを確認

## Testing Strategy

### Test 8.1: Basic Boolean Literals

**File**: `dosjava/tests/boolliteral.jav`

```java
class BoolLiteralTest {
    public static void main() {
        boolean t = true;
        boolean f = false;
        
        if (t && !f) {
            System.out.println("Boolean literals work!");
        }
        
        return;
    }
}
```

**Expected Output**: "Boolean literals work!"

### Test 8.2: Boolean in Conditions

**File**: `dosjava/tests/boolcond.jav`

```java
class BoolCondTest {
    public static void main() {
        // Test true in while condition
        int count = 0;
        while (true) {
            count = count + 1;
            if (count == 3) {
                break;
            }
        }
        
        // Test false in if condition
        if (false) {
            System.out.println("ERROR: Should not print");
        } else {
            System.out.println("Boolean conditions work!");
        }
        
        return;
    }
}
```

**Expected Output**: "Boolean conditions work!"

### Test 8.3: Boolean Arrays

**File**: Update `dosjava/samples/boolarr.jav`

```java
class BoolArrTest {
    public static void main() {
        // Use true/false literals instead of (1==1)/(1==0)
        boolean[] arr1 = new boolean[5];
        arr1[0] = true;
        arr1[1] = false;
        arr1[2] = true;
        arr1[3] = false;
        arr1[4] = true;
        
        // Test array initialization (if supported)
        // boolean[] arr2 = {true, true, false, false};
        
        int trueCount = 0;
        int i = 0;
        while (i < 5) {
            if (arr1[i]) {
                trueCount = trueCount + 1;
            }
            i = i + 1;
        }
        
        if (trueCount == 3) {
            System.out.println("boolarr.jav worked correctly.");
        }
        
        return;
    }
}
```

### Test 8.4: Update Existing Tests

**Files to Update**:
1. `dosjava/samples/bool.jav` - Replace `(1 == 1)` with `true`, `(1 == 0)` with `false`
2. `dosjava/samples/while.jav` - Replace `while (1 == 1)` with `while (true)`
3. Any other test files using boolean workarounds

## Implementation Steps

### Step 1: Code Analysis ✅
- [x] Confirm lexer already supports `TOK_TRUE` / `TOK_FALSE`
- [x] Identify parser location for enhancement
- [x] Identify semantic analyzer location
- [x] Identify code generator location

### Step 2: AST Enhancement
- [ ] Check if `NODE_LITERAL_BOOL` exists in ast.h
- [ ] Add `NODE_LITERAL_BOOL` if needed
- [ ] Add `literal_bool` data structure if needed

### Step 3: Parser Implementation
- [ ] Add `TOK_TRUE` handling in `parse_primary()`
- [ ] Add `TOK_FALSE` handling in `parse_primary()`
- [ ] Test parser with boolean literal inputs

### Step 4: Semantic Analyzer Implementation
- [ ] Add `NODE_LITERAL_BOOL` case in type checking
- [ ] Verify boolean type is correctly assigned
- [ ] Test semantic analysis with boolean literals

### Step 5: Code Generator Implementation
- [ ] Add `NODE_LITERAL_BOOL` case in code generation
- [ ] Emit `OP_ICONST_0` for `false`
- [ ] Emit `OP_ICONST_1` for `true`
- [ ] Test bytecode generation

### Step 6: Integration Testing
- [ ] Compile and test `boolliteral.jav`
- [ ] Compile and test `boolcond.jav`
- [ ] Update and test `boolarr.jav`
- [ ] Update and test `bool.jav`
- [ ] Update and test `while.jav`

### Step 7: Regression Testing
- [ ] Run full test suite in `dosjava/samples/`
- [ ] Verify no existing tests are broken
- [ ] Verify all updated tests pass

### Step 8: Documentation
- [ ] Update FEATURES.md to include boolean literals
- [ ] Update COMPILER_IMPLEMENTATION_STATUS.md
- [ ] Create PHASE8_COMPLETION.md with results

## Expected Outcomes

### Compiler Behavior

**Before**:
```
> djc.exe bool.jav
Parse error at line 4, column 17: Expected expression
Compilation failed
```

**After**:
```
> djc.exe bool.jav
Compiled: bool.jav -> bool.djc

> djvm.exe bool.djc
Boolean literals work!
```

### Code Quality Improvements

1. **Readability**: `boolean t = true;` is clearer than `boolean t = (1 == 1);`
2. **Standards Compliance**: Matches Java language specification
3. **Maintainability**: Reduces workarounds in test code
4. **User Experience**: Developers can write natural Java code

## Risk Assessment

### Low Risk
- Lexer already supports the tokens
- VM already handles boolean values as integers
- Implementation is straightforward addition to existing pipeline

### Potential Issues
1. **AST Node Allocation**: Ensure `NODE_LITERAL_BOOL` doesn't conflict with existing node types
2. **Memory**: Verify no memory issues in 16-bit DOS environment
3. **Bytecode Compatibility**: Ensure generated bytecode is compatible with existing VM

### Mitigation
- Thorough testing at each phase
- Incremental implementation with validation
- Regression testing to catch any breakage

## Timeline Estimate

- **Phase 8.1-8.2 (AST/Parser)**: 1-2 hours
- **Phase 8.3 (Semantic)**: 30 minutes
- **Phase 8.4 (CodeGen)**: 30 minutes
- **Phase 8.5 (Testing)**: 1-2 hours
- **Total**: 3-5 hours

## Success Criteria

1. ✅ `true` and `false` keywords compile without errors
2. ✅ Boolean literals have correct type (TYPE_BOOLEAN)
3. ✅ Generated bytecode correctly represents boolean values
4. ✅ VM executes boolean literal code correctly
5. ✅ All existing tests continue to pass
6. ✅ Updated tests using `true`/`false` pass
7. ✅ Documentation is updated

## References

- **Lexer Implementation**: `dosjava/tools/compiler/lexer.c` (lines 41-42)
- **Parser Structure**: `dosjava/tools/compiler/parser.c` (parse_primary function)
- **AST Definitions**: `dosjava/tools/compiler/ast.h`
- **Semantic Analyzer**: `dosjava/tools/compiler/semantic.c`
- **Code Generator**: `dosjava/tools/compiler/codegen.c`
- **Compiler Design**: `dosjava/COMPILER_DESIGN.md`
- **Implementation Status**: `dosjava/COMPILER_IMPLEMENTATION_STATUS.md`

---

**Document Version**: 1.0  
**Created**: 2026-05-24  
**Status**: Planning Phase