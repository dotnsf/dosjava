# DOS Java Compiler

16-bit PC-DOS上で動作するJavaコンパイラとランタイム環境

## 概要

DOS Java Compilerは、16-bit PC-DOS環境でJavaのサブセットをコンパイル・実行するための完全なツールチェーンです。Open Watcom v2 Cコンパイラでビルドされ、Small memory model（64KB code + 64KB data）で動作します。

## 主な機能

- **統合コンパイラ（djc.exe）**: Javaソースから.djcバイトコードへの一括コンパイル
- **仮想マシン**: スタックベースのバイトコードインタープリタ
- **ランタイムライブラリ**: 基本的なJavaクラス（Object, String, System, Integer）
- **ツールチェーン**: .djc生成、.class変換ツール

## サポートするJavaサブセット

### データ型
- `int` - 16-bit符号付き整数（-32768 ～ 32767）
- `boolean` - 真偽値（true/false）
- `void` - 戻り値なし

### 制御構文
- `if` 文
- `while` ループ
- `return` 文

### 演算子
- **算術**: `+`, `-`, `*`, `/`, `%`
- **関係**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **論理**: `&&`, `||`, `!`
- **代入**: `=`

### クラスとメソッド
- クラス宣言（1ファイル1クラス）
- staticメソッド
- ローカル変数

### 制限事項
- オブジェクト生成不可（`new`）
- 配列不可
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
```batch
# build/bin/ ディレクトリに以下が含まれます
djc.exe         - 統合コンパイラ
djvm.exe        - 仮想マシン（未実装）
mkdjc.exe       - .djc生成ツール
java2djc.exe    - .class変換ツール
```

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

## クイックスタート

### 1. Hello Worldプログラム

`Hello.java`を作成:
```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

### 2. コンパイル

```batch
djc Hello.java
```

出力: `Hello.djc`

### 3. 実行（将来実装予定）

```batch
djvm Hello.djc
```

## 使用方法

### djc（統合コンパイラ）

```batch
djc [options] <source.java>
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
djc Hello.java

# 出力ファイル指定
djc -o test.djc Test.java

# 詳細出力と中間ファイル保持
djc -k -v Hello.java
```

### コンパイルプロセス

```
source.java
    ↓
[Lexer] → source.tok (トークン)
    ↓
[Parser] → source.ast (抽象構文木)
    ↓
[Semantic] → source.sym (シンボルテーブル)
    ↓
[CodeGen] → source.djc (バイトコード)
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
- **制御**: if文、whileループ
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
- オブジェクト指向機能なし
- 配列なし
- 文字列リテラルなし（現在）
- float/double型なし

## 今後の予定

- [ ] 仮想マシン（djvm.exe）の実装
- [ ] 文字列リテラルのサポート
- [ ] 配列のサポート
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