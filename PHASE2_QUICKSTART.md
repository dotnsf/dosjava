# Phase 2: String型完全サポート - クイックスタートガイド

このガイドは Phase 2 の実装を開始するための手順を説明します。

## 前提条件

- Phase 1（オブジェクト指向基盤）が完了していること
- Open Watcom v2 が `C:\WATCOM` にインストールされていること
- DOSBox-X が利用可能であること

## ドキュメント構成

Phase 2 には以下のドキュメントがあります：

1. **[`PHASE2_STRING_PLAN.md`](PHASE2_STRING_PLAN.md)** - 実装計画（全体像）
2. **[`PHASE2_STRING_TASKS.md`](PHASE2_STRING_TASKS.md)** - 詳細タスクリスト
3. **このファイル** - クイックスタートガイド

## 実装の流れ

Phase 2 は以下の4つのステップで実装します：

```
Step 2.1: String パラメータ (2-3日)
    ↓
Step 2.2: String 戻り値 (2-3日)
    ↓
Step 2.3: String + int 混合演算 (3-4日)
    ↓
Step 2.4: String フィールド (2-3日)
    ↓
総合テストと最終調整 (2-3日)
```

## Step 2.1: String パラメータのサポート

### 目標
メソッドの引数として String を渡せるようにする。

### 実装箇所

#### 1. Semantic Analysis
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**変更内容**:
```c
// check_method_declaration() 内
// パラメータ型チェックで TYPE_CLASS (String) を許可

// 既存コード（例）:
if (param_type.kind != TYPE_INT && param_type.kind != TYPE_BOOLEAN) {
    semantic_error(...);
}

// 変更後:
if (param_type.kind != TYPE_INT && 
    param_type.kind != TYPE_BOOLEAN &&
    param_type.kind != TYPE_CLASS) {
    semantic_error(...);
}

// check_call() 内
// 引数の型チェックで String を許可
```

#### 2. Code Generation
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**変更内容**:
```c
// generate_method_descriptor() 内
// String パラメータを "Ljava/lang/String;" として追加

// 例: void greet(String name) → "(Ljava/lang/String;)V"
// 例: void test(int a, String b) → "(ILjava/lang/String;)V"
```

#### 3. VM の確認
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**確認内容**:
- `OP_INVOKE_STATIC` / `OP_INVOKE_VIRTUAL` が String 引数を処理できるか
- `descriptor_param_count()` が `Ljava/lang/String;` を正しくカウントするか

### テストケース

**ファイル**: `tests/strprm1.jav`
```java
class strprm1 {
    public static void greet(String name) {
        System.out.println("Hello, " + name);
    }
    
    public static void main() {
        greet("Alice");
        greet("Bob");
    }
}
```

### ビルドと実行

```batch
REM 環境変数設定
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binnt;C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h;C:\WATCOM\h\nt

REM ビルド
cd dosjava
wmake all

REM テスト（DOSBox-X で実行）
REM 1. DOSBox-X を起動
REM 2. build\bin に移動
REM 3. djc strprm1.jav
REM 4. djvm strprm1.djc
```

### 期待出力
```
Hello, Alice
Hello, Bob
```

---

## Step 2.2: String 戻り値のサポート

### 目標
メソッドから String を返せるようにする。

### 実装箇所

#### 1. Semantic Analysis
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**変更内容**:
```c
// check_method_declaration() 内
// 戻り値型で TYPE_CLASS (String) を許可

// check_return() 内
// String 型の return 値チェック

// check_call() 内
// String を返すメソッドの戻り値型を正しく設定
```

#### 2. Code Generation
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**変更内容**:
```c
// generate_method_descriptor() 内
// String 戻り値を "Ljava/lang/String;" として追加

// 例: String getName() → "()Ljava/lang/String;"
// 例: String greet(String name) → "(Ljava/lang/String;)Ljava/lang/String;"
```

### テストケース

**ファイル**: `tests/strret1.jav`
```java
class strret1 {
    public static String getName() {
        return "John";
    }
    
    public static String getGreeting(String name) {
        return "Hello, " + name;
    }
    
    public static void main() {
        String name = getName();
        String greeting = getGreeting(name);
        System.out.println(greeting);
    }
}
```

### 期待出力
```
Hello, John
```

---

## Step 2.3: String + int 混合演算のサポート

### 目標
`String + int` および `int + String` を可能にする。

### 実装箇所

#### 1. Semantic Analysis
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**変更内容**:
```c
// check_binary_op() 内
// BINOP_ADD で以下を追加:

if (op == BINOP_ADD) {
    // String + String (既存)
    if (is_string_type(analyzer, left_type) && 
        is_string_type(analyzer, right_type)) {
        result_type->kind = TYPE_CLASS;
        result_type->class_name = semantic_add_string(analyzer, "String");
        return 0;
    }
    
    // String + int (新規)
    if (is_string_type(analyzer, left_type) && 
        is_numeric_type(right_type)) {
        result_type->kind = TYPE_CLASS;
        result_type->class_name = semantic_add_string(analyzer, "String");
        return 0;
    }
    
    // int + String (新規)
    if (is_numeric_type(left_type) && 
        is_string_type(analyzer, right_type)) {
        result_type->kind = TYPE_CLASS;
        result_type->class_name = semantic_add_string(analyzer, "String");
        return 0;
    }
}
```

#### 2. Code Generation
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**変更内容**:
```c
// generate_binary_op() 内
// String + int の場合:
// 1. int を String に変換（Integer.toString 呼び出し）
// 2. String + String 連結

// int + String の場合:
// 1. int を String に変換
// 2. String + String 連結
```

#### 3. VM の実装
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**追加内容**:
```c
// native_integer_tostring() の実装
static int16_t native_integer_tostring(VM* vm) {
    int16_t value = vm_pop(vm);
    String* str = string_fromint(value);  // 既存関数を使用
    if (!str) {
        return -1;
    }
    // String ハンドルをスタックにプッシュ
    vm_push(vm, (int16_t)str);
    return 0;
}

// ネイティブメソッドテーブルに登録
// "Integer.toString" -> native_integer_tostring
```

### テストケース

**ファイル**: `tests/strint1.jav`
```java
class strint1 {
    public static void main() {
        int count = 42;
        String msg1 = "Count: " + count;
        String msg2 = count + " items";
        String msg3 = "Total: " + count + " items";
        
        System.out.println(msg1);
        System.out.println(msg2);
        System.out.println(msg3);
    }
}
```

### 期待出力
```
Count: 42
42 items
Total: 42 items
```

---

## Step 2.4: String フィールドのサポート

### 目標
クラスのフィールドとして String を持てるようにする。

### 実装箇所

#### 1. Semantic Analysis
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**変更内容**:
```c
// check_field_declaration() 内
// TYPE_CLASS (String) をフィールド型として許可

// check_field_access() 内
// String フィールドの型情報を返す

// check_assignment() 内
// String フィールドへの代入チェック
```

#### 2. Code Generation
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**変更内容**:
```c
// generate_field_descriptor() 内
// String フィールドを "Ljava/lang/String;" として追加

// generate_field_access() / generate_field_assignment() 内
// String ハンドルの読み書き
```

### テストケース

**ファイル**: `tests/strfld1.jav`
```java
class Person {
    String name;
    int age;
    
    void setName(String n) {
        name = n;
    }
    
    String getName() {
        return name;
    }
    
    void setAge(int a) {
        age = a;
    }
    
    int getAge() {
        return age;
    }
}

class strfld1 {
    public static void main() {
        Person p = new Person();
        p.setName("Alice");
        p.setAge(25);
        
        System.out.println(p.getName());
        System.out.println(p.getAge());
    }
}
```

### 期待出力
```
Alice
25
```

---

## 総合テスト

### テストケース

**ファイル**: `tests/strall1.jav`
```java
class Student {
    String name;
    int score;
    
    void init(String n, int s) {
        name = n;
        score = s;
    }
    
    String getReport() {
        return name + ": " + score + " points";
    }
}

class strall1 {
    public static void main() {
        Student s1 = new Student();
        s1.init("Alice", 95);
        
        Student s2 = new Student();
        s2.init("Bob", 87);
        
        System.out.println(s1.getReport());
        System.out.println(s2.getReport());
    }
}
```

### 期待出力
```
Alice: 95 points
Bob: 87 points
```

---

## デバッグのヒント

### デバッグ出力の追加

コンパイラやVMにデバッグ出力を追加する場合：

```c
// コンパイラ（semantic.c, codegen.c など）
printf("[DEBUG] String parameter detected: %s\n", param_name);

// VM（interpreter.c）
printf("[DEBUG] String handle: %d\n", string_handle);
```

### よくある問題と解決策

#### 問題1: String パラメータがコンパイルエラーになる
**原因**: Semantic Analysis で String 型が許可されていない
**解決**: `check_method_declaration()` で `TYPE_CLASS` を許可

#### 問題2: String 戻り値が正しく受け取れない
**原因**: メソッドディスクリプタが正しく生成されていない
**解決**: `generate_method_descriptor()` で戻り値部分を確認

#### 問題3: String + int がコンパイルエラーになる
**原因**: `check_binary_op()` で混合演算が許可されていない
**解決**: `BINOP_ADD` で `String + int` と `int + String` を追加

#### 問題4: String フィールドが正しく動作しない
**原因**: フィールドディスクリプタが正しく生成されていない
**解決**: `generate_field_descriptor()` で String 対応を確認

---

## ビルドコマンド

### 通常ビルド
```batch
wmake all
```

### クリーンビルド
```batch
wmake clean
wmake all
```

### 特定のターゲットのみビルド
```batch
wmake djc.exe
wmake djvm.exe
```

---

## テスト実行手順

### Windows 上でのコンパイルテスト

```batch
cd dosjava\build\bin
djc.exe ..\..\tests\strprm1.jav
```

### DOSBox-X でのテスト

1. DOSBox-X を起動
2. dosjava ディレクトリをマウント
   ```
   mount c c:\Users\dotns\src\dosjava
   c:
   ```
3. build\bin に移動
   ```
   cd build\bin
   ```
4. コンパイルと実行
   ```
   djc strprm1.jav
   djvm strprm1.djc
   ```

---

## チェックリスト

各ステップ完了時に確認：

### Step 2.1 完了チェック
- [ ] String パラメータを持つメソッドがコンパイルできる
- [ ] String 引数を渡すメソッド呼び出しが動作する
- [ ] テストケース `strprm1.jav` が成功する
- [ ] ビルド時に警告が出ない

### Step 2.2 完了チェック
- [ ] String を返すメソッドがコンパイルできる
- [ ] String 戻り値が正しく受け取られる
- [ ] テストケース `strret1.jav` が成功する
- [ ] ビルド時に警告が出ない

### Step 2.3 完了チェック
- [ ] `String + int` が動作する
- [ ] `int + String` が動作する
- [ ] テストケース `strint1.jav` が成功する
- [ ] ビルド時に警告が出ない

### Step 2.4 完了チェック
- [ ] String フィールドを持つクラスがコンパイルできる
- [ ] String フィールドへのアクセスが動作する
- [ ] テストケース `strfld1.jav` が成功する
- [ ] ビルド時に警告が出ない

### Phase 2 完了チェック
- [ ] 全ステップが完了している
- [ ] 総合テストケース `strall1.jav` が成功する
- [ ] ドキュメントが更新されている
- [ ] DOSBox-X での動作確認が完了している

---

## 次のステップ

Phase 2 完了後は、以下のいずれかに進むことができます：

1. **Phase 3: 配列の拡張**
   - String 配列（`String[]`）
   - 多次元配列

2. **Phase 3: オブジェクト指向の拡張**
   - コンストラクタ
   - 静的メンバー
   - メソッドオーバーロード

3. **Phase 3: 制御構文の拡張**
   - `switch` 文
   - `do-while` ループ
   - `break` / `continue`

---

## 参考資料

- [`PHASE2_STRING_PLAN.md`](PHASE2_STRING_PLAN.md) - 実装計画
- [`PHASE2_STRING_TASKS.md`](PHASE2_STRING_TASKS.md) - 詳細タスクリスト
- [`README.md`](README.md) - プロジェクト概要
- [`.clinerules`](.clinerules) - コーディング規約

---

## サポート

実装中に問題が発生した場合：

1. デバッグ出力を追加して問題箇所を特定
2. 関連するテストケースを作成して問題を再現
3. ドキュメントを参照して設計を確認
4. 必要に応じて計画を見直す

Phase 2 の実装を開始する準備が整いました！