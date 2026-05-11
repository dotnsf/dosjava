# Phase 2: String型の完全サポート実装計画

## 概要

Phase 1 では String の基本機能（リテラル、ローカル変数、`println(String)`、`length()`、`String + String` 連結）を実装しました。Phase 2 では、String 型を完全にサポートし、実用的な文字列処理を可能にします。

## Phase 1 の実装状況（完了済み）

### ✅ 実装済み機能

1. **String リテラル**
   - 文字列リテラルの字句解析・構文解析
   - 定数プールへの格納

2. **String ローカル変数**
   - `String str = "Hello";` の宣言と初期化
   - ローカル変数としての String 型

3. **System.out.println(String)**
   - String 引数の `println()` 呼び出し
   - コンソール出力

4. **String.length()**
   - 文字列長の取得
   - `str.length()` および `"literal".length()`

5. **String + String 連結**
   - 2つの String の連結
   - 連鎖的な連結（`a + b + "56"`）

6. **String メソッド（Phase 1 で実装済み）**
   - `toUpperCase()` / `toLowerCase()` - 大文字/小文字変換
   - `startsWith(String)` / `endsWith(String)` - 前方/後方一致
   - `equals(String)` - 文字列比較
   - `compareTo(String)` - 辞書順比較
   - `indexOf(String)` / `lastIndexOf(String)` - 部分文字列検索
   - `substr(int)` / `substr(int, int)` - 部分文字列取得

### ❌ 未実装機能（Phase 2 で実装）

1. **String パラメータ**
   - メソッドの引数として String を渡せない
   - 例: `void greet(String name)` が使えない

2. **String 戻り値**
   - メソッドから String を返せない
   - 例: `String getName()` が使えない

3. **String + int 混合演算**
   - String と int の連結ができない
   - 例: `"Count: " + 42` が使えない

4. **String フィールド**
   - クラスのフィールドとして String を持てない
   - 例: `class Person { String name; }` が使えない

## Phase 2 の目標

Phase 2 では以下の機能を実装します：

### 2.1 String パラメータのサポート
- メソッド引数として String を受け取る
- 複数の String パラメータ
- String と int の混在パラメータ

### 2.2 String 戻り値のサポート
- メソッドから String を返す
- String を返すメソッドの呼び出しと代入

### 2.3 String + int 混合演算
- `String + int` → String
- `int + String` → String
- 連鎖的な混合演算（`"Count: " + count + " items"`）

### 2.4 String フィールドのサポート
- インスタンスフィールドとしての String
- String フィールドへのアクセスと代入

## 実装計画

### Step 2.1: String パラメータのサポート

#### 2.1.1 Semantic Analysis の拡張
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**現状の制限**:
- 現在、メソッドパラメータは `int` のみサポート
- String パラメータを宣言するとエラーになる

**実装内容**:
1. パラメータの型チェックで String を許可
2. String パラメータのシンボルテーブル登録
3. メソッド呼び出し時の引数型チェック（String 対応）

**変更箇所**:
```c
// semantic.c の check_method_declaration() 内
// パラメータ型チェックで TYPE_CLASS (String) を許可

// check_call() 内
// 引数の型チェックで String を許可
```

#### 2.1.2 Code Generation の拡張
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**実装内容**:
1. メソッドディスクリプタに String パラメータを含める
   - 例: `(ILjava/lang/String;)V` - int と String を受け取る
2. String 引数のスタック配置
3. ローカル変数インデックスの計算（String は参照型）

**変更箇所**:
```c
// codegen.c の generate_method_descriptor() 内
// String パラメータを "Ljava/lang/String;" として追加

// generate_method_code() 内
// String パラメータのローカル変数割り当て
```

#### 2.1.3 VM の対応確認
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**確認内容**:
- `OP_INVOKE_VIRTUAL` / `OP_INVOKE_STATIC` が String 引数を正しく処理できるか
- スタックからの String ハンドル取得
- ローカル変数への String ハンドル格納

**必要に応じて修正**:
- String ハンドルの受け渡し処理
- ディスクリプタ解析での String 対応

### Step 2.2: String 戻り値のサポート

#### 2.2.1 Semantic Analysis の拡張
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**実装内容**:
1. メソッド宣言で String 戻り値を許可
2. `return` 文での String 型チェック
3. メソッド呼び出しの戻り値型として String を扱う

**変更箇所**:
```c
// check_method_declaration() 内
// 戻り値型で TYPE_CLASS (String) を許可

// check_return() 内
// String 型の return 値チェック

// check_call() 内
// String を返すメソッドの戻り値型設定
```

#### 2.2.2 Code Generation の拡張
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**実装内容**:
1. メソッドディスクリプタに String 戻り値を含める
   - 例: `(I)Ljava/lang/String;` - int を受け取り String を返す
2. `OP_RETURN` での String ハンドルの返却
3. 呼び出し側での String ハンドルの受け取り

**変更箇所**:
```c
// generate_method_descriptor() 内
// String 戻り値を "Ljava/lang/String;" として追加

// generate_return() 内
// String 型の return 処理
```

#### 2.2.3 VM の対応確認
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**確認内容**:
- `OP_RETURN` が String ハンドルを正しく返せるか
- 呼び出し側がスタックから String ハンドルを受け取れるか

### Step 2.3: String + int 混合演算のサポート

#### 2.3.1 Semantic Analysis の拡張
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**実装内容**:
1. `check_binary_op()` で `String + int` と `int + String` を許可
2. 結果型を String として設定
3. 連鎖的な混合演算の型チェック

**変更箇所**:
```c
// check_binary_op() 内
// BINOP_ADD で以下のパターンを追加:
// - String + int → String
// - int + String → String
// - String + String → String (既存)
```

#### 2.3.2 Code Generation の拡張
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**実装内容**:
1. `String + int` の場合:
   - int を String に変換（`Integer.toString(int)` 相当）
   - 2つの String を連結
2. `int + String` の場合:
   - int を String に変換
   - 2つの String を連結

**新しいネイティブメソッド**:
```c
// VM に追加するネイティブメソッド
String* native_integer_tostring(int16_t value);
```

**変更箇所**:
```c
// generate_binary_op() 内
// BINOP_ADD で String + int / int + String の処理を追加
// 1. int を String に変換するネイティブ呼び出し
// 2. String 連結のネイティブ呼び出し
```

#### 2.3.3 VM の実装
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**実装内容**:
1. `native_integer_tostring()` の実装
   - int を String に変換
   - [`string_fromint()`](src/runtime/string.c:210) を使用（既存）
2. ネイティブメソッドテーブルに登録

**追加コード**:
```c
// interpreter.c 内
static int16_t native_integer_tostring(VM* vm) {
    int16_t value = vm_pop(vm);
    String* str = string_fromint(value);
    if (!str) {
        return -1;  // Error
    }
    // String ハンドルをスタックにプッシュ
    vm_push(vm, (int16_t)str);
    return 0;
}
```

### Step 2.4: String フィールドのサポート

#### 2.4.1 Semantic Analysis の拡張
**ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)

**実装内容**:
1. フィールド宣言で String 型を許可
2. フィールドアクセス時の String 型チェック
3. フィールド代入時の String 型チェック

**変更箇所**:
```c
// check_field_declaration() 内
// TYPE_CLASS (String) をフィールド型として許可

// check_field_access() 内
// String フィールドの型情報を返す

// check_assignment() 内
// String フィールドへの代入チェック
```

#### 2.4.2 Code Generation の拡張
**ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)

**実装内容**:
1. String フィールドのフィールドディスクリプタ生成
   - 例: `Ljava/lang/String;`
2. `OP_GET_FIELD` / `OP_PUT_FIELD` での String ハンドル処理

**変更箇所**:
```c
// generate_field_descriptor() 内
// String フィールドを "Ljava/lang/String;" として追加

// generate_field_access() / generate_field_assignment() 内
// String ハンドルの読み書き
```

#### 2.4.3 VM の対応確認
**ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)

**確認内容**:
- `OP_GET_FIELD` / `OP_PUT_FIELD` が String ハンドルを正しく処理できるか
- フィールドストレージでの String ハンドル管理

## テストケース

### Test 2.1: String パラメータ
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

**期待出力**:
```
Hello, Alice
Hello, Bob
```

### Test 2.2: String 戻り値
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

**期待出力**:
```
Hello, John
```

### Test 2.3: String + int 混合演算
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

**期待出力**:
```
Count: 42
42 items
Total: 42 items
```

### Test 2.4: String フィールド
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

**期待出力**:
```
Alice
25
```

### Test 2.5: 総合テスト
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

**期待出力**:
```
Alice: 95 points
Bob: 87 points
```

## 実装順序

推奨実装順序（依存関係を考慮）：

### 優先度 1: String パラメータ（Step 2.1）
- **理由**: 最も基本的な機能拡張
- **依存**: なし
- **期間**: 2-3日

### 優先度 2: String 戻り値（Step 2.2）
- **理由**: パラメータと対になる機能
- **依存**: Step 2.1 の経験を活かせる
- **期間**: 2-3日

### 優先度 3: String + int 混合演算（Step 2.3）
- **理由**: 実用性が高い
- **依存**: String パラメータと戻り値が必要
- **期間**: 3-4日

### 優先度 4: String フィールド（Step 2.4）
- **理由**: オブジェクト指向の完成度向上
- **依存**: 他の String 機能が完成していること
- **期間**: 2-3日

## 成功基準

Phase 2 完了条件：

- ✓ メソッドの引数として String を渡せる
- ✓ メソッドから String を返せる
- ✓ `String + int` および `int + String` が動作する
- ✓ クラスのフィールドとして String を持てる
- ✓ 全テストケースが成功する
- ✓ ビルド時に警告が出ない
- ✓ DOSBox-X での動作確認が完了

## 見積もり工数

- Step 2.1 (String パラメータ): 2-3日
- Step 2.2 (String 戻り値): 2-3日
- Step 2.3 (String + int 混合演算): 3-4日
- Step 2.4 (String フィールド): 2-3日
- テストと調整: 2-3日

**合計**: 11-16日

## 注意事項

### メモリ管理
- String オブジェクトは動的メモリを使用
- 参照カウントによるメモリ管理（既存実装を活用）
- メモリリークに注意

### 16-bit DOS の制約
- Small memory model（64KB code + 64KB data）
- スタックサイズの制限
- 文字列長の制限（実用的には 1KB 程度まで）

### デバッグ
- 各ステップで動作確認を行う
- デバッグ出力を活用（`[DEBUG]` プレフィックス）
- DOSBox-X での実機テストを重視

### ドキュメント更新
- [`README.md`](README.md) の更新
- [`COMPILER_IMPLEMENTATION_STATUS.md`](COMPILER_IMPLEMENTATION_STATUS.md) の更新
- テストガイドの更新

## 次のステップ

Phase 2 完了後の展望：

### Phase 3 候補機能
1. **配列の拡張**
   - String 配列（`String[]`）
   - 多次元配列

2. **オブジェクト指向の拡張**
   - コンストラクタ
   - 静的メンバー
   - メソッドオーバーロード

3. **制御構文の拡張**
   - `switch` 文
   - `do-while` ループ
   - `break` / `continue`

4. **例外処理**
   - `try-catch-finally`
   - 基本的な例外クラス

## まとめ

Phase 2 では String 型の完全サポートを実現し、実用的な文字列処理を可能にします。これにより、dosjava は以下のような実用的なプログラムを作成できるようになります：

- テキスト処理ツール
- 簡易データベース
- ファイル処理ユーティリティ
- インタラクティブなコンソールアプリケーション

Phase 2 の完成により、dosjava は「実験的なコンパイラ」から「実用的な開発ツール」へと進化します。