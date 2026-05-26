# DOS Java Compiler

![DOSJava](https://raw.githubusercontent.com/dotnsf/dosjava/refs/heads/main/dosjava.png)

16-bit PC-DOS上で動作するJavaコンパイラとランタイム環境

## 概要

DOS Java Compilerは、16-bit PC-DOS環境でJavaのサブセットをコンパイル・実行するための完全なツールチェーンです。Open Watcom v2 Cコンパイラでビルドされ、Large memory model（複数セグメント対応）で動作します。

## 主な機能

- **統合コンパイラ（djc.exe）**: Javaソースから.djcバイトコードへの一括コンパイル
- **仮想マシン（djvm.exe）**: スタックベースのバイトコードインタープリタ
- **ランタイムライブラリ**: 基本的なJavaクラス（Object, String, System, Integer）
- **ツールチェーン**: .djc生成、.class変換ツール

## サポートするJavaサブセット

### データ型
- `int` - 16-bit符号付き整数（-32768 ～ 32767）
  - 配列のインデックスは `int` 型のみ
- `long` - 32-bit符号付き整数（Phase 5で追加）
  - リテラル: `123L` または `123l`
  - 演算: `+`, `-`, `*`, `/`, `%`
  - 比較: `==`, `!=`, `<`, `>`, `<=`, `>=`
  - `System.out.println(long)` サポート
- `float` - 32-bit浮動小数点数（Phase 6.1で追加）
  - リテラル: `3.14f` または `3.14F`
  - 演算: `+`, `-`, `*`, `/`, `%`
  - 比較: `==`, `!=`, `<`, `>`, `<=`, `>=`
  - `System.out.println(float)` サポート
  - IEEE 754形式
- `boolean` - 真偽値
  - リテラル: `true`, `false`（Phase 8で追加）
  - 論理演算: `&&`, `||`, `!`
- `void` - 戻り値なし
- `null` - nullリテラル（Phase 9.3で追加）
  - 参照型（String、配列）への代入可能
  - 参照型との比較演算（`==`, `!=`）
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
- `switch` / `case` / `default` / `break`（Phase 7で追加）
  - INT型、LONG型、STRING型のswitch文
  - 複数のcase文
  - Fall-through動作（Phase 7.3で追加）
  - default case（任意の位置）
  - Nested switch（switch文のネスト）

### 演算子
- **算術**: `+`, `-`, `*`, `/`, `%`
- **関係**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **論理**: `&&`, `||`, `!`
- **代入**: `=`
- **型キャスト**: `(type)expression`（Phase 9.1で追加）
  - int ↔ long ↔ float の相互変換
  - 例: `(int)3.7f`, `(float)42`, `(long)999`

### クラスとメソッド
- クラス宣言（1ファイル1クラス）
- `public static` メソッド
- `int` パラメータ付き static メソッド
- `int` / `void` 戻り値
- ローカル変数
- static メソッド呼び出し

### 現在利用できる配列機能
- `int[]` - 整数配列
- `long[]` - 長整数配列（Phase 5.4で追加）
- `float[]` - 浮動小数点配列（Phase 6.2で追加）
- `boolean[]` - 真偽値配列
- 配列要素アクセス
- 配列要素代入
- `array.length`

### Mathクラス（Phase 6.3で追加、Phase 9.2で整数引数対応）
数学関数を提供するクラス

**Phase 9.2の拡張**: すべてのMath関数がint/long引数を受け入れ可能になりました。
整数引数は自動的にfloatに変換されます。

#### 基本関数
- `float Math.abs(float x)` - 絶対値
  - `Math.abs(int)`, `Math.abs(long)` も使用可能
- `float Math.min(float a, float b)` - 最小値
  - `Math.min(int, int)`, `Math.min(long, long)` も使用可能
- `float Math.max(float a, float b)` - 最大値
  - `Math.max(int, int)`, `Math.max(long, long)` も使用可能
- `float Math.sqrt(float x)` - 平方根
  - `Math.sqrt(int)`, `Math.sqrt(long)` も使用可能
- `float Math.floor(float x)` - 床関数（Phase 6.3で追加）
  - `Math.floor(int)`, `Math.floor(long)` も使用可能
- `float Math.ceil(float x)` - 天井関数（Phase 6.3で追加）
  - `Math.ceil(int)`, `Math.ceil(long)` も使用可能

#### 三角関数
- `float Math.sin(float x)` - サイン（ラジアン）
  - `Math.sin(int)`, `Math.sin(long)` も使用可能
- `float Math.cos(float x)` - コサイン（ラジアン）
  - `Math.cos(int)`, `Math.cos(long)` も使用可能
- `float Math.tan(float x)` - タンジェント（ラジアン）
  - `Math.tan(int)`, `Math.tan(long)` も使用可能

#### 指数・対数関数
- `float Math.pow(float base, float exp)` - べき乗
  - `Math.pow(int, int)`, `Math.pow(long, long)` も使用可能
- `float Math.exp(float x)` - 自然指数関数（e^x）
  - `Math.exp(int)`, `Math.exp(long)` も使用可能
- `float Math.log(float x)` - 自然対数（ln(x)）
  - `Math.log(int)`, `Math.log(long)` も使用可能

#### 使用例
```java
class MathDemo {
    public static void main() {
        float x = 3.0f;
        float y = 4.0f;
        
        // ピタゴラスの定理: c = √(x² + y²)
        float c = Math.sqrt(Math.pow(x, 2.0f) + Math.pow(y, 2.0f));
        System.out.println(c);  // 5.00
        
        // 円の面積: A = π × r²
        float pi = 3.14f;
        float r = 5.0f;
        float area = pi * Math.pow(r, 2.0f);
        System.out.println(area);  // 78.50
        
        // Phase 9.2: 整数引数の使用
        int a = -42;
        float absValue = Math.abs(a);  // 自動的にfloatに変換
        System.out.println(absValue);  // 42.00
        
        int base = 2;
        float result = Math.pow(base, 3);  // 2^3 = 8
        System.out.println(result);  // 8.00
        
        return;
    }
}
```

### Dateクラス（Phase 3.5で追加、Phase 5.5でlong型対応）
DOS環境で日付・時刻を扱うためのクラス

#### コンストラクタ
- `Date()` - 現在のシステム時刻でDateオブジェクトを作成
- `Date(long timestamp)` - 指定したUnixタイムスタンプ（**秒単位**）でDateオブジェクトを作成
  - **重要**: `timestamp`は**秒単位**です（Javaの標準APIはミリ秒ですが、dosjavaでは秒を使用）
  - 32ビットlong型により、1970年～2106年の範囲をサポート

#### メソッド
- `long getTime()` - Unixタイムスタンプ（**秒単位**）を返す
  - **重要**: 戻り値は**秒単位**です（Javaの標準APIはミリ秒ですが、dosjavaでは秒を使用）
- `void setTime(long timestamp)` - 指定したタイムスタンプ（**秒単位**）に設定
  - **重要**: `timestamp`は**秒単位**です（Javaの標準APIはミリ秒ですが、dosjavaでは秒を使用）
- `int getFullYear()` - 年を返す（1970-2106）
- `int getMonth()` - 月を返す（0-11、0=1月）
- `int getDate()` - 日を返す（1-31）
- `int getHours()` - 時を返す（0-23）
- `int getMinutes()` - 分を返す（0-59）
- `int getSeconds()` - 秒を返す（0-59）

#### 使用例
```java
class DateDemo {
    public static void main() {
        // 現在時刻のDateオブジェクトを作成
        Date now = new Date();
        
        // 年月日を取得
        int year = now.getFullYear();
        int month = now.getMonth() + 1;  // 1-12で表示
        int day = now.getDate();
        
        System.out.println("Date: ");
        System.out.println(year);
        System.out.println("-");
        System.out.println(month);
        System.out.println("-");
        System.out.println(day);
        
        // タイムスタンプを取得（秒単位）
        long timestamp = now.getTime();
        System.out.println("Timestamp (seconds): ");
        System.out.println(timestamp);
        
        // 特定のタイムスタンプでDateオブジェクトを作成（秒単位）
        long oneDay = 86400L;  // 24 * 60 * 60 秒
        Date tomorrow = new Date(timestamp + oneDay);
        
        return;
    }
}
```

#### 重要な制約事項

##### ⚠️ タイムスタンプは秒単位（ミリ秒ではない）
dosjavaのDateクラスは、32ビットlong型の制限により、**秒単位**でタイムスタンプを扱います。
これは標準JavaのDate APIとは異なります：

| API | dosjava | 標準Java |
|-----|---------|----------|
| `Date(long)` | **秒単位** | ミリ秒単位 |
| `getTime()` | **秒単位** | ミリ秒単位 |
| `setTime(long)` | **秒単位** | ミリ秒単位 |

**理由**:
- 32ビットlong型の最大値: 4,294,967,295
- 2026年のミリ秒タイムスタンプ: 約1,779,468,000,000（32ビットを超える）
- 2026年の秒タイムスタンプ: 約1,779,468,000（32ビット範囲内）

**例**:
```java
// dosjava（秒単位）
long timestamp = 1779468000L;  // 2026年5月22日（秒）
Date d = new Date(timestamp);

// 標準Java（ミリ秒単位）の場合
// long timestamp = 1779468000000L;  // 2026年5月22日（ミリ秒）
```

##### その他の制限事項
- **32ビットlong型**: タイムスタンプは0～4,294,967,295の範囲（1970年～2106年）
- **月の規約**: 0ベース（0=1月、11=12月）、JavaScript Date APIと同じ
- **タイムゾーン**: ローカル時刻のみ（タイムゾーン変換なし）
- **精度**: 秒単位（ミリ秒精度なし）

### 制限事項
- インスタンスメソッド不可（Dateクラスを除く）
- メソッドオーバーロード不可
- `String` パラメータ不可
- `String` 戻り値不可
- `String + int` 不可
- 一般オブジェクト生成不可（Dateクラスを除く）
- 継承・インターフェース不可
- 例外処理不可（Phase 3.5.1でテストファイル作成済み、実装は将来）
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
- [linechk.exe](https://github.com/dotnsf/dosjava/raw/refs/heads/main/build/bin/linechk.exe)        - テスト時のファイルチェッカー（tests\runtest.bat 内で使用）


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
- **配列**: `int[]`、`array.length`
- **複雑**: フィボナッチ、階乗

## ドキュメント

- [QUICKSTART.md](QUICKSTART.md) - 5分で始めるガイド
- [ARCHITECTURE.md](ARCHITECTURE.md) - システムアーキテクチャ
- [BUILD.md](BUILD.md) - ビルド手順
- [TECHNICAL_SPEC.md](TECHNICAL_SPEC.md) - 技術仕様
- [PHASE3_5_SUMMARY.md](PHASE3_5_SUMMARY.md) - Phase 3.5実装サマリー（Date Support完了）
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
- インスタンスベースのオブジェクト指向機能なし（Dateクラスを除く）
- `String` は Phase 1 の限定サポートのみ
- `String + int`、`String` 引数、`String` 戻り値は未対応
- `double` 型なし（`float` は Phase 6.1 で追加済み）

## 開発フェーズ

### Phase 1: 基本機能 ✅ 完了
- 仮想マシン（djvm.exe）の実装
- 文字列リテラルのサポート
- `String.length()` のサポート
- `String + String` 連結のサポート

### Phase 2: String機能拡張 ✅ 完了
- String クラスの完全実装
- 文字列操作メソッド（charAt, substring, indexOf, etc.）
- 文字列比較とユーティリティ

### Phase 3: I/O System ✅ 完了
- InputStream/OutputStream 基底クラス
- FileInputStream/FileOutputStream
- BufferedReader/BufferedWriter
- コンストラクタ引数サポート

### Phase 3.5: Exception Handling and Date Support 🔄 進行中
- **Phase 3.5.1**: Exception Handling ⏳ 準備完了
  - try-catch-finally-throw テストファイル作成済み
  - コンパイラ/VM実装待ち
- **Phase 3.5.2**: Date Support ✅ 完了
  - DOS Time API実装（dostime.h/dostime.c）
  - Date クラス実装（date.h/date.c）
  - 全48テストパス
  - 詳細: [PHASE3_5_SUMMARY.md](PHASE3_5_SUMMARY.md)

### Phase 4: Network Functionality 🔄 進行中
- Wattcp TCP/IP スタック統合
- Socket/ServerSocket クラス
- ネットワークI/O

### Phase 5: Long Type Support ✅ 完了
- **Phase 5.1**: 32-bit long型の基本実装
  - リテラル、演算子、比較演算
- **Phase 5.2**: long型のコード生成とVM実装
  - スタック操作、算術演算、比較演算
- **Phase 5.3**: long型の出力サポート
  - `System.out.println(long)` 実装
- **Phase 5.4**: long配列サポート
  - `long[]` 配列の作成、アクセス、代入
- **Phase 5.5**: Date クラスのlong型対応
  - タイムスタンプを秒単位のlong型で扱う

### Phase 6: Float Type and Math Support ✅ 完了
- **Phase 6.1**: 32-bit float型の実装
  - リテラル、演算子、比較演算
  - IEEE 754形式
  - `System.out.println(float)` サポート
- **Phase 6.2**: float配列サポート
  - `float[]` 配列の作成、アクセス、代入
- **Phase 6.3**: Math クラスの実装
  - 基本関数: abs, min, max, sqrt
  - 三角関数: sin, cos, tan
  - 指数・対数: pow, exp, log
  - 詳細: [PHASE6_3_MATH_COMPLETION.md](PHASE6_3_MATH_COMPLETION.md)

### Phase 7: Switch Statement Support ✅ 完了
- **Phase 7.1**: 基本的なswitch文サポート
  - INT型、LONG型、STRING型のswitch
  - case文とdefault文
- **Phase 7.2**: String switchの最適化
  - 文字列比較の効率化
- **Phase 7.3**: Fall-through動作の実装
  - breakなしでの次のcaseへの継続実行
  - 詳細: [PHASE7_3_SWITCH_FALLTHROUGH_FIX.md](PHASE7_3_SWITCH_FALLTHROUGH_FIX.md)

### Phase 8: Boolean Literal Support ✅ 完了
- `true` / `false` リテラルのサポート
- パーサー、意味解析器、コード生成器の拡張
- ループ条件での使用（`while (true)` など）
- 詳細: [PHASE8_BOOLEAN_LITERAL_PLAN.md](PHASE8_BOOLEAN_LITERAL_PLAN.md)

### Phase 9: Type System Enhancements ✅ 完了
- **Phase 9.1**: 型キャストサポート
  - `(type)expression` 構文の実装
  - int ↔ long ↔ float の相互変換
  - Math関数戻り値のキャスト対応
- **Phase 9.2**: Math関数の整数引数サポート
  - すべてのMath関数でint/long引数を受け入れ
  - 自動的なfloat型への変換
- **Phase 9.3**: nullリテラルサポート
  - `null` キーワードの追加
  - 参照型（String、配列）への代入
  - null比較演算（`==`, `!=`）
- 詳細: [PHASE9_COMPLETION.md](PHASE9_COMPLETION.md)

### 今後の予定
- [ ] Phase 4 Network機能完成
- [ ] Phase 10: より多くのランタイムライブラリ
- [ ] 最適化
- [ ] デバッガ

## ライセンス

This code is licensed under the MIT License.

This project is provided as-is without warranty. The MIT License permits use for any purpose, including commercial use.

## Copyright

2026 [K.Kimura @ Juge.Me](https://github.com/dotnsf) all rights reserved.

## 貢献

プルリクエストを歓迎します。大きな変更の場合は、まずissueを開いて変更内容を議論してください。

## 作者

[K.Kimura](https://github.com/dotnsf), developed for 16-bit DOS environments using Open Watcom C/C++.

## 謝辞

- Open Watcom Project
- Java Language Specification
- DOS開発コミュニティ
