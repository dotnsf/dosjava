# Phase 10: Variable Initialization & Exception Enhancement Plan

## Overview

Phase 10では、2つの重要な機能を実装します：
1. **Phase 10.1**: 変数宣言時の初期化サポート (`int a = 1;`)
2. **Phase 10.2**: Exception変数の出力機能 (`System.out.println("" + e)`)

## Phase 10.1: Variable Declaration with Initialization

### 目的
変数宣言と同時に値を代入できるようにする。現在は`int a; a = 1;`のように2行に分ける必要があるが、`int a = 1;`のように1行で書けるようにする。

### 現状分析

#### 既に実装済みの部分
- **Parser** ([`parser.c:856-901`](dosjava/tools/compiler/parser.c:856-901)): 初期化式の解析は完全に実装済み
  ```c
  if (parser_consume(parser, TOK_ASSIGN)) {
      init_expr = parse_expression(parser);
  }
  ```

- **Semantic Analyzer** ([`semantic.c:1665-1743`](dosjava/tools/compiler/semantic.c:1665-1743)): 初期化式の型チェックは実装済み
  ```c
  if (init_expr_index != 0) {
      init_expr = semantic_get_node(analyzer, init_expr_index);
      if (init_expr) {
          if (check_expression(analyzer, init_expr, &init_type) == 0) {
              if (!types_compatible(var_type, init_type)) {
                  semantic_error(...);
              }
          }
      }
  }
  ```

#### 実装が必要な部分
- **Code Generator** ([`codegen.c:562-563`](dosjava/tools/compiler/codegen.c:562-563)): `generate_var_decl()`関数で初期化式のコード生成が未実装

### 実装手順

#### Step 1: Code Generatorの修正
**ファイル**: `dosjava/tools/compiler/codegen.c`

`generate_var_decl()`関数を修正して、初期化式がある場合にコード生成を行う：

```c
int generate_var_decl(CodeGenerator* codegen, ASTNode* var_node) {
    TypeInfo var_type;
    uint16_t init_expr_idx;
    
    // 変数情報を取得
    var_type = var_node->data.var_decl.type;
    init_expr_idx = var_node->data.var_decl.init_expr;
    
    // 初期化式がある場合
    if (init_expr_idx != 0) {
        ASTNode* init_expr = codegen_get_node(codegen, init_expr_idx);
        if (init_expr) {
            ASTNode init_copy;
            memcpy(&init_copy, init_expr, sizeof(ASTNode));
            
            // 初期化式のコード生成
            if (generate_expression(codegen, &init_copy) != 0) {
                return -1;
            }
            
            // 変数への代入コード生成
            // 型に応じて適切なSTORE命令を発行
            if (var_type.kind == TYPE_FLOAT) {
                emit_opcode(codegen, OP_STORE_FLOAT);
                emit_u1(codegen, local_index);
                update_stack(codegen, -2);
            } else if (var_type.kind == TYPE_LONG) {
                emit_opcode(codegen, OP_STORE_LONG);
                emit_u1(codegen, local_index);
                update_stack(codegen, -2);
            } else {
                // int, boolean, String
                if (local_index <= 2) {
                    emit_opcode(codegen, OP_STORE_0 + local_index);
                } else {
                    emit_opcode(codegen, OP_STORE_LOCAL);
                    emit_u1(codegen, local_index);
                }
                update_stack(codegen, -1);
            }
        }
    }
    
    return 0;
}
```

#### Step 2: テストケースの作成
**ファイル**: `dosjava/tests/varinit.jav`

すべての型での初期化をテスト：

```java
class VarInit {
    public static void main() {
        // int型
        int a = 42;
        System.out.println(a);
        
        // long型
        long b = 123456L;
        System.out.println(b);
        
        // float型
        float c = 3.14f;
        System.out.println(c);
        
        // boolean型
        boolean d = true;
        System.out.println(d);
        
        // String型
        String e = "Hello";
        System.out.println(e);
        
        // 式での初期化
        int f = 10 + 20;
        System.out.println(f);
        
        // 変数での初期化
        int g = a;
        System.out.println(g);
    }
}
```

#### Step 3: ドキュメント更新
- `README.md`: 変数宣言時の初期化機能を追加
- `PHASE10_COMPLETION.md`: 実装完了レポート作成

### 期待される動作

**入力**:
```java
int a = 1;
System.out.println(a);
```

**出力**:
```
1
```

---

## Phase 10.2: Exception Variable Output

### 目的
try-catch構文でcatchしたException変数を出力できるようにする。具体的には`System.out.println("" + e)`のような形でException変数の内容を表示する。

### 現状分析

#### 既存の実装
- **Parser**: catch節でException変数名を解析済み ([`parser.c:1231-1237`](dosjava/tools/compiler/parser.c:1231-1237))
- **VM**: 基本的なtry-catch制御フロー実装済み ([`interpreter.c:3172-3190`](dosjava/src/vm/interpreter.c:3172-3190))
- **Exception handling**: `throw_runtime_exception()`でエラーメッセージ出力 ([`interpreter.c:379-390`](dosjava/src/vm/interpreter.c:379-390))

#### 問題点
1. Exception変数にメッセージが保存されていない
2. Exceptionオブジェクトとしての実装が不完全
3. String変換機能が未実装

### 実装手順

#### Step 1: Exception構造体の定義
**新規ファイル**: `dosjava/src/vm/exception.h`

```c
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdint.h>

/* Exception object structure */
typedef struct {
    char message[256];  /* Exception message */
    uint16_t type;      /* Exception type (for future extension) */
} ExceptionObject;

/* Create exception object */
ExceptionObject* exception_create(const char* message);

/* Free exception object */
void exception_free(ExceptionObject* ex);

/* Get exception message */
const char* exception_get_message(ExceptionObject* ex);

#endif /* EXCEPTION_H */
```

**新規ファイル**: `dosjava/src/vm/exception.c`

```c
#include "exception.h"
#include <stdlib.h>
#include <string.h>

ExceptionObject* exception_create(const char* message) {
    ExceptionObject* ex = (ExceptionObject*)malloc(sizeof(ExceptionObject));
    if (ex) {
        memset(ex, 0, sizeof(ExceptionObject));
        if (message) {
            strncpy(ex->message, message, 255);
            ex->message[255] = '\0';
        }
        ex->type = 0;
    }
    return ex;
}

void exception_free(ExceptionObject* ex) {
    if (ex) {
        free(ex);
    }
}

const char* exception_get_message(ExceptionObject* ex) {
    return ex ? ex->message : "";
}
```

#### Step 2: VMでのException変数管理
**ファイル**: `dosjava/src/vm/interpreter.c`

1. `ExecutionContext`にException変数用のフィールド追加：
```c
typedef struct {
    // ... existing fields ...
    ExceptionObject* current_exception;  /* Current exception object */
    uint16_t exception_var_index;        /* Local variable index for exception */
} ExecutionContext;
```

2. `throw_runtime_exception()`の修正：
```c
static int throw_runtime_exception(ExecutionContext* ctx, const char* message) {
    if (ctx->in_try_block && ctx->catch_pc) {
        /* Create exception object */
        ctx->current_exception = exception_create(message);
        
        /* Store exception object reference in catch variable */
        if (ctx->exception_var_index < MAX_LOCALS) {
            ctx->locals[ctx->exception_var_index] = (uint16_t)(uintptr_t)ctx->current_exception;
        }
        
        /* Jump to catch block */
        ctx->pc = ctx->catch_pc;
        ctx->in_try_block = 0;
        return 0;
    } else {
        printf("ERROR: %s\n", message);
        return -1;
    }
}
```

3. `OP_THROW`の修正：
```c
case OP_THROW:
    /* Pop exception value (string message or object) */
    if (ctx->stack_pointer > 0) {
        uint16_t ex_ref = stack_pop_shared(ctx);
        
        if (ctx->catch_pc != NULL) {
            /* Create exception from message */
            const char* msg = (const char*)(uintptr_t)ex_ref;
            ctx->current_exception = exception_create(msg);
            
            /* Store in exception variable */
            if (ctx->exception_var_index < MAX_LOCALS) {
                ctx->locals[ctx->exception_var_index] = (uint16_t)(uintptr_t)ctx->current_exception;
            }
            
            ctx->pc = ctx->catch_pc;
            ctx->catch_pc = NULL;
        } else {
            printf("Unhandled exception\n");
            ctx->running = 0;
            return 1;
        }
    }
    break;
```

#### Step 3: Code GeneratorでのException変数処理
**ファイル**: `dosjava/tools/compiler/codegen.c`

1. `OP_TRY`生成時にException変数インデックスを記録
2. catch節でException変数をローカル変数として登録

#### Step 4: String連結でのException処理
**ファイル**: `dosjava/tools/compiler/semantic.c`

String連結時にException型を検出し、toString()呼び出しを挿入：

```c
/* Check if operand is Exception type */
if (left_type.kind == TYPE_CLASS) {
    const char* class_name = semantic_get_string(analyzer, left_type.class_name);
    if (class_name && strcmp(class_name, "Exception") == 0) {
        /* Exception + String requires toString() */
        // Mark for special handling in codegen
    }
}
```

**ファイル**: `dosjava/tools/compiler/codegen.c`

Exception変数の文字列連結時に特殊処理：

```c
/* If left operand is Exception, call getMessage() */
if (is_exception_type(left_node)) {
    /* Load exception object */
    generate_expression(codegen, left_node);
    
    /* Call exception_get_message() */
    emit_opcode(codegen, OP_INVOKE_NATIVE);
    emit_u2(codegen, NATIVE_EXCEPTION_GET_MESSAGE);
    
    /* Result is string pointer */
}
```

#### Step 5: Native関数の追加
**ファイル**: `dosjava/src/vm/interpreter.c`

Exception.getMessage()相当のネイティブ関数を追加：

```c
case NATIVE_EXCEPTION_GET_MESSAGE: {
    uint16_t ex_ref = stack_pop_shared(ctx);
    ExceptionObject* ex = (ExceptionObject*)(uintptr_t)ex_ref;
    const char* msg = exception_get_message(ex);
    stack_push_shared(ctx, (uint16_t)(uintptr_t)msg);
    break;
}
```

#### Step 6: テストケースの作成
**ファイル**: `dosjava/tests/excprint.jav`

```java
class ExcPrint {
    public static void main() {
        try {
            int a = 10;
            int b = 0;
            int c = a / b;  // Division by zero
        } catch (Exception e) {
            System.out.println("Caught: " + e);
        }
        
        System.out.println("Done");
    }
}
```

**期待される出力**:
```
Caught: Division by zero
Done
```

#### Step 7: ドキュメント更新
- `README.md`: Exception出力機能を追加
- `PHASE10_COMPLETION.md`: 実装完了レポート作成

---

## 実装順序とマイルストーン

### Milestone 1: Phase 10.1 完了（推定2-4時間）
- [ ] Code Generatorの`generate_var_decl()`修正
- [ ] テストケース作成と検証
- [ ] ドキュメント更新

### Milestone 2: Phase 10.2 基礎実装（推定4-6時間）
- [ ] Exception構造体定義
- [ ] VMでのException変数管理
- [ ] `throw_runtime_exception()`修正

### Milestone 3: Phase 10.2 String連結（推定2-3時間）
- [ ] Semantic analyzerでのException型検出
- [ ] Code generatorでの特殊処理
- [ ] Native関数追加

### Milestone 4: Phase 10.2 完了（推定2-3時間）
- [ ] テストケース作成と検証
- [ ] ドキュメント更新
- [ ] 統合テスト

---

## リスクと対策

### Phase 10.1のリスク
- **リスク**: 既存のコード生成ロジックとの競合
- **対策**: 既存の代入文生成コードを参考にする

### Phase 10.2のリスク
- **リスク**: メモリ管理の複雑化（Exception objectのライフサイクル）
- **対策**: シンプルな実装から始め、必要に応じて拡張

- **リスク**: String連結の複雑な型チェック
- **対策**: Exception型の特殊ケースとして明示的に処理

---

## 成功基準

### Phase 10.1
- すべての基本型（int, long, float, boolean, String）で初期化が動作
- 式での初期化が動作
- 既存のテストが引き続き動作

### Phase 10.2
- Exception変数が正しくcatch節で受け取れる
- `System.out.println("" + e)`でExceptionメッセージが出力される
- 既存のException処理が引き続き動作

---

## 次のステップ

1. Phase 10.1の実装開始
2. Phase 10.1のテストと検証
3. Phase 10.2の実装開始
4. Phase 10.2のテストと検証
5. 統合テストと最終検証