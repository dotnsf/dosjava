# Phase 2: Advanced Object-Oriented Features Plan

## Overview

Phase 1でオブジェクトの基本機能（オブジェクト生成、フィールドアクセス、インスタンスメソッド呼び出し）を実装しました。Phase 2では、より高度なオブジェクト指向機能を追加します。

## Phase 1 Recap (Completed)

✅ 実装済み機能：
- オブジェクト生成 (`new ClassName()`)
- フィールドアクセス (`object.field`)
- フィールド代入 (`object.field = value`)
- インスタンスメソッド呼び出し (`object.method(args)`)
- 暗黙的な `this` アクセス

## Phase 2 Goals

Phase 2では以下の機能を実装します：

### 2.1 コンストラクタ (Constructors)
- パラメータ付きコンストラクタ
- フィールドの初期化
- `this()` による他のコンストラクタ呼び出し

### 2.2 静的メンバー (Static Members)
- 静的フィールド (`static int count`)
- 静的メソッド (`static void method()`)
- クラス名によるアクセス (`ClassName.staticMethod()`)

### 2.3 配列とオブジェクトの統合
- オブジェクト配列 (`ClassName[] objects`)
- 配列要素へのオブジェクト代入
- 配列要素からのメソッド呼び出し

### 2.4 メソッドオーバーロード (Method Overloading)
- 同名メソッドの複数定義
- パラメータ型・数による識別
- 適切なメソッドの選択

## Implementation Plan

### Step 2.1: Constructors

#### 2.1.1 Parser Modifications
**File**: `tools/compiler/parser.c`

- コンストラクタ宣言の認識
  ```java
  ClassName(int param1, String param2) {
      this.field1 = param1;
      this.field2 = param2;
  }
  ```
- `this()` 呼び出しのパース
- デフォルトコンストラクタの自動生成

**AST Changes**:
- `NODE_CONSTRUCTOR` - コンストラクタ定義
- `NODE_CONSTRUCTOR_CALL` - `this()` 呼び出し

#### 2.1.2 Semantic Analysis
**File**: `tools/compiler/semantic.c`

- コンストラクタのシンボルテーブル登録
- パラメータ型チェック
- `this()` 呼び出しの検証（最初の文でなければならない）
- フィールド初期化の検証

#### 2.1.3 Code Generation
**File**: `tools/compiler/codegen.c`

- `OP_INVOKE_SPECIAL` の生成（コンストラクタ呼び出し用）
- `new` 式でのコンストラクタ自動呼び出し
- パラメータのスタック配置

**New Opcode**:
```c
#define OP_INVOKE_SPECIAL  0x42  /* Invoke constructor or private method */
```

#### 2.1.4 VM Implementation
**File**: `src/vm/interpreter.c`

- `OP_INVOKE_SPECIAL` の実装
- コンストラクタ実行時のオブジェクト初期化
- `this` 参照の適切な設定

### Step 2.2: Static Members

#### 2.2.1 Parser Modifications
**File**: `tools/compiler/parser.c`

- `static` キーワードの認識
- 静的フィールド宣言のパース
- 静的メソッド宣言のパース
- クラス名によるアクセスのパース (`ClassName.method()`)

**AST Changes**:
- `NODE_STATIC_FIELD` - 静的フィールド宣言
- `NODE_STATIC_METHOD` - 静的メソッド宣言
- `NODE_STATIC_ACCESS` - 静的メンバーアクセス

#### 2.2.2 Semantic Analysis
**File**: `tools/compiler/semantic.c`

- 静的メンバーのシンボルテーブル登録
- 静的コンテキストの検証（静的メソッド内でインスタンスメンバーにアクセス不可）
- クラス名による静的メンバーアクセスの検証

**Symbol Table Changes**:
```c
typedef struct {
    // ... existing fields ...
    int is_static;  /* 1 if static, 0 if instance */
} Symbol;
```

#### 2.2.3 Code Generation
**File**: `tools/compiler/codegen.c`

- `OP_GET_STATIC` の生成（静的フィールド読み取り）
- `OP_PUT_STATIC` の生成（静的フィールド書き込み）
- `OP_INVOKE_STATIC` の生成（静的メソッド呼び出し）

**New Opcodes**:
```c
#define OP_GET_STATIC      0x53  /* Get static field */
#define OP_PUT_STATIC      0x54  /* Put static field */
#define OP_INVOKE_STATIC   0x43  /* Invoke static method */
```

#### 2.2.4 VM Implementation
**File**: `src/vm/interpreter.c`

- 静的フィールド用のグローバルストレージ
- `OP_GET_STATIC` の実装
- `OP_PUT_STATIC` の実装
- `OP_INVOKE_STATIC` の実装（オブジェクト参照なし）

**Memory Layout**:
```c
/* Static field storage (per class) */
typedef struct {
    uint16_t class_id;
    uint16_t field_count;
    int32_t* fields;  /* Array of static field values */
} StaticFields;
```

### Step 2.3: Object Arrays

#### 2.3.1 Parser Modifications
**File**: `tools/compiler/parser.c`

- オブジェクト配列型の認識 (`ClassName[]`)
- オブジェクト配列生成のパース (`new ClassName[size]`)
- 配列要素へのオブジェクト代入
- 配列要素からのメソッド呼び出し

#### 2.3.2 Semantic Analysis
**File**: `tools/compiler/semantic.c`

- オブジェクト配列型のチェック
- 配列要素の型検証
- null チェック（将来の拡張）

#### 2.3.3 Code Generation
**File**: `tools/compiler/codegen.c`

- オブジェクト配列用の `OP_NEW_ARRAY` 生成
- 配列要素へのオブジェクト参照の格納
- 配列要素からのメソッド呼び出し

#### 2.3.4 VM Implementation
**File**: `src/vm/interpreter.c`

- オブジェクト配列のメモリ管理
- 配列要素へのオブジェクトハンドル格納
- 配列要素からのオブジェクト取得

### Step 2.4: Method Overloading

#### 2.4.1 Parser Modifications
**File**: `tools/compiler/parser.c`

- 同名メソッドの複数定義を許可
- パラメータリストの完全なパース

#### 2.4.2 Semantic Analysis
**File**: `tools/compiler/semantic.c`

- メソッドシグネチャの生成（名前 + パラメータ型）
- オーバーロードされたメソッドの識別
- 呼び出し時の適切なメソッド選択

**Method Signature**:
```c
/* Example: "add(II)" for add(int, int) */
char* generate_method_signature(const char* name, ASTNode* params);
```

#### 2.4.3 Code Generation
**File**: `tools/compiler/codegen.c`

- メソッドシグネチャに基づくメソッドインデックスの解決
- 適切なメソッドへの呼び出し生成

#### 2.4.4 VM Implementation
**File**: `src/vm/interpreter.c`

- メソッドテーブルでのシグネチャベースの検索
- 既存の `OP_INVOKE_VIRTUAL` で対応可能

## Test Cases

### Test 2.1: Constructor Test
**File**: `tests/const1.jav`
```java
class Point {
    int x;
    int y;
    
    Point(int px, int py) {
        x = px;
        y = py;
    }
    
    int getX() {
        return x;
    }
    
    int getY() {
        return y;
    }
}

class Main {
    public static void main() {
        Point p = new Point(10, 20);
        System.out.println(p.getX());  // Expected: 10
        System.out.println(p.getY());  // Expected: 20
    }
}
```

### Test 2.2: Static Members Test
**File**: `tests/static1.jav`
```java
class Counter {
    static int count = 0;
    
    static void increment() {
        count = count + 1;
    }
    
    static int getCount() {
        return count;
    }
}

class Main {
    public static void main() {
        Counter.increment();
        Counter.increment();
        System.out.println(Counter.getCount());  // Expected: 2
    }
}
```

### Test 2.3: Object Array Test
**File**: `tests/objarr1.jav`
```java
class Box {
    int value;
    
    Box(int v) {
        value = v;
    }
    
    int getValue() {
        return value;
    }
}

class Main {
    public static void main() {
        Box[] boxes = new Box[3];
        boxes[0] = new Box(10);
        boxes[1] = new Box(20);
        boxes[2] = new Box(30);
        
        System.out.println(boxes[0].getValue());  // Expected: 10
        System.out.println(boxes[1].getValue());  // Expected: 20
        System.out.println(boxes[2].getValue());  // Expected: 30
    }
}
```

### Test 2.4: Method Overloading Test
**File**: `tests/overload1.jav`
```java
class Calculator {
    int add(int a, int b) {
        return a + b;
    }
    
    int add(int a, int b, int c) {
        return a + b + c;
    }
}

class Main {
    public static void main() {
        Calculator calc = new Calculator();
        System.out.println(calc.add(10, 20));      // Expected: 30
        System.out.println(calc.add(10, 20, 30));  // Expected: 60
    }
}
```

## Implementation Order

推奨実装順序：

1. **Step 2.2: Static Members** (最優先)
   - 既存のインスタンスメンバーと対比しやすい
   - コンストラクタより単純
   - 独立して実装・テスト可能

2. **Step 2.1: Constructors**
   - オブジェクト初期化の改善
   - Static members の実装経験を活かせる

3. **Step 2.3: Object Arrays**
   - 配列とオブジェクトの統合
   - より複雑なデータ構造

4. **Step 2.4: Method Overloading**
   - 最も複雑
   - 他の機能が完成してから実装

## Success Criteria

Phase 2 完了条件：

- ✓ 静的フィールドと静的メソッドが動作する
- ✓ コンストラクタでオブジェクトを初期化できる
- ✓ オブジェクト配列を作成・操作できる
- ✓ メソッドオーバーロードが正しく解決される
- ✓ 全テストケースが成功する
- ✓ ビルド時に警告が出ない

## Estimated Timeline

- Step 2.2 (Static Members): 2-3 days
- Step 2.1 (Constructors): 2-3 days
- Step 2.3 (Object Arrays): 2-3 days
- Step 2.4 (Method Overloading): 3-4 days
- Testing and refinement: 2 days

**Total**: 11-15 days

## Notes

- 16-bit DOS の制約を常に意識する
- メモリ使用量を最小限に抑える
- 無限ループを避ける
- 各ステップで動作確認を行う
- ドキュメントを随時更新する