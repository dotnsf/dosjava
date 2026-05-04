# DOS Java Compiler

16-bit PC-DOS上で動作するJavaコンパイラとランタイム環境

## 概要

DOS Java Compilerは、16-bit PC-DOS環境でJavaのサブセットをコンパイル・実行するための完全なツールチェーンです。Open Watcom v2 Cコンパイラでビルドされ、Small memory model（64KB code + 64KB data）で動作します。

## 主な機能

- **統合コンパイラ（djc.exe）**: Javaソースから.djcバイトコードへの一括コンパイル
- **仮想マシン（djvm.exe）**: スタックベースのバイトコードインタープリタ
- **ランタイムライブラリ**: 基本的なJavaクラス（Object, String, System, Integer）
- **ツールチェーン**: .djc生成、.class変換ツール

## サポートするJavaサブセット

### データ型
- `int` - 16-bit符号付き整数（-32768 ～ 32767）
- `boolean` - 真偽値（true/false）
- `void` - 戻り値なし
- `String` - Phase 1 の限定サポート
  - 文字列リテラル
  - `String` ローカル変数
  - `System.out.println(String)`
  - `str.length()` / `"abc".length()`
  - `String + String`
  - 連結の連鎖（例: `a + b + "56"`）

### 制御構文
- `if` / `else`
- `while`
- `for`
- `return`

### 演算子
- **算術**: `+`, `-`, `*`, `/`, `%`
- **関係**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **論理**: `&&`, `||`, `!`
- **代入**: `=`

### クラスとメソッド
- クラス宣言（1ファイル1クラス）
- `public static` メソッド
- `int` パラメータ付き static メソッド
- `int` / `void` 戻り値
- ローカル変数
- static メソッド呼び出し

### 現在利用できる配列機能
- `int[]`
- `boolean[]`
- 配列要素アクセス
- 配列要素代入
- `array.length`

### 制限事項
- インスタンスメソッド不可
- メソッドオーバーロード不可
- `String` パラメータ不可
- `String` 戻り値不可
- `String + int` 不可
- 一般オブジェクト生成不可
- 継承・インターフェース不可
- 例外処理不可
- パッケージ・import不可

## システム要件

### 実行環境
- 16-bit PC-DOS 3.0以上
- 640KB RAM以上
- DOSBox（推奨）

### ビルド環境
- Open Watcom v2 C Compiler
- Windows/Linux/Mac（クロスコンパイル）

## インストール

### ビルド済みバイナリ

`build/bin/` ディレクトリに以下が含まれます：

- [djc.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/djc.exe)         - 統合コンパイラ
- [djvm.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/djvm.exe)        - 仮想マシン

### ソースからビルド
```batch
# 環境変数設定
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binnt;C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h;C:\WATCOM\h\nt

# ビルド
cd dosjava
wmake all
```

### （例）git リポジトリからクローンして DOSBox で動作させる

- 1. [DOSBox](http://www.dosbox.com/) をインストール

- 2. git clone
  - `> git clone https://github.com/dotnsf/dosjava.git`

- 3. `doxbox.conf` を編集
  - Windows であれば、デフォルトで `$HOME\AppData\Local\DOSBOx\dosbox-x.xx-x.conf` （`x.xx-x` は DOSBox のバージョン）というファイルが存在するはずなので、そのファイルをメモ帳などのテキストエディタで編集する
  - `[autoexec]` 以下に以下を追加

```
[autoexec]
mount c "C:\Users\username\dosjava"
set PATH=%PATH%;c:\build\bin
c:
```

- 4. DOSBox を起動

- 5. DOSBox の `c:\build\bin` 内に `djc.exe` と `djvm.exe` が存在するので、DOSBox 内で実行する



## クイックスタート

### 1. Hello Worldプログラム

`hello.jav`を作成:
```java
class hello {
    public static void main() {
        System.out.println("Hello, DOS!");
    }
}
```

### 2. コンパイル

```batch
djc hello.jav
```

出力: `hello.djc`

### 3. 実行

```batch
djvm hello.djc
```

## 使用方法

### djc（統合コンパイラ）

```batch
djc [options] <source.jav>
```

#### オプション
- `-o <file>` - 出力ファイル指定（デフォルト: `<source>.djc`）
- `-k` - 中間ファイル保持（.tok, .ast, .sym）
- `-v` - 詳細出力
- `-h, --help` - ヘルプ表示
- `--version` - バージョン情報

#### 例
```batch
# 基本的な使用
djc hello.jav

# 出力ファイル指定
djc -o test.djc test.jav

# 詳細出力と中間ファイル保持
djc -k -v hello.jav
```

### コンパイルプロセス

```
source.jav
    ↓
[Lexer] → source.tok (トークン)
    ↓
[Parser] → source.ast (抽象構文木)
    ↓
[Semantic] → source.sym (シンボルテーブル)
    ↓
[CodeGen] → source.djc (バイトコード)
```

※ 実行環境は 16-bit PC-DOS のため、ソース/出力ファイル名は原則として DOS 8.3 形式（ファイル名 8 文字以下 + 拡張子 3 文字以下）を使用してください。
例: `hello.java` ではなく `hello.jav`
```

## サンプルプログラム

### 算術演算
```java
class Arithmetic {
    public static void main() {
        int a = 10;
        int b = 20;
        int sum = a + b;
        int product = a * b;
        return;
    }
}
```

### 条件分岐
```java
class Conditional {
    public static void main() {
        int x = 10;
        int result = 0;
        
        if (x > 5) {
            result = 100;
        }
        
        return;
    }
}
```

### ループ
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

## プロジェクト構造

```
dosjava/
├── src/                    # ソースコード
│   ├── vm/                # 仮想マシン
│   ├── runtime/           # ランタイムライブラリ
│   └── format/            # .djcフォーマット
├── tools/                 # ツール
│   └── compiler/          # コンパイラ
├── tests/                 # テスト
│   ├── e2e/              # エンドツーエンドテスト
│   ├── lexer/            # レキサーテスト
│   ├── parser/           # パーサーテスト
│   └── semantic/         # 意味解析テスト
├── build/                 # ビルド出力
│   ├── bin/              # 実行ファイル
│   └── obj/              # オブジェクトファイル
├── Makefile              # ビルドスクリプト
└── README.md             # このファイル
```

## テスト

### エンドツーエンドテスト実行
```batch
cd tests\e2e
run_tests.bat
```

### テストカテゴリ
- **基本**: Hello World、最小プログラム
- **算術**: 加算、複雑な式
- **制御**: if/else、while、for
- **メソッド**: static 呼び出し、引数、int 戻り値
- **文字列**: `println(String)`、`length()`、`String + String`
- **配列**: `int[]`、`boolean[]`、`array.length`
- **複雑**: フィボナッチ、階乗

## ドキュメント

- [QUICKSTART.md](QUICKSTART.md) - 5分で始めるガイド
- [ARCHITECTURE.md](ARCHITECTURE.md) - システムアーキテクチャ
- [BUILD.md](BUILD.md) - ビルド手順
- [TECHNICAL_SPEC.md](TECHNICAL_SPEC.md) - 技術仕様
- [PHASE5_PLAN.md](PHASE5_PLAN.md) - Phase 5実装計画

## トラブルシューティング

### コンパイルエラー
```
Error: Input file not found: Hello.java
```
→ ファイル名とパスを確認してください

### 構文エラー
```
Compilation failed in phase: Parsing
Error: Parse error at line 3, column 16
```
→ 構文を確認してください（セミコロン忘れなど）

### 型エラー
```
Compilation failed in phase: Semantic analysis
Error: Type mismatch
```
→ 型の互換性を確認してください

## 制限事項

### メモリ制限
- コード: 64KB
- データ: 64KB
- スタック: 4-8KB
- ヒープ: ~40KB

### 言語制限
- インスタンスベースのオブジェクト指向機能なし
- `String` は Phase 1 の限定サポートのみ
- `String + int`、`String` 引数、`String` 戻り値は未対応
- float/double型なし

## 今後の予定

- [x] 仮想マシン（djvm.exe）の実装
- [x] 文字列リテラルのサポート
- [x] `String.length()` のサポート
- [x] `String + String` 連結のサポート
- [-] 配列サポートの安定化
- [ ] `String` 引数/戻り値のサポート
- [ ] より多くのランタイムライブラリ
- [ ] 最適化
- [ ] デバッガ

## ライセンス

MIT License

## 貢献

プルリクエストを歓迎します。大きな変更の場合は、まずissueを開いて変更内容を議論してください。

## 作者

DOS Java Compiler Project

## 謝辞

- Open Watcom Project
- Java Language Specification
- DOS開発コミュニティ