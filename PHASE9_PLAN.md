# Phase 9: Type System Enhancements - Implementation Plan

## Overview

Phase 9では、DOSJavaの型システムを強化し、より柔軟なプログラミングを可能にします。3つのサブフェーズで段階的に実装します。

## Phase 9.1: Type Casting Support

### 目標
型変換（キャスト）のサポートを追加し、異なる数値型間での変換を可能にする。

### 実装内容

#### 1. Widening Cast（暗黙的型変換）
自動的に行われる安全な型変換：
- `int` → `long` (16-bit → 32-bit)
- `int` → `float` (16-bit整数 → 32-bit浮動小数点)
- `long` → `float` (32-bit整数 → 32-bit浮動小数点)

#### 2. Narrowing Cast（明示的型変換）
プログラマが明示的に指定する型変換：
- `long` → `int` (32-bit → 16-bit、オーバーフロー注意)
- `float` → `int` (浮動小数点 → 整数、小数部切り捨て)

#### 3. Math関数の追加
- `float Math.floor(float x)` - 床関数（小数点以下切り捨て）
- `float Math.ceil(float x)` - 天井関数（小数点以下切り上げ）

### 技術設計

#### AST拡張
```c
/* ast.h */
typedef enum {
    // ... existing node types ...
    NODE_CAST,           /* Type cast expression */
} NodeType;

typedef struct {
    uint16_t expr;       /* Expression to cast */
    TypeInfo target_type; /* Target type */
    uint8_t is_explicit; /* 0=implicit, 1=explicit */
} CastData;
```

#### Parser拡張
```c
/* parser.c - parse_cast_expression() */
// Explicit cast: (type)expression
if (parser_match(parser, TOK_LPAREN)) {
    if (is_type_token(parser->current)) {
        TypeInfo target_type = parse_type(parser);
        parser_consume(parser, TOK_RPAREN);
        uint16_t expr = parse_unary(parser);
        return create_cast_node(parser, expr, target_type, 1);
    }
}
```

#### Semantic Analyzer拡張
```c
/* semantic.c - analyze_assignment() */
// Implicit cast insertion
if (needs_widening_cast(expr_type, var_type)) {
    expr_node = insert_implicit_cast(analyzer, expr_node, var_type);
}
```

#### Code Generator拡張
```c
/* codegen.c - generate_cast() */
switch (cast_type) {
    case INT_TO_LONG:
        emit_opcode(gen, OP_I2L);  /* Sign-extend int to long */
        break;
    case INT_TO_FLOAT:
        emit_opcode(gen, OP_I2F);  /* Convert int to float */
        break;
    case LONG_TO_INT:
        emit_opcode(gen, OP_L2I);  /* Truncate long to int */
        break;
    case FLOAT_TO_INT:
        emit_opcode(gen, OP_F2I);  /* Truncate float to int */
        break;
}
```

#### VM Opcodes
新しいオペコードの追加：
```c
/* opcodes.h */
#define OP_I2L  0xA0  /* int to long */
#define OP_I2F  0xA1  /* int to float */
#define OP_L2I  0xA2  /* long to int */
#define OP_L2F  0xA3  /* long to float */
#define OP_F2I  0xA4  /* float to int */
```

#### VM Implementation
```c
/* interpreter.c */
case OP_I2L: {
    int16_t i = stack_pop_int(stack);
    int32_t l = (int32_t)i;  /* Sign-extend */
    stack_push_long(stack, l);
    break;
}

case OP_I2F: {
    int16_t i = stack_pop_int(stack);
    float f = (float)i;
    stack_push_float(stack, f);
    break;
}

case OP_L2I: {
    int32_t l = stack_pop_long(stack);
    int16_t i = (int16_t)(l & 0xFFFF);  /* Truncate */
    stack_push_int(stack, i);
    break;
}

case OP_F2I: {
    float f = stack_pop_float(stack);
    int16_t i = (int16_t)f;  /* Truncate */
    stack_push_int(stack, i);
    break;
}
```

### テスト計画

#### Test 1: Widening Cast
```java
class WideningCastTest {
    public static void main() {
        int i = 100;
        long l = i;           // int → long
        float f1 = i;         // int → float
        float f2 = l;         // long → float
        
        if (l == 100L && f1 > 99.9f && f1 < 100.1f) {
            System.out.println("Widening cast works!");
        }
    }
}
```

#### Test 2: Explicit Cast
```java
class ExplicitCastTest {
    public static void main() {
        long l = 70000L;
        int i = (int)l;       // long → int (overflow)
        
        float f = 123.456f;
        int i2 = (int)f;      // float → int (truncate)
        
        System.out.println(i2);  // Should print 123
    }
}
```

#### Test 3: Math.floor/ceil
```java
class MathFloorCeilTest {
    public static void main() {
        float f1 = 3.7f;
        float f2 = -2.3f;
        
        float floor1 = Math.floor(f1);  // 3.0
        float ceil1 = Math.ceil(f1);    // 4.0
        float floor2 = Math.floor(f2);  // -3.0
        float ceil2 = Math.ceil(f2);    // -2.0
        
        System.out.println(floor1);
        System.out.println(ceil1);
    }
}
```

### 実装ステップ

1. **AST拡張** (1時間)
   - NODE_CAST追加
   - CastData構造体定義

2. **Parser実装** (2時間)
   - 明示的キャスト構文のパース
   - 型トークン判定

3. **Semantic Analyzer実装** (2時間)
   - 暗黙的キャスト挿入
   - 型互換性チェック
   - キャスト妥当性検証

4. **Code Generator実装** (1時間)
   - キャストオペコード生成
   - 型変換ロジック

5. **VM実装** (1時間)
   - 新オペコード実装
   - 型変換処理

6. **Math関数追加** (1時間)
   - floor/ceil実装（native.c）

7. **テスト** (2時間)
   - 単体テスト作成
   - 統合テスト実行

**推定工数**: 10時間

---

## Phase 9.2: Math Integer Arguments Support

### 目標
Math関数で整数引数を受け付けられるようにし、使いやすさを向上させる。

### 実装内容

#### 対象関数
- `Math.abs(int)` / `Math.abs(long)`
- `Math.min(int, int)` / `Math.min(long, long)`
- `Math.max(int, int)` / `Math.max(long, long)`
- `Math.pow(int, int)` (オプション)

#### 実装戦略
内部でfloatにキャストして既存実装を呼び出す：

```c
/* native.c */
/* Math.abs(int) */
case NATIVE_MATH_ABS_INT: {
    int16_t i = stack_pop_int(stack);
    float f = (float)i;
    float result = fabsf(f);
    int16_t result_int = (int16_t)result;
    stack_push_int(stack, result_int);
    break;
}

/* Math.abs(long) */
case NATIVE_MATH_ABS_LONG: {
    int32_t l = stack_pop_long(stack);
    float f = (float)l;
    float result = fabsf(f);
    int32_t result_long = (int32_t)result;
    stack_push_long(stack, result_long);
    break;
}
```

### Semantic Analyzer拡張

メソッドオーバーロード解決：
```c
/* semantic.c - resolve_method_call() */
// Find best matching method based on argument types
MethodInfo* find_best_method(const char* name, TypeInfo* arg_types, int arg_count) {
    // 1. Exact match
    // 2. Widening conversion match
    // 3. Error if no match
}
```

### テスト計画

```java
class MathIntTest {
    public static void main() {
        int i1 = -10;
        int i2 = 5;
        long l1 = -1000L;
        
        int abs_i = Math.abs(i1);      // 10
        long abs_l = Math.abs(l1);     // 1000L
        int min = Math.min(i1, i2);    // -10
        int max = Math.max(i1, i2);    // 5
        
        System.out.println(abs_i);
        System.out.println(abs_l);
    }
}
```

### 実装ステップ

1. **Native関数追加** (2時間)
   - 整数版Math関数実装
   - オペコード追加

2. **Semantic Analyzer拡張** (2時間)
   - メソッドオーバーロード解決
   - 引数型マッチング

3. **Code Generator更新** (1時間)
   - 適切なnative関数呼び出し生成

4. **テスト** (1時間)
   - 整数引数テスト
   - 型混在テスト

**推定工数**: 6時間

---

## Phase 9.3: null Literal Support

### 目標
`null`リテラルをサポートし、参照型の初期化や条件判定を可能にする。

### 実装内容

#### 1. null Literal
```java
String s = null;
Date d = null;
int[] arr = null;
```

#### 2. null Check
```java
if (s == null) {
    System.out.println("String is null");
}

if (arr != null) {
    int len = arr.length;
}
```

#### 3. null in Conditions
```java
while (s != null) {
    // ...
}

// null is treated as false
if (null) {  // Always false
    // Never executed
}
```

### 技術設計

#### Lexer拡張
```c
/* lexer.c */
{"null", TOK_NULL},
```

#### AST拡張
```c
/* ast.h */
typedef enum {
    NODE_LITERAL_NULL,   /* null literal */
} NodeType;
```

#### Parser拡張
```c
/* parser.c - parse_primary() */
if (parser_match(parser, TOK_NULL)) {
    node = parser_alloc_node(parser, NODE_LITERAL_NULL);
    return node;
}
```

#### Semantic Analyzer拡張
```c
/* semantic.c */
// null can be assigned to any reference type
if (expr_type == TYPE_NULL) {
    if (is_reference_type(target_type)) {
        return 1;  // Compatible
    }
}

// null comparison
if (left_type == TYPE_NULL || right_type == TYPE_NULL) {
    if (is_reference_type(left_type) || is_reference_type(right_type)) {
        return TYPE_BOOLEAN;
    }
}
```

#### Code Generator
```c
/* codegen.c */
case NODE_LITERAL_NULL:
    emit_opcode(gen, OP_PUSH_NULL);  /* Push 0 (null pointer) */
    break;
```

#### VM Implementation
```c
/* interpreter.c */
case OP_PUSH_NULL:
    stack_push_int(stack, 0);  /* null = 0 */
    break;
```

### 制限事項

1. **NullPointerException未実装**
   - null参照のメンバーアクセスは未定義動作
   - 将来の例外処理実装で対応

2. **null安全性チェックなし**
   - コンパイル時のnullチェックは行わない
   - プログラマの責任

3. **プリミティブ型には使用不可**
   ```java
   int i = null;  // Error
   ```

### テスト計画

#### Test 1: null Assignment
```java
class NullAssignTest {
    public static void main() {
        String s = null;
        int[] arr = null;
        
        if (s == null && arr == null) {
            System.out.println("null assignment works!");
        }
    }
}
```

#### Test 2: null Comparison
```java
class NullCompareTest {
    public static void main() {
        String s1 = null;
        String s2 = "hello";
        
        if (s1 == null) {
            System.out.println("s1 is null");
        }
        
        if (s2 != null) {
            System.out.println("s2 is not null");
        }
    }
}
```

#### Test 3: null in Conditions
```java
class NullConditionTest {
    public static void main() {
        String s = null;
        
        // null is false
        if (null) {
            System.out.println("ERROR: Should not print");
        }
        
        // null check in while
        int count = 0;
        while (s == null && count < 3) {
            count = count + 1;
        }
        
        System.out.println(count);  // Should print 3
    }
}
```

### 実装ステップ

1. **Lexer拡張** (30分)
   - TOK_NULL追加

2. **AST拡張** (30分)
   - NODE_LITERAL_NULL追加

3. **Parser実装** (1時間)
   - nullリテラルのパース

4. **Semantic Analyzer実装** (3時間)
   - null型の導入
   - 参照型との互換性チェック
   - null比較の型チェック

5. **Code Generator実装** (1時間)
   - OP_PUSH_NULL生成

6. **VM実装** (30分)
   - OP_PUSH_NULL処理

7. **テスト** (2時間)
   - null代入テスト
   - null比較テスト
   - 条件式テスト

**推定工数**: 8.5時間

---

## 全体スケジュール

### タイムライン

| Phase | 機能 | 工数 | 期間 |
|-------|------|------|------|
| 9.1 | Type Casting | 10時間 | 2-3日 |
| 9.2 | Math Integer Args | 6時間 | 1-2日 |
| 9.3 | null Literal | 8.5時間 | 2日 |
| **合計** | | **24.5時間** | **5-7日** |

### マイルストーン

- **Day 1-3**: Phase 9.1完了
  - キャスト機能実装
  - Math.floor/ceil追加
  - テスト合格

- **Day 4-5**: Phase 9.2完了
  - Math整数引数サポート
  - オーバーロード解決
  - テスト合格

- **Day 6-7**: Phase 9.3完了
  - nullリテラル実装
  - null安全性基本対応
  - テスト合格

---

## 依存関係

```
Phase 9.1 (Type Casting)
    ↓
    ├─→ Phase 9.2 (Math Integer Args)
    │   - キャスト機能を使用
    │   - 整数→float変換
    │
    └─→ Phase 9.3 (null Literal)
        - 参照型キャストでnull処理
```

---

## リスク管理

### 高リスク項目

1. **型システムの複雑化**
   - 対策: 段階的実装、十分なテスト

2. **オーバーフロー/アンダーフロー**
   - 対策: ドキュメント化、テストケース追加

3. **null参照の未定義動作**
   - 対策: 制限事項を明記、将来の例外処理で対応

### 中リスク項目

1. **メソッドオーバーロード解決**
   - 対策: シンプルなルール、明確なエラーメッセージ

2. **既存コードへの影響**
   - 対策: リグレッションテスト、後方互換性維持

---

## 成功基準

### Phase 9.1
- ✅ 暗黙的キャスト（int→long, int→float, long→float）動作
- ✅ 明示的キャスト（long→int, float→int）動作
- ✅ Math.floor/ceil実装
- ✅ cast.javテスト合格

### Phase 9.2
- ✅ Math.abs(int/long)動作
- ✅ Math.min/max(int/long)動作
- ✅ オーバーロード解決正常動作
- ✅ 整数引数テスト合格

### Phase 9.3
- ✅ nullリテラル使用可能
- ✅ null比較動作
- ✅ 条件式でnull使用可能
- ✅ nullテスト合格

### 全体
- ✅ すべてのテスト合格
- ✅ 既存テストにリグレッションなし
- ✅ ドキュメント更新完了
- ✅ README.md更新

---

## ドキュメント

### 作成予定ドキュメント

1. `PHASE9_1_CASTING_COMPLETION.md` - キャスト実装完了報告
2. `PHASE9_2_MATH_INT_COMPLETION.md` - Math整数引数完了報告
3. `PHASE9_3_NULL_COMPLETION.md` - null実装完了報告
4. `PHASE9_COMPLETION.md` - Phase 9全体完了報告

### 更新予定ドキュメント

1. `README.md` - Phase 9機能追加
2. `PROGRESS.md` - 進捗更新
3. `AGENTS.md` - 新機能情報追加

---

## 参考資料

- Java Language Specification - Type Conversion
- Java Language Specification - null Type
- IEEE 754 - Floating Point Standard
- Open Watcom C Compiler Documentation

---

**Document Version**: 1.0  
**Created**: 2026-05-26  
**Status**: Planning Phase  
**Next Action**: Phase 9.1 Implementation