# Phase 5.2 Day 10-11: Compiler Integration - Lexer & Parser

## 完了日時
2026-05-21

## 概要
Long型のコンパイラサポート（字句解析と構文解析）を実装しました。

## Day 10: 字句解析（Lexer）

### 変更ファイル

#### 1. tools/compiler/lexer.h
- `TOK_LONG` キーワードトークンを追加
- `TOK_LONG_LITERAL` リテラルトークンを追加
- `Token`構造体に`int32_t long_value`フィールドを追加

#### 2. tools/compiler/lexer.c
- キーワードテーブルに`"long"`を追加
- `lexer_read_number()`を修正してLong型リテラル（`100L`）をサポート
  - 数値の後に`L`または`l`サフィックスがあれば`TOK_LONG_LITERAL`
  - なければ`TOK_INTEGER`
- `token_type_name()`に`TOK_LONG`と`TOK_LONG_LITERAL`を追加

### 実装詳細

```c
// Long型リテラルの認識
if (lexer->current_char == 'L' || lexer->current_char == 'l') {
    token->type = TOK_LONG_LITERAL;
    token->value.long_value = (int32_t)value;
} else {
    token->type = TOK_INTEGER;
    token->value.int_value = (int16_t)value;
}
```

## Day 11: 構文解析と型システム（Parser & AST）

### 変更ファイル

#### 1. tools/compiler/ast.h
- `TypeKind`列挙型に`TYPE_LONG = 2`を追加
  - 既存の型番号を調整：`TYPE_BOOLEAN = 3`, `TYPE_CLASS = 4`, `TYPE_ARRAY = 5`
- `NodeType`列挙型に`NODE_LITERAL_LONG`を追加
- `ASTNode`構造体に`literal_long`フィールドを追加
  ```c
  struct {
      int32_t long_value;
  } literal_long;
  ```

#### 2. tools/compiler/parser.c
- `parse_type()`関数を修正してlong型をサポート
  ```c
  } else if (parser_consume(parser, TOK_LONG)) {
      base_kind = TYPE_LONG;
      type->class_name = 0;
  ```
- `parse_primary()`関数にLong型リテラルのパース処理を追加
  ```c
  if (parser_match(parser, TOK_LONG_LITERAL)) {
      node = parser_alloc_node(parser, NODE_LITERAL_LONG);
      parser->nodes[node - parser->total_nodes - 1].data.literal_long.long_value = 
          parser->current.value.long_value;
      parser_next_token(parser);
      return node;
  }
  ```
- `node_type_name()`に`NODE_LITERAL_LONG`を追加
- `type_kind_name()`に`TYPE_LONG`と`TYPE_ARRAY`を追加

### サポートされる構文

```java
// Long型変数宣言
long x;
long y = 100L;

// Long型リテラル
long a = 0L;
long b = 1000000L;
long c = -12345L;

// Long型演算（次のDay 12で実装）
long sum = a + b;
long diff = a - b;
long prod = a * b;
long quot = a / b;
long rem = a % b;
```

## テスト

### 字句解析テスト
```java
long x = 100L;
```
期待されるトークン列：
- TOK_LONG
- TOK_IDENTIFIER ("x")
- TOK_ASSIGN
- TOK_LONG_LITERAL (100)
- TOK_SEMICOLON

### 構文解析テスト
```java
class Test {
    public static void main() {
        long x = 100L;
        long y = 200L;
        long sum = x + y;
    }
}
```
期待されるAST：
- NODE_CLASS
  - NODE_METHOD
    - NODE_BLOCK
      - NODE_VAR_DECL (type=TYPE_LONG, name="x", init=NODE_LITERAL_LONG(100))
      - NODE_VAR_DECL (type=TYPE_LONG, name="y", init=NODE_LITERAL_LONG(200))
      - NODE_VAR_DECL (type=TYPE_LONG, name="sum", init=NODE_BINARY_OP(+))

## 次のステップ

### Day 12: コード生成（Codegen）
1. semantic.cでLong型の型チェックを実装
2. codegen.cでLong型のバイトコード生成を実装
   - `NODE_LITERAL_LONG` → `OP_PUSH_LONG`
   - Long型演算 → `OP_LADD`, `OP_LSUB`, etc.
   - 型変換 → `OP_I2L`, `OP_L2I`
   - ローカル変数 → `OP_LOAD_LONG`, `OP_STORE_LONG`

## ビルド状態
- ✅ lexer.c/h: コンパイル成功
- ✅ parser.c: コンパイル成功
- ✅ ast.h: コンパイル成功
- ⏳ semantic.c: 次のステップで更新
- ⏳ codegen.c: 次のステップで更新

## 互換性
- C89準拠
- 16-bit DOS環境対応
- Open Watcom C/C++コンパイラ対応