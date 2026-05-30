# Phase 4: HTTP Client Functionality - 完了報告

## 概要

Phase 4では、DOS環境でHTTPクライアント機能を提供する`Http`クラスを実装しました。外部プログラム`doscurl.exe`を統合することで、GET、POST、PUT、DELETEの各HTTPメソッドをサポートし、カスタムヘッダーの送信も可能になりました。

## 実装内容

### Phase 4.1: HTTP Client基本実装

#### 実装したメソッド
1. **Http.get(String url)** - 基本的なGETリクエスト
2. **Http.post(String url, String data)** - POSTリクエスト
3. **Http.put(String url, String data)** - PUTリクエスト
4. **Http.delete(String url)** - DELETEリクエスト

#### 新規ファイル
- `src/vm/http.c` (680+ lines) - HTTPクライアント実装
- `src/vm/http.h` (94 lines) - HTTPクライアントヘッダー

#### 主要機能
- doscurl.exeとの統合（外部プロセス実行）
- 一時ファイルを使用したデータ交換（`_http.tmp`, `_err.tmp`）
- NetworkException (タイプコード6) の追加
- エラーハンドリングとクリーンアップ

### Phase 4.5: カスタムヘッダーサポート

#### 実装したメソッド
5. **Http.get(String url, String headers)** - カスタムヘッダー付きGETリクエスト

#### 主要機能
- 改行区切りヘッダー形式のサポート
  - 例: `"User-Agent: DOSJava/1.0\nAccept: application/json"`
- DOS 127文字コマンドライン制限の回避
  - 一時バッチファイル（`_http.bat`）を使用
  - 長いコマンドラインでも実行可能

#### 技術的な課題と解決策

**問題**: DOSのコマンドライン長制限（127文字）
```
doscurl.exe -H "Header1" -H "Header2" -H "Header3" -o _http.tmp http://example.com/
```
このようなコマンドは約140文字になり、DOSの制限を超える。

**解決策**: 一時バッチファイルの使用
```c
FILE* batch_file = fopen("_http.bat", "w");
fprintf(batch_file, "@echo off\n");
fprintf(batch_file, "doscurl.exe -H \"Header1\" -H \"Header2\" -o _http.tmp URL\n");
fclose(batch_file);
system("_http.bat");
remove("_http.bat");
```

### Phase 4.6: テストとサンプル

#### テストプログラム
1. **tests/httphead.jav** - 包括的なヘッダーテスト（3ヘッダー）
2. **tests/httpsimpl.jav** - シンプルなヘッダーテスト（1ヘッダー）
3. **tests/httpdbg.jav** - デバッグ用詳細テスト

#### サンプルプログラム
**samples/http.jav** - Phase 4機能の検証プログラム
- 6つのテストケース
- 各リクエスト間に2秒の遅延（タイムアウト回避）
- 全テスト成功時のみ "http.jav worked correctly." を出力

```java
class HttpTest {
    public static void delay(int milliseconds) {
        int iterations = milliseconds * 100;
        int dummy = 0;
        for (int i = 0; i < iterations; i = i + 1) {
            dummy = dummy + 1;
        }
        return;
    }
    
    public static void main() {
        boolean allPassed = true;
        
        // Test 1: Basic GET
        try {
            String response1 = Http.get("http://httpbin.org/get");
            if (response1 == null || response1.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);  // 2秒待機
        
        // Test 2-6: その他のテスト...
        
        if (allPassed) {
            System.out.println("http.jav worked correctly.");
        }
        
        return;
    }
}
```

## 実装の詳細

### ファイル構成

```
dosjava/
├── src/vm/
│   ├── http.c          # HTTPクライアント実装
│   └── http.h          # HTTPクライアントヘッダー
├── tests/
│   ├── httphead.jav    # ヘッダーテスト（包括的）
│   ├── httpsimpl.jav   # ヘッダーテスト（シンプル）
│   └── httpdbg.jav     # デバッグテスト
└── samples/
    └── http.jav        # Phase 4検証プログラム
```

### コンパイラ統合

#### semantic.c
- `Http`クラスの5つのメソッド定義を追加
- メソッドオーバーロード対応（`get`メソッドが2つ）

#### codegen.c
- メソッド記述子の生成ロジックを更新
- `get`メソッドの引数数に応じた記述子生成

#### native.c
- 5つのネイティブメソッドを登録
- 正しいメソッド記述子の設定

### エラーハンドリング

#### NetworkException (タイプコード6)
```java
try {
    String response = Http.get("http://example.com/");
} catch (Exception e) {
    int type = e.getType();  // 6
    String msg = e.getMessage();  // "Network error (line XX)"
}
```

#### エラー検出条件
- doscurl.exeの実行失敗
- HTTPリクエストのタイムアウト（10秒）
- ネットワーク接続エラー
- 不正なURL

## テスト結果

### DOSBox-X環境でのテスト

#### 個別テスト
```
> djc.exe httphead.jav
Compiled: httphead.jav -> httphead.djc

> djvm.exe httphead.djc
Test 1: GET with 3 headers
Response received
Test 2: GET with 2 headers
Response received
All tests passed!
```

#### samples/runtest.bat
```
[TEST] http.jav
http.jav worked correctly.
[PASS] http.jav
```

全テストが成功し、Phase 4の実装が正常に動作することを確認しました。

## 技術的な課題と解決

### 課題1: djc_add_string()の戻り値チェック

**問題**: 初期実装で`if (const_idx == 0)`としていたが、これは誤り。
```c
uint16_t const_idx = djc_add_string(ctx, error_msg);
if (const_idx == 0) {  // 誤り！
    return -1;
}
```

**解決**: 正しくは`0xFFFF`をチェック
```c
uint16_t const_idx = djc_add_string(ctx, error_msg);
if (const_idx == 0xFFFF) {  // 正しい
    return -1;
}
```

http.c内の5箇所を修正。

### 課題2: メソッド登録の構文エラー

**問題**: native.cで閉じ括弧の位置が誤っていた。

**解決**: 正しい位置に修正し、メソッド登録が正常に動作。

### 課題3: DOSコマンドライン長制限

**問題**: 複数のヘッダーを含むコマンドが127文字を超える。

**解決**: 一時バッチファイル方式を採用し、任意の長さのコマンドを実行可能に。

### 課題4: 連続HTTPリクエストのタイムアウト

**問題**: 6つのテストを連続実行すると"connection timeout"エラーが発生。

**解決**: 各リクエスト間に2秒の遅延を追加。
```java
public static void delay(int milliseconds) {
    int iterations = milliseconds * 100;
    int dummy = 0;
    for (int i = 0; i < iterations; i = i + 1) {
        dummy = dummy + 1;
    }
    return;
}
```

## 制限事項

### 未実装機能
1. **HTTPステータスコード取得**
   - doscurl.exeの制限により実装困難
   - 終了コードのみで成功/失敗を判定

2. **POST/PUT/DELETEのカスタムヘッダー**
   - 現在はGETメソッドのみカスタムヘッダーをサポート
   - 将来の拡張として実装可能

### 技術的制限
- **外部依存**: doscurl.exeが必要
- **タイムアウト**: 10秒（doscurl.exeの設定）
- **一時ファイル**: `_http.tmp`, `_err.tmp`, `_http.bat`を使用
- **連続リクエスト**: 適切な遅延が必要

## 使用例

### 基本的なGETリクエスト
```java
class SimpleGet {
    public static void main() {
        try {
            String response = Http.get("http://httpbin.org/get");
            System.out.println(response);
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        return;
    }
}
```

### カスタムヘッダー付きGET
```java
class HeaderGet {
    public static void main() {
        try {
            String headers = "User-Agent: DOSJava/1.0\nAccept: application/json";
            String response = Http.get("http://httpbin.org/get", headers);
            System.out.println(response);
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        return;
    }
}
```

### POSTリクエスト
```java
class SimplePost {
    public static void main() {
        try {
            String data = "name=test&value=123";
            String response = Http.post("http://httpbin.org/post", data);
            System.out.println(response);
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        return;
    }
}
```

## まとめ

Phase 4では、DOS環境でHTTPクライアント機能を実装し、以下を達成しました：

### 達成事項
✅ 5つのHTTPメソッドの実装（GET×2, POST, PUT, DELETE）
✅ カスタムヘッダーのサポート
✅ NetworkExceptionの追加
✅ DOSコマンドライン制限の回避
✅ 包括的なテストとサンプルプログラム
✅ ドキュメントの更新

### 技術的成果
- 外部プログラム統合のベストプラクティス確立
- DOS環境の制約を克服する実装パターン
- エラーハンドリングとリソース管理の改善

### 今後の展開
Phase 4の基本実装は完了しましたが、以下の拡張が可能です：
- POST/PUT/DELETEのカスタムヘッダーサポート
- より高度なエラー情報の取得
- リクエストタイムアウトのカスタマイズ

Phase 4は、DOSJavaに実用的なネットワーク機能を追加し、16-bit DOS環境でのWeb APIアクセスを可能にしました。

---

**実装完了日**: 2026年5月30日
**実装者**: K.Kimura @ Juge.Me
**テスト環境**: DOSBox-X on Windows 11

### Phase 4.7: POST/PUT/DELETEカスタムヘッダーサポート ✅ 完了

#### 実装したメソッド
6. **Http.post(String url, String data, String headers)** - カスタムヘッダー付きPOSTリクエスト
7. **Http.put(String url, String data, String headers)** - カスタムヘッダー付きPUTリクエスト
8. **Http.delete(String url, String headers)** - カスタムヘッダー付きDELETEリクエスト

#### 実装の詳細

**新規ネイティブ関数（http.c）**:
- `native_http_post_with_headers()` (約160行)
- `native_http_put_with_headers()` (約160行)
- `native_http_delete_with_headers()` (約165行)

**メソッド登録（native.c）**:
- `Http.post(String, String, String)` - 3パラメータPOST
- `Http.put(String, String, String)` - 3パラメータPUT
- `Http.delete(String, String)` - 2パラメータDELETE

**コンパイラ対応（semantic.c）**:
- 各メソッドのシンボル定義追加（約240行）
- パラメータシンボルの登録

**コード生成（codegen.c）**:
- メソッドディスクリプタ生成の更新
- パラメータ数に応じた分岐処理

#### 技術的な課題と解決策

**問題1**: シンボルテーブルサイズ制限
- Httpクラスに8つのメソッド（各メソッドに複数パラメータ）を追加
- 既存の256シンボル制限を超過
- `semantic_init()` で "Failed to initialize semantic analyzer" エラー

**解決策**: シンボルテーブルとstring poolのサイズ拡張
```c
// symtable.h
typedef struct {
    Symbol symbols[512];        // 256 → 512 (8KB → 16KB)
    char string_pool[4096];     // 2048 → 4096 (2KB → 4KB)
    // ...
} SymbolTable;

// semantic.h
typedef struct {
    char string_pool[4096];     // 2048 → 4096
    // ...
} SemanticAnalyzer;

// semantic.c
static int load_string_pool(SemanticAnalyzer* analyzer) {
    if (analyzer->pool_size > 4096) {  // 2048 → 4096
        return -1;
    }
    // ...
}
```

**問題2**: samples/http.javのコンパイルエラー
- `System.out.println()` の使用（DOSJavaでは未サポート）
- コンパイル時に構文エラー

**解決策**: 不要なデバッグ出力を削除
```java
// 修正前
} catch (Exception e) {
    System.out.println("Exception: " + e);
    allPassed = false;
}

// 修正後
} catch (Exception e) {
    allPassed = false;
}
```

#### テスト結果

**samples/http.jav** - 9つのHTTPテストケース
1. ✅ GET（基本）
2. ✅ GET（単一ヘッダー）
3. ✅ GET（複数ヘッダー）
4. ✅ POST（基本）
5. ✅ POST（ヘッダー付き）- **Phase 4.7で追加**
6. ✅ PUT（基本）
7. ✅ PUT（ヘッダー付き）- **Phase 4.7で追加**
8. ✅ DELETE（基本）
9. ✅ DELETE（ヘッダー付き）- **Phase 4.7で追加**

**実行結果**:
```
> djc.exe http.jav
Compiled: http.jav -> http.djc

> djvm.exe http.djc
http.jav worked correctly.
```

#### コード統計

**Phase 4.7で追加されたコード**:
- http.c: 約485行（3つの新関数）
- http.h: 3つの関数宣言
- native.c: 約54行（3つのメソッド登録）
- semantic.c: 約240行（3つのメソッド定義）
- codegen.c: 約27行（ディスクリプタ生成更新）
- **合計**: 約809行

**Phase 4全体のコード**:
- http.c: 1172行
- http.h: 126行
- native.c: 約108行（HTTP関連）
- semantic.c: 約570行（HTTP関連）
- codegen.c: 約39行（HTTP関連）
- **合計**: 約2015行

#### まとめ

Phase 4.7の完了により、DOSJavaのHttpクラスは以下の8つのメソッドをサポートします：

1. `String get(String url)` - 基本GET
2. `String get(String url, String headers)` - ヘッダー付きGET
3. `String post(String url, String data)` - 基本POST
4. `String post(String url, String data, String headers)` - ヘッダー付きPOST ✨
5. `String put(String url, String data)` - 基本PUT
6. `String put(String url, String data, String headers)` - ヘッダー付きPUT ✨
7. `String delete(String url)` - 基本DELETE
8. `String delete(String url, String headers)` - ヘッダー付きDELETE ✨

これにより、**全HTTPメソッドでカスタムヘッダーのサポートが完了**しました。

#### 今後の展望

Phase 4は完全に完了しました。残る制限事項は以下の通りです：

**制限事項**:
- HTTPステータスコード取得は未実装（doscurlの制限により実装困難）

**可能な拡張**:
- HTTPSサポート（doscurlがサポートしている場合）
- タイムアウト設定
- リダイレクト制御
- より詳細なエラー情報
