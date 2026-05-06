# DOS Java Compiler - クイックスタートガイド

5分でDOS Java Compilerを始めましょう！

## 前提条件

- DOSBox または 16-bit DOS環境
- djc.exe（コンパイラ）
- djvm.exe（実行マシン）
- Java の基本概念である **"Write once, Run anywhere"** を無視しています

## ステップ1: 最初のプログラムを書く

`Hello.jav`というファイルを作成します：

```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

## ステップ2: コンパイルする

```batch
djc Hello.jav
```

成功すると以下のように表示されます：

```
Compiled: Hello.jav -> Hello.djc
```

## ステップ3: 詳細出力で確認

```batch
djc -v Hello.jav
```

出力例：

```
DOS Java Compiler v1.0
Compiling: Hello.jav
Output: Hello.djc

[1/4] Lexical analysis...
      15 tokens generated
[2/4] Parsing...
      8 AST nodes created
[3/4] Semantic analysis...
      3 symbols defined
[4/4] Code generation...
      45 bytes generated

Compilation successful!

Statistics:
  Lines:     6
  Tokens:    15
  AST nodes: 8
  Symbols:   3
  Bytecode:  45 bytes
```

## ステップ4: より複雑なプログラムを試す

### 算術演算

`Math.jav`:
```java
class Math {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        int product = a * b;
        int quotient = b / a;
        return;
    }
}
```

コンパイル:
```batch
djc Math.jav
```

### 条件分岐

`Conditional.jav`:
```java
class Conditional {
    public static void main() {
        int x = 10;
        int result = 0;
        
        if (x > 5) {
            result = 100;
        }
        
        if (x < 20) {
            result = result + 50;
        }
        
        return;
    }
}
```

### ループ

`Loop.jav`:
```java
class Loop {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        return;
    }
}
```

### フィボナッチ数列

`Fibonacci.jav`:
```java
class Fibonacci {
    public static void main() {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        return;
    }
}
```

## ステップ5: 中間ファイルを確認

中間ファイルを保持してコンパイルプロセスを確認：

```batch
djc -k Hello.jav
```

生成されるファイル：
- `Hello.tok` - トークンストリーム
- `Hello.ast` - 抽象構文木
- `Hello.sym` - シンボルテーブル
- `Hello.djc` - バイトコード

## よくある問題と解決方法

### 問題1: ファイルが見つからない

```
Error: Input file not found: Hello.jav
```

**解決方法**: ファイル名とパスを確認してください。

### 問題2: 構文エラー

```
Compilation failed in phase: Parsing
Error: Parse error at line 3, column 16
```

**解決方法**: セミコロン（`;`）や括弧（`{}`）の閉じ忘れを確認してください。

### 問題3: 型エラー

```
Compilation failed in phase: Semantic analysis
Error: Type mismatch
```

**解決方法**: 変数の型を確認してください。`int`と`boolean`は互換性がありません。

## コンパイラオプション

### 基本オプション

```batch
# 出力ファイル指定
djc -o output.djc source.jav

# 詳細出力
djc -v source.jav

# 中間ファイル保持
djc -k source.jav

# 複数オプション
djc -k -v source.jav
```

### ヘルプとバージョン

```batch
# ヘルプ表示
djc --help

# バージョン情報
djc --version
```

## サポートされる機能

### ✅ 使用可能

- `int`型変数
- `boolean`型変数
- 算術演算（`+`, `-`, `*`, `/`, `%`, `++`, `--`, `+=`, `-=` ）
- 比較演算（`==`, `!=`, `<`, `>`, `<=`, `>=`）
- 論理演算（`&&`, `||`, `!`）
- `if`文
- `while`ループ、`for`ループ
- ローカル変数
- staticメソッド
- 配列
- 文字列リテラル（`String`型変数）
- (String).length() などの一部文字列関数
  - .length(), .toUpperCase(), .toLowerCase(), .startsWith(), .endsWith(), .indexOf(), lastIndexOf(), .substr()
- File.open() などの一部ファイル関数
  - .open(), .readLine(), .writeLine(), .close()


### ❌ 使用不可

- オブジェクト生成（`new`）
- `switch`文
- 継承・インターフェース
- 例外処理
- パッケージ・import

## 次のステップ

1. **サンプルプログラムを試す**: `tests/e2e/`ディレクトリのサンプルを参照
2. **ドキュメントを読む**: [README.md](README.md)で詳細を確認
3. **テストを実行**: `tests/e2e/run_tests.bat`でテストスイートを実行
4. **独自のプログラムを作成**: サポートされる機能を組み合わせて実験

## 便利なヒント

### ヒント1: 段階的にテスト

小さなプログラムから始めて、徐々に複雑にしていきましょう。

### ヒント2: 詳細出力を活用

`-v`オプションで各フェーズの進行状況を確認できます。

### ヒント3: 中間ファイルでデバッグ

`-k`オプションで中間ファイルを保持し、問題を特定しやすくします。

### ヒント4: エラーメッセージを読む

エラーメッセージには行番号と列番号が含まれています。

## サンプルコード集

### 1. 変数宣言と代入

```java
class Variables {
    public static void main() {
        int x = 10;
        int y = 20;
        int z = x + y;
        return;
    }
}
```

### 2. 複雑な式

```java
class Expression {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = 30;
        int result = (a + b) * c - a / 2;
        return;
    }
}
```

### 3. ネストしたif文

```java
class NestedIf {
    public static void main() {
        int x = 10;
        int y = 20;
        
        if (x > 5) {
            if (y > 15) {
                int result = 100;
            }
        }
        
        return;
    }
}
```

### 4. ネストしたループ

```java
class NestedLoop {
    public static void main() {
        int i = 0;
        
        while (i < 5) {
            int j = 0;
            while (j < 3) {
                int product = i * j;
                j = j + 1;
            }
            i = i + 1;
        }
        
        return;
    }
}
```

### 5. 階乗計算

```java
class Factorial {
    public static void main() {
        int n = 5;
        int result = 1;
        int i = 1;
        
        while (i <= n) {
            result = result * i;
            i = i + 1;
        }
        
        return;
    }
}
```

## まとめ

これでDOS Java Compilerの基本的な使い方がわかりました！

- ✅ Javaプログラムを書く
- ✅ djcでコンパイルする
- ✅ エラーを理解して修正する
- ✅ サンプルプログラムを試す

さらに詳しい情報は[README.md](README.md)を参照してください。

Happy coding on DOS! 🎉