# Control Flow Implementation Plan
## 制御構造実装計画書

**作成日**: 2026-04-27
**目標**: if/else、while、算術演算を実装し、examples/cond.jav と loop.jav を実行可能にする

**注意**: 16bit PC-DOSのファイル名制約（8.3形式）に準拠しています

---

## 📋 現状分析

### ✅ 既に実装済み
1. **基本インフラ**
   - メモリ管理 (memory.c/h)
   - スタック管理 (stack.c/h)
   - バイトコードフォーマット (djc.c/h)
   - インタープリタ基盤 (interpreter.c/h)

2. **コンパイラ基盤**
   - レキサー (lexer.c/h) - トークン定義済み
   - パーサー (parser.c/h) - AST構造定義済み
   - セマンティック解析 (semantic.c/h)
   - コード生成 (codegen.c/h)

3. **オペコード定義**
   - 算術演算: `OP_ADD`, `OP_SUB`, `OP_MUL`, `OP_DIV`, `OP_MOD`, `OP_NEG`
   - 比較演算: `OP_CMP_EQ`, `OP_CMP_NE`, `OP_CMP_LT`, `OP_CMP_LE`, `OP_CMP_GT`, `OP_CMP_GE`
   - 制御フロー: `OP_GOTO`, `OP_IF_TRUE`, `OP_IF_FALSE`
   - ローカル変数: `OP_LOAD_LOCAL`, `OP_STORE_LOCAL`

4. **動作確認済み**
   - メソッド呼び出し (`INVOKE_STATIC`, `RETURN`)
   - 文字列リテラル (`PUSH_CONST`)
   - `System.out.println()` (ネイティブメソッド)

### ❌ 未実装の機能
1. **パーサー**: 制御構造のパース処理（スタブのみ）
2. **セマンティック解析**: 型チェック、変数スコープ管理
3. **コード生成**: 制御構造のバイトコード生成
4. **インタープリタ**: 算術・比較演算の実行

---

## 🎯 実装目標

### 最終目標プログラム
```java
class SimpleTest {
    public static void main() {
        int x = 10;
        int y = 20;
        int sum = 0;
        
        // 算術演算
        sum = x + y;  // sum = 30
        
        // if文
        if (sum > 25) {
            sum = sum + 5;  // sum = 35
        }
        
        // while文
        int i = 0;
        while (i < 3) {
            sum = sum + 1;
            i = i + 1;
        }
        // sum = 38
        
        System.out.println(sum);
    }
}
```

---

## 📐 実装フェーズ

### Phase 1: 現状分析と設計 ✓
**期間**: 1日  
**成果物**: この計画書

**タスク**:
- [x] 既存コードの調査
- [x] AST構造の確認
- [x] オペコード定義の確認
- [x] 実装計画の策定

---

### Phase 2: ローカル変数のサポート実装
**期間**: 2-3日  
**優先度**: 🔴 最高

#### 2.1 パーサー拡張
**ファイル**: `tools/compiler/parser.c`

**実装内容**:
```c
// 変数宣言のパース
uint16_t parse_var_decl(Parser* parser) {
    // int x = 10;
    // int y;
}

// 代入文のパース  
uint16_t parse_assignment(Parser* parser) {
    // x = 10;
    // y = x + 5;
}
```

**必要な機能**:
- 型指定子の認識 (`int`, `boolean`)
- 変数名の取得
- 初期化式のパース（オプション）
- 代入演算子 `=` の処理

#### 2.2 セマンティック解析
**ファイル**: `tools/compiler/semantic.c`

**実装内容**:
```c
// 変数スコープ管理
typedef struct {
    char name[32];
    TypeInfo type;
    uint16_t local_index;  // ローカル変数インデックス
} Variable;

// シンボルテーブルに変数を追加
int add_local_variable(SemanticAnalyzer* analyzer, 
                       const char* name, 
                       TypeInfo type);

// 変数の検索
Variable* find_variable(SemanticAnalyzer* analyzer, 
                        const char* name);
```

**必要な機能**:
- ローカル変数のシンボルテーブル管理
- 変数の重複宣言チェック
- 未宣言変数の使用チェック
- 型の互換性チェック

#### 2.3 コード生成
**ファイル**: `tools/compiler/codegen.c`

**実装内容**:
```c
// 変数宣言のコード生成
int generate_var_decl(CodeGenerator* gen, ASTNode* node) {
    // 初期化式があれば評価
    // STORE_LOCAL命令を生成
}

// 代入文のコード生成
int generate_assignment(CodeGenerator* gen, ASTNode* node) {
    // 右辺の式を評価（スタックにプッシュ）
    // STORE_LOCAL命令を生成
}

// 変数参照のコード生成
int generate_identifier(CodeGenerator* gen, ASTNode* node) {
    // LOAD_LOCAL命令を生成
}
```

**生成するバイトコード例**:
```
int x = 10;
→ PUSH_INT 10
  STORE_LOCAL 0

y = x + 5;
→ LOAD_LOCAL 0
  PUSH_INT 5
  ADD
  STORE_LOCAL 1
```

#### 2.4 インタープリタ確認
**ファイル**: `src/vm/interpreter.c`

**確認項目**:
- `OP_LOAD_LOCAL` の実装状況
- `OP_STORE_LOCAL` の実装状況
- ローカル変数配列のサイズ

---

### Phase 3: 算術演算子の実装
**期間**: 2日  
**優先度**: 🔴 最高

#### 3.1 パーサー拡張
**ファイル**: `tools/compiler/parser.c`

**実装内容**:
```c
// 二項演算式のパース（演算子優先順位を考慮）
uint16_t parse_expression(Parser* parser) {
    return parse_logical_or(parser);
}

uint16_t parse_logical_or(Parser* parser);    // ||
uint16_t parse_logical_and(Parser* parser);   // &&
uint16_t parse_equality(Parser* parser);      // ==, !=
uint16_t parse_relational(Parser* parser);    // <, <=, >, >=
uint16_t parse_additive(Parser* parser);      // +, -
uint16_t parse_multiplicative(Parser* parser); // *, /, %
uint16_t parse_unary(Parser* parser);         // -, !
uint16_t parse_primary(Parser* parser);       // リテラル、変数、()
```

**演算子優先順位**:
1. `*`, `/`, `%` (乗除算)
2. `+`, `-` (加減算)
3. `<`, `<=`, `>`, `>=` (比較)
4. `==`, `!=` (等価)
5. `&&` (論理AND)
6. `||` (論理OR)

#### 3.2 セマンティック解析
**ファイル**: `tools/compiler/semantic.c`

**実装内容**:
```c
// 二項演算の型チェック
int check_binary_op(SemanticAnalyzer* analyzer, 
                    ASTNode* node, 
                    TypeInfo* result_type) {
    // 左辺と右辺の型を取得
    // 演算子に応じた型チェック
    // 結果の型を決定
}
```

**型ルール**:
- `int + int → int`
- `int - int → int`
- `int * int → int`
- `int / int → int`
- `int % int → int`
- `int < int → boolean`
- `boolean && boolean → boolean`

#### 3.3 コード生成
**ファイル**: `tools/compiler/codegen.c`

**実装内容**:
```c
// 二項演算のコード生成
int generate_binary_op(CodeGenerator* gen, ASTNode* node) {
    // 左辺を評価（スタックにプッシュ）
    // 右辺を評価（スタックにプッシュ）
    // 演算子に応じたオペコードを生成
}
```

**生成するバイトコード例**:
```
x + y
→ LOAD_LOCAL 0  // x
  LOAD_LOCAL 1  // y
  ADD

x * y + z
→ LOAD_LOCAL 0  // x
  LOAD_LOCAL 1  // y
  MUL
  LOAD_LOCAL 2  // z
  ADD
```

#### 3.4 インタープリタ実装
**ファイル**: `src/vm/interpreter.c`

**実装内容**:
```c
case OP_ADD: {
    int16_t b = stack_pop_shared(ctx);
    int16_t a = stack_pop_shared(ctx);
    stack_push_shared(ctx, a + b);
    break;
}

case OP_SUB: {
    int16_t b = stack_pop_shared(ctx);
    int16_t a = stack_pop_shared(ctx);
    stack_push_shared(ctx, a - b);
    break;
}

// MUL, DIV, MOD, NEG も同様に実装
```

---

### Phase 4: 比較演算子の実装
**期間**: 1-2日  
**優先度**: 🟡 高

#### 4.1 コード生成
**ファイル**: `tools/compiler/codegen.c`

**実装内容**:
```c
// 比較演算のコード生成
int generate_comparison(CodeGenerator* gen, ASTNode* node) {
    // 左辺を評価
    // 右辺を評価
    // 比較オペコードを生成
}
```

**生成するバイトコード例**:
```
x < y
→ LOAD_LOCAL 0
  LOAD_LOCAL 1
  CMP_LT

x == 10
→ LOAD_LOCAL 0
  PUSH_INT 10
  CMP_EQ
```

#### 4.2 インタープリタ実装
**ファイル**: `src/vm/interpreter.c`

**実装内容**:
```c
case OP_CMP_EQ: {
    int16_t b = stack_pop_shared(ctx);
    int16_t a = stack_pop_shared(ctx);
    stack_push_shared(ctx, (a == b) ? 1 : 0);
    break;
}

case OP_CMP_LT: {
    int16_t b = stack_pop_shared(ctx);
    int16_t a = stack_pop_shared(ctx);
    stack_push_shared(ctx, (a < b) ? 1 : 0);
    break;
}

// NE, LE, GT, GE も同様に実装
```

---

### Phase 5: if/else文の実装
**期間**: 2-3日  
**優先度**: 🔴 最高

#### 5.1 パーサー実装
**ファイル**: `tools/compiler/parser.c`

**実装内容**:
```c
static uint16_t parse_if_stmt(Parser* parser) {
    // if (condition) { ... }
    // if (condition) { ... } else { ... }
    
    // 1. 'if' キーワードを消費
    // 2. '(' を期待
    // 3. 条件式をパース
    // 4. ')' を期待
    // 5. then節（ブロック）をパース
    // 6. 'else' があればelse節をパース
    // 7. ASTノードを作成
}
```

**AST構造** (既に定義済み):
```c
struct {
    uint16_t condition;   // 条件式のノードインデックス
    uint16_t then_stmt;   // then節のノードインデックス
    uint16_t else_stmt;   // else節のノードインデックス（0=なし）
} if_stmt;
```

#### 5.2 コード生成
**ファイル**: `tools/compiler/codegen.c`

**実装内容**:
```c
int generate_if_stmt(CodeGenerator* gen, ASTNode* node) {
    // 条件式を評価
    // IF_FALSE命令（else節またはif文の終わりへジャンプ）
    // then節のコード生成
    // else節がある場合:
    //   - GOTO命令（if文の終わりへ）
    //   - else節のコード生成
    // ジャンプ先のアドレスをバックパッチ
}
```

**生成するバイトコード例**:
```java
if (x > 5) {
    y = 10;
}
```
↓
```
LOAD_LOCAL 0      // x
PUSH_INT 5
CMP_GT
IF_FALSE +8       // then節をスキップ
PUSH_INT 10
STORE_LOCAL 1     // y
```

```java
if (x > 5) {
    y = 10;
} else {
    y = 20;
}
```
↓
```
LOAD_LOCAL 0      // x
PUSH_INT 5
CMP_GT
IF_FALSE +10      // else節へジャンプ
PUSH_INT 10
STORE_LOCAL 1     // y
GOTO +7           // if文の終わりへ
PUSH_INT 20       // else節
STORE_LOCAL 1     // y
```

#### 5.3 インタープリタ実装
**ファイル**: `src/vm/interpreter.c`

**実装内容**:
```c
case OP_IF_FALSE: {
    int16_t offset;
    int16_t condition = stack_pop_shared(ctx);
    
    // オフセットを読み取り（2バイト）
    offset = (int16_t)((ctx->pc[0] << 8) | ctx->pc[1]);
    ctx->pc += 2;
    
    // 条件がfalse（0）ならジャンプ
    if (condition == 0) {
        ctx->pc += offset;
    }
    break;
}

case OP_GOTO: {
    int16_t offset;
    
    // オフセットを読み取り（2バイト）
    offset = (int16_t)((ctx->pc[0] << 8) | ctx->pc[1]);
    ctx->pc += 2;
    
    // 無条件ジャンプ
    ctx->pc += offset;
    break;
}
```

---

### Phase 6: while文の実装
**期間**: 2日  
**優先度**: 🟡 高

#### 6.1 パーサー実装
**ファイル**: `tools/compiler/parser.c`

**実装内容**:
```c
static uint16_t parse_while_stmt(Parser* parser) {
    // while (condition) { ... }
    
    // 1. 'while' キーワードを消費
    // 2. '(' を期待
    // 3. 条件式をパース
    // 4. ')' を期待
    // 5. ループ本体（ブロック）をパース
    // 6. ASTノードを作成
}
```

**AST構造** (既に定義済み):
```c
struct {
    uint16_t condition;   // 条件式のノードインデックス
    uint16_t body;        // ループ本体のノードインデックス
} while_stmt;
```

#### 6.2 コード生成
**ファイル**: `tools/compiler/codegen.c`

**実装内容**:
```c
int generate_while_stmt(CodeGenerator* gen, ASTNode* node) {
    // ループ開始位置を記録
    // 条件式を評価
    // IF_FALSE命令（ループ終了へジャンプ）
    // ループ本体のコード生成
    // GOTO命令（ループ開始へ戻る）
    // ジャンプ先のアドレスをバックパッチ
}
```

**生成するバイトコード例**:
```java
while (i < 10) {
    sum = sum + i;
    i = i + 1;
}
```
↓
```
[loop_start:]
LOAD_LOCAL 0      // i
PUSH_INT 10
CMP_LT
IF_FALSE +15      // ループ終了へ
LOAD_LOCAL 1      // sum
LOAD_LOCAL 0      // i
ADD
STORE_LOCAL 1     // sum
LOAD_LOCAL 0      // i
PUSH_INT 1
ADD
STORE_LOCAL 0     // i
GOTO -18          // loop_startへ戻る
[loop_end:]
```

#### 6.3 インタープリタ確認
- `OP_IF_FALSE` と `OP_GOTO` が既に実装されていれば、追加実装不要
- 負のオフセット（後方ジャンプ）が正しく動作することを確認

---

### Phase 7: テストと検証
**期間**: 2-3日  
**優先度**: 🟢 中

#### 7.1 単体テスト

**Test 1: ローカル変数**
```java
class VarTest {
    public static void main() {
        int x = 10;
        int y = 20;
        int z = x;
        System.out.println(z);  // 期待: 10
    }
}
```

**Test 2: 算術演算**
```java
class ArithTest {
    public static void main() {
        int a = 5;
        int b = 3;
        int sum = a + b;        // 8
        int diff = a - b;       // 2
        int prod = a * b;       // 15
        int quot = a / b;       // 1
        int rem = a % b;        // 2
        System.out.println(sum);
    }
}
```

**Test 3: 比較演算**
```java
class CompTest {
    public static void main() {
        int x = 10;
        int y = 20;
        int result = 0;
        
        if (x < y) {
            result = 1;
        }
        System.out.println(result);  // 期待: 1
    }
}
```

**Test 4: if/else文**
```java
class IfTest {
    public static void main() {
        int x = 15;
        int result = 0;
        
        if (x > 10) {
            result = 100;
        } else {
            result = 200;
        }
        System.out.println(result);  // 期待: 100
    }
}
```

**Test 5: while文**
```java
class WhileTest {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 5) {
            sum = sum + i;
            i = i + 1;
        }
        System.out.println(sum);  // 期待: 10 (0+1+2+3+4)
    }
}
```

#### 7.2 統合テスト

**examples/cond.jav の実行**
- 複数のif文
- ネストしたif文
- 論理演算子

**examples/loop.jav の実行**
- 基本的なwhileループ
- ネストしたループ

#### 7.3 エラーハンドリング

**テストケース**:
- 未宣言変数の使用
- 型の不一致
- 変数の重複宣言
- 無限ループの検出（オプション）

---

### Phase 8: ドキュメント更新
**期間**: 1日  
**優先度**: 🟢 中

#### 8.1 更新するドキュメント

1. **PROGRESS.md**
   - 実装完了した機能を記録
   - 新しいマイルストーンを追加

2. **QUICKSTART.md**
   - 新しい機能の使用例を追加
   - サンプルプログラムを更新

3. **TECHNICAL_SPEC.md**
   - 実装した機能の詳細を記録
   - バイトコード生成の例を追加

4. **新規ドキュメント作成**
   - `CONTROL_FLOW_GUIDE.md` - 制御構造の使い方ガイド
   - `VARIABLE_GUIDE.md` - 変数の使い方ガイド

---

## 🔧 技術的な課題と解決策

### 課題1: ジャンプオフセットの計算
**問題**: コード生成時にジャンプ先のアドレスが未確定

**解決策**: バックパッチング
```c
// 1. ジャンプ命令の位置を記録
uint16_t jump_pos = gen->code_length;
emit_byte(gen, OP_IF_FALSE);
emit_word(gen, 0);  // プレースホルダー

// 2. then節のコード生成
generate_statement(gen, then_stmt);

// 3. ジャンプオフセットを計算してパッチ
uint16_t offset = gen->code_length - jump_pos - 3;
gen->code[jump_pos + 1] = (offset >> 8) & 0xFF;
gen->code[jump_pos + 2] = offset & 0xFF;
```

### 課題2: 演算子の優先順位
**問題**: `2 + 3 * 4` を正しく `2 + (3 * 4)` として解釈

**解決策**: 再帰下降パーサー
```c
// 優先順位の低い演算子から順に処理
expression → logical_or
logical_or → logical_and (|| logical_and)*
logical_and → equality (&& equality)*
equality → relational ((== | !=) relational)*
relational → additive ((< | <= | > | >=) additive)*
additive → multiplicative ((+ | -) multiplicative)*
multiplicative → unary ((* | / | %) unary)*
unary → (- | !) unary | primary
primary → literal | identifier | ( expression )
```

### 課題3: ローカル変数のスコープ管理
**問題**: ブロック内の変数がブロック外で見えてはいけない

**解決策**: スコープスタック
```c
typedef struct {
    Variable variables[MAX_LOCALS];
    uint16_t count;
    uint16_t scope_depth;
} SymbolTable;

// ブロック開始時
void enter_scope(SymbolTable* table) {
    table->scope_depth++;
}

// ブロック終了時
void exit_scope(SymbolTable* table) {
    // 現在のスコープの変数を削除
    while (table->count > 0 && 
           table->variables[table->count - 1].scope_depth == table->scope_depth) {
        table->count--;
    }
    table->scope_depth--;
}
```

### 課題4: 16bitメモリ制約
**問題**: DOS環境では64KBのメモリ制限

**対策**:
- ASTノードを効率的に配置（パック構造体）
- 不要なデータの早期解放
- コンパイル時のメモリ使用量監視

---

## 📊 実装の優先順位

### 🔴 Critical Path (最優先)
1. ローカル変数 → 算術演算 → if文
   - これらがないと何もできない
   - 相互依存が強い

### 🟡 High Priority (高優先度)
2. 比較演算 → while文
   - if文の後に実装
   - 実用的なプログラムに必要

### 🟢 Medium Priority (中優先度)
3. テスト → ドキュメント
   - 品質保証
   - ユーザビリティ向上

---

## 📈 成功基準

### Minimum Viable Product (MVP)
- [ ] ローカル変数の宣言と使用
- [ ] 基本的な算術演算 (+, -, *, /)
- [ ] 単純なif文（else なし）
- [ ] 単純なwhileループ

### Full Feature Set
- [ ] すべての算術演算 (+, -, *, /, %, 単項-)
- [ ] すべての比較演算 (==, !=, <, <=, >, >=)
- [ ] 論理演算 (&&, ||, !)
- [ ] if/else文（ネスト対応）
- [ ] whileループ（ネスト対応）
- [ ] examples/cond.jav が動作
- [ ] examples/loop.jav が動作

---

## 🗓️ タイムライン

| Phase | 期間 | 開始日 | 完了予定日 |
|-------|------|--------|-----------|
| Phase 1: 現状分析 | 1日 | 2026-04-27 | 2026-04-27 |
| Phase 2: ローカル変数 | 3日 | 2026-04-28 | 2026-04-30 |
| Phase 3: 算術演算 | 2日 | 2026-05-01 | 2026-05-02 |
| Phase 4: 比較演算 | 2日 | 2026-05-03 | 2026-05-04 |
| Phase 5: if/else文 | 3日 | 2026-05-05 | 2026-05-07 |
| Phase 6: while文 | 2日 | 2026-05-08 | 2026-05-09 |
| Phase 7: テスト | 3日 | 2026-05-10 | 2026-05-12 |
| Phase 8: ドキュメント | 1日 | 2026-05-13 | 2026-05-13 |

**合計**: 約17日（2.5週間）

---

## 🎓 学習リソース

### 参考資料
1. **Crafting Interpreters** by Robert Nystrom
   - 演算子優先順位の実装
   - 制御フローの実装

2. **Java Virtual Machine Specification**
   - バイトコード命令の詳細
   - スタックベースの実行モデル

3. **Dragon Book (Compilers: Principles, Techniques, and Tools)**
   - セマンティック解析
   - コード生成

### 既存コードの参考箇所
- `tools/compiler/parser.c` - パーサーの基本構造
- `tools/compiler/codegen.c` - コード生成の基本パターン
- `src/vm/interpreter.c` - オペコード実行の実装例

---

## 📝 次のステップ

1. **Phase 2を開始**: ローカル変数のサポート実装
   - `parser.c` の `parse_var_decl()` を実装
   - `semantic.c` のシンボルテーブルを実装
   - `codegen.c` の変数関連コード生成を実装

2. **テストプログラムの作成**
   - 各フェーズで動作確認用の小さなテストプログラムを作成
   - DOSBoxで実行して動作を確認

3. **進捗の記録**
   - 各フェーズ完了時にこのドキュメントを更新
   - 問題が発生したら「技術的な課題」セクションに追記

---

**作成者**: Bob (AI Assistant)  
**レビュー**: 必要に応じて更新してください