# DOSJava API リファレンス

DOSJavaで利用可能なすべてのクラスとメソッドの完全なリファレンスガイドです。

## 目次

1. [System クラス](#system-クラス)
2. [String クラス](#string-クラス)
3. [Integer クラス](#integer-クラス)
4. [Math クラス](#math-クラス)
5. [Date クラス](#date-クラス)
6. [Http クラス](#http-クラス)
7. [Exception クラス](#exception-クラス)
8. [I/O クラス](#io-クラス)

---

## System クラス

標準入出力を提供するクラス。すべてのメソッドは静的メソッドです。

### メソッド

#### `void System.println(String message)`
文字列を出力し、改行します。

**例:**
```java
System.println("Hello, DOS!");
```

#### `void System.print(String message)`
文字列を出力します（改行なし）。

#### `void System.printInt(int value)`
整数値を出力します。

#### `void System.printLong(long value)`
長整数値を出力します。

#### `void System.printFloat(float value)`
浮動小数点数を出力します。

---

## String クラス

文字列を扱うためのクラス。

### 基本操作

```java
String message = "Hello, World!";
String result = "Hello" + " " + "World";  // 連結
```

### メソッド

#### `int length()`
文字列の長さを返します。

**例:**
```java
String text = "Hello";
int len = text.length();  // 5
```

#### `String charAt(int index)`
指定位置の文字を返します（String型）。

**例外:** `StringIndexOutOfBoundsException`

**例:**
```java
String text = "Hello";
String ch = text.charAt(1);  // "e"
```

#### `int isEmpty()`
空文字列かチェックします。

**戻り値:** 空の場合は1、そうでない場合は0

#### `String trim()`
前後の空白を削除します。

**例:**
```java
String text = "  Hello  ";
String trimmed = text.trim();  // "Hello"
```

#### `String replace(String target, String replacement)`
文字列を置換します。

**例:**
```java
String text = "Hello World";
String result = text.replace("World", "DOS");  // "Hello DOS"
```

#### `int compareTo(String other)`
辞書順で比較します。

**戻り値:** 0（等しい）、負（前）、正（後）

#### `int indexOf(String str)`
部分文字列の最初の出現位置を返します。

**戻り値:** 位置（0から）、見つからない場合は-1

#### `int lastIndexOf(String str)`
部分文字列の最後の出現位置を返します。

#### `int contains(String str)`
部分文字列が含まれているかチェックします。

**戻り値:** 含まれている場合は1、そうでない場合は0

#### `String substring(int start, int end)`
部分文字列を抽出します。

**例外:** `StringIndexOutOfBoundsException`

**例:**
```java
String text = "Hello World";
String sub = text.substring(0, 5);  // "Hello"
```

#### `String repeat(int count)`
文字列を繰り返します。

**例:**
```java
String text = "Ha";
String result = text.repeat(3);  // "HaHaHa"
```

---

## Integer クラス

整数値の変換を提供するクラス。

### メソッド

#### `int Integer.parseInt(String str)`
文字列を整数値に変換します。

**例外:** `NumberFormatException`

**例:**
```java
try {
    int num = Integer.parseInt("123");  // 123
} catch (Exception e) {
    System.println("Parse error");
}
```

---

## Math クラス

数学関数を提供するクラス。すべてのメソッドは静的メソッドで、int/long引数も受け入れ可能です。

### 基本関数

#### `float Math.abs(float x)`
絶対値を返します。

**例:**
```java
float result = Math.abs(-3.14f);  // 3.14
float result2 = Math.abs(-42);    // 42.0 (int引数)
```

#### `float Math.min(float a, float b)`
最小値を返します。

#### `float Math.max(float a, float b)`
最大値を返します。

#### `float Math.sqrt(float x)`
平方根を返します。

**例:**
```java
float result = Math.sqrt(16.0f);  // 4.0
```

#### `float Math.floor(float x)`
床関数（切り捨て）。

#### `float Math.ceil(float x)`
天井関数（切り上げ）。

### 三角関数

#### `float Math.sin(float x)`
サイン（ラジアン）。

#### `float Math.cos(float x)`
コサイン（ラジアン）。

#### `float Math.tan(float x)`
タンジェント（ラジアン）。

### 指数・対数関数

#### `float Math.pow(float base, float exp)`
べき乗。

**例:**
```java
float result = Math.pow(2.0f, 3.0f);  // 8.0
float result2 = Math.pow(2, 10);      // 1024.0 (int引数)
```

#### `float Math.exp(float x)`
自然指数関数（e^x）。

#### `float Math.log(float x)`
自然対数（ln(x)）。

### 実用例

```java
class MathExample {
    public static void main() {
        // ピタゴラスの定理
        float a = 3.0f;
        float b = 4.0f;
        float c = Math.sqrt(Math.pow(a, 2.0f) + Math.pow(b, 2.0f));
        System.printFloat(c);  // 5.0
        
        // 円の面積
        float pi = 3.14159f;
        float r = 5.0f;
        float area = pi * Math.pow(r, 2.0f);
        System.printFloat(area);  // 78.54
        
        return;
    }
}
```

---

## Date クラス

日付と時刻を扱うためのクラス。

### コンストラクタ

#### `Date()`
現在のシステム時刻でDateオブジェクトを作成します。

#### `Date(long timestamp)`
指定したUnixタイムスタンプ（**秒単位**）でDateオブジェクトを作成します。

**重要:** DOSJavaでは秒単位を使用します（標準Javaのミリ秒単位とは異なります）

### メソッド

#### `long getTime()`
Unixタイムスタンプ（秒単位）を返します。

#### `void setTime(long timestamp)`
タイムスタンプ（秒単位）を設定します。

#### `int getFullYear()`
年を返します（1970-2106）。

#### `int getMonth()`
月を返します（0-11、0=1月）。

#### `int getDate()`
日を返します（1-31）。

#### `int getHours()`
時を返します（0-23）。

#### `int getMinutes()`
分を返します（0-59）。

#### `int getSeconds()`
秒を返します（0-59）。

### 実用例

```java
class DateExample {
    public static void main() {
        Date now = new Date();
        
        System.print("Date: ");
        System.printInt(now.getFullYear());
        System.print("-");
        System.printInt(now.getMonth() + 1);
        System.print("-");
        System.printInt(now.getDate());
        System.println("");
        
        // 1日後の日付
        long timestamp = now.getTime();
        long oneDay = 86400L;  // 24 * 60 * 60 秒
        Date tomorrow = new Date(timestamp + oneDay);
        
        return;
    }
}
```

### 重要な注意事項

**⚠️ タイムスタンプは秒単位（ミリ秒ではない）**

| API | DOSJava | 標準Java |
|-----|---------|----------|
| `Date(long)` | 秒単位 | ミリ秒単位 |
| `getTime()` | 秒単位 | ミリ秒単位 |
| `setTime(long)` | 秒単位 | ミリ秒単位 |

**制限事項:**
- 32ビットlong型: 1970年～2106年
- 月の規約: 0ベース（0=1月、11=12月）
- タイムゾーン: ローカル時刻のみ
- 精度: 秒単位

---

## Http クラス

HTTP通信を行うためのクラス。外部プログラム`doscurl.exe`を使用します。

### 前提条件

- `doscurl.exe`がPATH環境変数に設定されていること
- ネットワーク接続が利用可能であること

### メソッド

#### `String Http.get(String url)`
HTTP GETリクエスト（基本）。

**例外:** `NetworkException`

**例:**
```java
try {
    String response = Http.get("http://example.com/api");
    System.println(response);
} catch (Exception e) {
    System.println("Network error");
}
```

#### `String Http.get(String url, String headers)`
HTTP GETリクエスト（カスタムヘッダー付き）。

**パラメータ:**
- `headers` - 改行区切りのヘッダー（例: `"User-Agent: DOSJava\nAccept: */*"`）

**例:**
```java
String headers = "User-Agent: DOSJava/1.0\nAccept: application/json";
String response = Http.get("http://example.com/api", headers);
```

#### `String Http.post(String url, String data)`
HTTP POSTリクエスト（基本）。

**例:**
```java
String data = "name=test&value=123";
String response = Http.post("http://example.com/api", data);
```

#### `String Http.post(String url, String data, String headers)`
HTTP POSTリクエスト（カスタムヘッダー付き）。

**例:**
```java
String data = "test=value";
String headers = "Content-Type: application/x-www-form-urlencoded";
String response = Http.post("http://example.com/api", data, headers);
```

#### `String Http.put(String url, String data)`
HTTP PUTリクエスト（基本）。

#### `String Http.put(String url, String data, String headers)`
HTTP PUTリクエスト（カスタムヘッダー付き）。

#### `String Http.delete(String url)`
HTTP DELETEリクエスト（基本）。

#### `String Http.delete(String url, String headers)`
HTTP DELETEリクエスト（カスタムヘッダー付き）。

### 実用例

```java
class HttpExample {
    // 遅延関数（連続リクエスト時のタイムアウト回避）
    public static void delay(int milliseconds) {
        int iterations = milliseconds * 100;
        int dummy = 0;
        for (int i = 0; i < iterations; i = i + 1) {
            dummy = dummy + 1;
        }
        return;
    }
    
    public static void main() {
        try {
            // 基本的なGET
            String response1 = Http.get("http://httpbin.org/get");
            System.println(response1);
            
            delay(2000);  // 2秒待機
            
            // カスタムヘッダー付きGET
            String headers = "User-Agent: DOSJava/1.0";
            String response2 = Http.get("http://httpbin.org/get", headers);
            System.println(response2);
            
            delay(2000);
            
            // POST
            String data = "name=DOSJava";
            String response3 = Http.post("http://httpbin.org/post", data);
            System.println(response3);
            
        } catch (Exception e) {
            System.println("Error: ");
            System.println(e.getMessage());
        }
        
        return;
    }
}
```

### 重要な注意事項

- すべてのHTTPメソッドは`NetworkException`をスローする可能性があります
- タイムアウトは10秒
- 連続リクエスト時は2秒程度の遅延を推奨
- HTTPステータスコードの取得は未サポート

---

## Exception クラス

例外処理を行うためのクラス。

### 例外タイプ

| 例外タイプ | コード | 検出条件 |
|-----------|--------|---------|
| 一般例外 | 0 | throw文 |
| NullPointerException | 1 | null参照 |
| ArrayIndexOutOfBoundsException | 2 | 配列範囲外 |
| NumberFormatException | 3 | 数値変換エラー |
| IllegalArgumentException | 4 | 不正な引数 |
| StringIndexOutOfBoundsException | 5 | 文字列範囲外 |
| NetworkException | 6 | HTTP失敗 |

### メソッド

#### `int getType()`
例外のタイプコードを返します。

**例:**
```java
try {
    int[] arr = new int[5];
    int x = arr[10];
} catch (Exception e) {
    int type = e.getType();  // 2
    System.printInt(type);
}
```

#### `String getMessage()`
例外メッセージを返します（行番号付き）。

**形式:** `"<例外タイプ> (line <行番号>)"`

**例:**
```java
try {
    int[] arr = new int[5];
    int x = arr[10];  // Line 3
} catch (Exception e) {
    String msg = e.getMessage();
    System.println(msg);  // "Array index out of bounds (line 3)"
}
```

### 例外処理の構文

```java
try {
    // 例外が発生する可能性のあるコード
} catch (Exception e) {
    // 例外処理
}
```

---

## I/O クラス

ファイル入出力を行うためのクラス群。

### FileOutputStream

ファイルへのバイト出力。

```java
FileOutputStream fos = new FileOutputStream("output.txt");
fos.write(65);  // 'A'
fos.close();
```

### FileInputStream

ファイルからのバイト入力。

```java
FileInputStream fis = new FileInputStream("input.txt");
int b = fis.read();
while (b != -1) {
    System.printInt(b);
    b = fis.read();
}
fis.close();
```

### BufferedWriter

バッファリングされた文字出力。

```java
FileOutputStream fos = new FileOutputStream("output.txt");
BufferedWriter writer = new BufferedWriter(fos);
writer.writeLine("Hello, World!");
writer.close();
```

**メソッド:**
- `void write(String str)` - 文字列を書き込む
- `void writeLine(String str)` - 文字列を書き込み、改行
- `void newLine()` - 改行
- `void close()` - ストリームを閉じる

### BufferedReader

バッファリングされた文字入力。

```java
FileInputStream fis = new FileInputStream("input.txt");
BufferedReader reader = new BufferedReader(fis);
String line = reader.readLine();
while (line != null) {
    System.println(line);
    line = reader.readLine();
}
reader.close();
```

**メソッド:**
- `int read()` - 1文字を読み込む
- `String readLine()` - 1行を読み込む
- `void close()` - ストリームを閉じる

### 実用例

```java
class FileIOExample {
    public static void main() {
        try {
            // 書き込み
            FileOutputStream fos = new FileOutputStream("output.txt");
            BufferedWriter writer = new BufferedWriter(fos);
            writer.writeLine("Hello, DOSJava!");
            writer.writeLine("Line 2");
            writer.close();
            
            // 読み込み
            FileInputStream fis = new FileInputStream("output.txt");
            BufferedReader reader = new BufferedReader(fis);
            String line = reader.readLine();
            while (line != null) {
                System.println(line);
                line = reader.readLine();
            }
            reader.close();
            
        } catch (Exception e) {
            System.println("I/O error");
        }
        
        return;
    }
}
```

---

## 付録

### データ型

| 型 | サイズ | 範囲 |
|----|--------|------|
| int | 16-bit | -32,768 ～ 32,767 |
| long | 32-bit | -2,147,483,648 ～ 2,147,483,647 |
| float | 32-bit | IEEE 754形式 |
| boolean | - | true / false |
| String | - | 文字列 |

### 演算子

- **算術:** `+`, `-`, `*`, `/`, `%`
- **比較:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- **論理:** `&&`, `||`, `!`
- **代入:** `=`

### 制御構文

```java
// 条件分岐
if (condition) {
    // ...
} else {
    // ...
}

// ループ
while (condition) {
    // ...
}

for (int i = 0; i < 10; i = i + 1) {
    // ...
}

// switch文
switch (value) {
    case 1:
        // ...
        break;
    default:
        // ...
        break;
}
```

### 配列

```java
// 配列の宣言と初期化
int[] numbers = new int[5];
numbers[0] = 10;
numbers[1] = 20;

// 配列の長さ
int len = numbers.length;

// 多次元配列
int[][] matrix = new int[3][3];
matrix[0][0] = 1;
```

---

## クイックリファレンス

### よく使う操作

```java
// 文字列出力
System.println("Hello");

// 数値出力
System.printInt(42);
System.printLong(1234567890L);
System.printFloat(3.14f);

// 文字列操作
String text = "Hello World";
int len = text.length();
String sub = text.substring(0, 5);
String replaced = text.replace("World", "DOS");

// 数値変換
int num = Integer.parseInt("123");

// 数学関数
float result = Math.sqrt(16.0f);
float power = Math.pow(2.0f, 3.0f);

// 日付
Date now = new Date();
int year = now.getFullYear();

// HTTP
String response = Http.get("http://example.com");

// ファイルI/O
FileOutputStream fos = new FileOutputStream("file.txt");
BufferedWriter writer = new BufferedWriter(fos);
writer.writeLine("Hello");
writer.close();

// 例外処理
try {
    // ...
} catch (Exception e) {
    System.println(e.getMessage());
}
```

---

**DOSJava Version 1.0**  
**Last Updated: 2026-05-30**