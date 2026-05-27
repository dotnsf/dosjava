# Phase 10.2: Exception Variable Output Support

## 概要

Phase 10.2では、Exception変数を文字列として出力する機能を実装します。これにより、catch節で捕捉したException変数を`System.out.println()`や文字列連結で使用できるようになります。

## 目標

```java
try {
    throw new Exception();
} catch (Exception e) {
    System.out.println("Exception: " + e);  // Exception変数を文字列として出力
    System.out.println(e);                   // Exception変数を直接出力
}
```

## 現状分析

### 既存のException関連実装

1. **テストファイル**: `tests/except.jav`, `tests/excsimpl.jav`, `tests/EXC2.JAV`
   - try-catch-finally構文のテスト
   - throw文のテスト
   - Exception変数の宣言（`catch (Exception e)`）
   - **未実装**: Exception変数の出力

2. **Exception処理の現状**:
   - Exception変数は宣言されるが、使用されていない
   - Exception変数の型情報は保持されている
   - Exception変数の出力機能は未実装

## 実装計画

### Step 1: Exception変数の文字列表現定義

Exception変数を文字列として表現する方法を決定：

**オプション1: シンプルな固定文字列**
```
"Exception"
```

**オプション2: クラス名を含む**
```
"java.lang.Exception"
```

**オプション3: メッセージを含む（将来拡張）**
```
"java.lang.Exception: Error message"
```

**決定**: Phase 10.2ではオプション1（シンプルな固定文字列）を採用。将来的にメッセージサポートを追加可能。

### Step 2: パーサーの確認

Exception変数の使用をパーサーが認識できるか確認：

1. **変数参照**: `e` → 既存の変数参照パーサーで対応可能
2. **文字列連結**: `"" + e` → 既存の文字列連結パーサーで対応可能
3. **println引数**: `println(e)` → 既存の引数パーサーで対応可能

**結論**: パーサーの変更は不要。既存の変数参照機構で対応可能。

### Step 3: 意味解析器の拡張

Exception変数の型チェックを追加：

1. **変数参照の型チェック**:
   - Exception型変数の参照を許可
   - 文字列連結での使用を許可
   - println引数での使用を許可

2. **型変換ルール**:
   - Exception → String への暗黙的変換を許可
   - `"" + e` → String型
   - `println(e)` → Exception型を受け入れる新しいオーバーロード

**実装ファイル**: `tools/compiler/semantic.c`

### Step 4: コード生成器の拡張

Exception変数を文字列に変換するコードを生成：

1. **新しいオペコード**: `OP_EXCEPTION_TO_STRING`
   - スタックトップのException参照を文字列に変換
   - 固定文字列 "Exception" を生成

2. **文字列連結での処理**:
   ```
   PUSH_VAR e           // Exception変数をプッシュ
   EXCEPTION_TO_STRING  // 文字列に変換
   CONCAT               // 文字列連結
   ```

3. **println引数での処理**:
   ```
   PUSH_VAR e           // Exception変数をプッシュ
   EXCEPTION_TO_STRING  // 文字列に変換
   CALL_NATIVE println_string
   ```

**実装ファイル**: `tools/compiler/codegen.c`

### Step 5: VMの拡張

新しいオペコードを実装：

1. **OP_EXCEPTION_TO_STRING**:
   ```c
   case OP_EXCEPTION_TO_STRING: {
       // スタックトップのException参照を取得
       // 固定文字列 "Exception" を作成
       // 文字列参照をスタックにプッシュ
       break;
   }
   ```

2. **文字列生成**:
   - ヒープから文字列オブジェクトを割り当て
   - "Exception" をコピー
   - 文字列参照を返す

**実装ファイル**: `src/vm/interpreter.c`

### Step 6: オペコード定義の追加

新しいオペコードを定義：

**実装ファイル**: `src/format/opcodes.h`

```c
#define OP_EXCEPTION_TO_STRING  0xXX  // Exception → String変換
```

### Step 7: テストケースの作成

Exception変数出力の包括的なテスト：

**ファイル**: `tests/excprint.jav`

```java
class ExceptionPrintTest {
    public static void main() {
        System.out.println("=== Exception Print Tests ===");
        
        // Test 1: Direct println
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println(e);
        }
        
        // Test 2: String concatenation
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Caught: " + e);
        }
        
        // Test 3: Multiple concatenations
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Error [" + e + "] occurred");
        }
        
        System.out.println("=== Tests Complete ===");
    }
}
```

**期待される出力**:
```
=== Exception Print Tests ===
Exception
Caught: Exception
Error [Exception] occurred
=== Tests Complete ===
```

### Step 8: サンプルプログラムの作成

**ファイル**: `samples/excprint.jav`

```java
class excprint {
    public static void main() {
        try {
            System.out.println("Attempting operation...");
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Error: " + e);
        }
        System.out.println("excprint.jav worked correctly.");
    }
}
```

### Step 9: ドキュメント更新

1. **README.md**: Exception変数出力機能を追加
2. **PROGRESS.md**: Phase 10.2完了を記録
3. **PHASE10_2_COMPLETION.md**: 実装詳細を記録

## 実装順序

1. ✅ **計画書作成** (このファイル)
2. ⏳ **オペコード定義追加** (`opcodes.h`)
3. ⏳ **VM実装** (`interpreter.c`)
4. ⏳ **意味解析器拡張** (`semantic.c`)
5. ⏳ **コード生成器拡張** (`codegen.c`)
6. ⏳ **テストケース作成** (`tests/excprint.jav`)
7. ⏳ **サンプル作成** (`samples/excprint.jav`)
8. ⏳ **DOSBox-Xでテスト**
9. ⏳ **ドキュメント更新**

## 技術的考慮事項

### メモリ管理

- Exception変数自体はスタック上に存在
- 文字列変換時にヒープから文字列オブジェクトを割り当て
- 文字列は既存のガベージコレクション機構で管理

### 型システム

- Exception型は既存の型システムに統合
- String型への暗黙的変換を追加
- 型チェックは意味解析フェーズで実施

### パフォーマンス

- 固定文字列なので変換コストは最小限
- 文字列オブジェクトの割り当てのみ

## 将来の拡張

### Phase 10.3: Exception Message Support

Exception変数にメッセージを追加：

```java
throw new Exception("Error message");
catch (Exception e) {
    System.out.println(e);  // "java.lang.Exception: Error message"
}
```

### Phase 10.4: Exception Stack Trace

スタックトレース情報の追加：

```java
catch (Exception e) {
    e.printStackTrace();
}
```

## まとめ

Phase 10.2では、Exception変数を文字列として出力する基本機能を実装します。これにより、デバッグやエラーハンドリングがより実用的になります。実装は既存のアーキテクチャに自然に統合され、将来の拡張（メッセージ、スタックトレース）への道を開きます。