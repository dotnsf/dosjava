# Phase 4: HTTP Client Implementation Plan
## DOSJava HTTP Client via doscurl Integration

## 概要

Phase 4では、既存の`doscurl.exe`を活用してDOSJavaにHTTPクライアント機能を実装します。TCP/IPスタックを直接統合する代わりに、外部プロセスとしてdoscurlを呼び出すことで、メモリ制約を回避しながらHTTP機能を提供します。

### 設計方針

**✅ 採用するアプローチ:**
- doscurl.exeを外部プロセスとして実行
- system()呼び出しによるプロセス起動
- 一時ファイル経由でのデータ受け渡し
- 既存のFile I/O実装パターンを踏襲

**❌ 採用しないアプローチ:**
- TCP/IPスタック（mTCP/Wattcp）の直接統合
- ネイティブソケットAPI実装
- Keep-alive接続

### 前提条件

1. **doscurl.exeが利用可能**
   - PATH環境変数に含まれている
   - Phase 7まで完了（GET/POST/リダイレクト/プロキシ対応）
   - Large memory model (-ml) でビルド済み

2. **DOSJava環境**
   - Large memory model (-ml)
   - File I/O機能が動作
   - Exception handling機能が動作

---

## Phase 4.1: 基本HTTP GET実装

### 目標
最もシンプルなHTTP GET機能を実装し、doscurl連携の基本パターンを確立

### 実装内容

#### 4.1.1 Java API設計

```java
class Http {
    /**
     * HTTP GETリクエストを実行
     * @param url リクエスト先URL
     * @return レスポンスボディ（最大256バイト）
     * @throws NetworkException ネットワークエラー時
     */
    public static String get(String url) {
        // Native method呼び出し
    }
}
```

#### 4.1.2 Native Method実装

**ファイル:** `src/vm/http.c` (新規作成)

```c
/**
 * Http.get(String url) の実装
 * doscurl.exeを実行してレスポンスを取得
 */
int native_http_get(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    char cmd[256];
    char response[256];
    FILE* f;
    int exit_code;
    size_t len;
    uint16_t const_idx;
    
    // 引数チェック
    if (arg_count != 1) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_ILLEGAL_ARGUMENT, 
                                       "Http.get requires 1 argument");
    }
    
    // URLを取得
    url = get_string_from_constant_pool(ctx, args[0]);
    if (!url) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NULL_POINTER, 
                                       "URL cannot be null");
    }
    
    // doscurlコマンドを構築
    sprintf(cmd, "doscurl.exe -o _http.tmp %s 2>_err.tmp", url);
    
    // doscurlを実行
    exit_code = system(cmd);
    
    // エラーチェック
    if (exit_code != 0) {
        // エラーファイルを読んでメッセージ取得（オプション）
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK, 
                                       "HTTP request failed");
    }
    
    // レスポンスファイルを読み込み
    f = fopen("_http.tmp", "r");
    if (!f) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO, 
                                       "Failed to read HTTP response");
    }
    
    // 最初の行を読み込み（256バイト制限）
    if (fgets(response, sizeof(response), f) == NULL) {
        fclose(f);
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO, 
                                       "Empty HTTP response");
    }
    
    fclose(f);
    
    // 改行を削除
    len = strlen(response);
    if (len > 0 && response[len-1] == '\n') {
        response[len-1] = '\0';
    }
    
    // 一時ファイル削除
    remove("_http.tmp");
    remove("_err.tmp");
    
    // Stringとして返す
    const_idx = djc_add_string(ctx->djc_file, response);
    if (const_idx == 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY, 
                                       "Failed to create response string");
    }
    
    *result = const_idx;
    return 0;
}
```

#### 4.1.3 ヘッダーファイル

**ファイル:** `src/vm/http.h` (新規作成)

```c
#ifndef HTTP_H
#define HTTP_H

#include "interpreter.h"

/**
 * HTTP client functions using doscurl.exe
 */

/* Native method implementations */
int native_http_get(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result);

#endif /* HTTP_H */
```

#### 4.1.4 Native Method登録

**ファイル:** `src/vm/native.c` (既存ファイルに追加)

```c
#include "http.h"

// register_native_methods() 内に追加
register_native_method("Http", "get", "(Ljava/lang/String;)Ljava/lang/String;", native_http_get);
```

#### 4.1.5 NetworkException追加

**ファイル:** `src/vm/interpreter.h` (既存ファイルに追加)

```c
// Exception types
#define EXCEPTION_TYPE_NETWORK 6  // Network error (HTTP, connection, etc.)
```

**ファイル:** `src/vm/interpreter.c` (既存ファイルに追加)

```c
// get_exception_type_name() 内に追加
case EXCEPTION_TYPE_NETWORK:
    return "NetworkException";
```

#### 4.1.6 Makefileの更新

**ファイル:** `Makefile`

```makefile
# HTTP client support
HTTP_OBJS = build/obj/http.obj

# Add to OBJS
OBJS = ... $(HTTP_OBJS)

# Add build rule
build/obj/http.obj: src/vm/http.c src/vm/http.h
	$(CC) $(CFLAGS) -fo=$@ src/vm/http.c
```

### テストプログラム

#### 4.1.7 基本テスト

**ファイル:** `tests/httpget.jav`

```java
class HttpGetTest {
    public static void main() {
        System.out.println("=== HTTP GET Test ===");
        
        try {
            // Test 1: Simple GET
            System.out.println("Test 1: Simple GET");
            String response = Http.get("http://example.com/");
            System.out.println("Response: ");
            System.out.println(response);
            System.out.println("");
            
            // Test 2: API endpoint
            System.out.println("Test 2: API endpoint");
            String json = Http.get("http://httpbin.org/get");
            System.out.println("JSON: ");
            System.out.println(json);
            System.out.println("");
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        System.out.println("=== Test Complete ===");
    }
}
```

#### 4.1.8 サンプルプログラム

**ファイル:** `samples/http.jav`

```java
class HttpDemo {
    public static void main() {
        try {
            String response = Http.get("http://example.com/");
            System.out.println(response);
            System.out.println("http.jav worked correctly.");
        } catch (Exception e) {
            System.out.println("Error occurred");
        }
    }
}
```

### 成果物

- ✅ `src/vm/http.c` - HTTP client実装
- ✅ `src/vm/http.h` - ヘッダーファイル
- ✅ NetworkException追加
- ✅ `tests/httpget.jav` - テストプログラム
- ✅ `samples/http.jav` - サンプルプログラム
- ✅ Makefile更新

### 検証項目

1. ✅ doscurl.exeが正常に実行される
2. ✅ レスポンスが正しく取得される
3. ✅ エラー時に適切な例外がスローされる
4. ✅ 一時ファイルが正しく削除される
5. ✅ メモリリークがない

---

## Phase 4.2: レスポンス情報の取得

### 目標
HTTPステータスコードとヘッダー情報を取得できるようにする

### 実装内容

#### 4.2.1 Java API設計

```java
class HttpResponse {
    private int statusCode;
    private String body;
    
    public int getStatusCode() {
        // Native method
    }
    
    public String getBody() {
        // Native method
    }
}

class Http {
    /**
     * HTTP GETリクエストを実行（詳細情報付き）
     * @param url リクエスト先URL
     * @return HttpResponseオブジェクト
     * @throws NetworkException ネットワークエラー時
     */
    public static HttpResponse request(String url) {
        // Native method呼び出し
    }
}
```

#### 4.2.2 Native Method実装

**ファイル:** `src/vm/http.c` (追加)

```c
/**
 * Http.request(String url) の実装
 * doscurl -v を使用してヘッダー情報も取得
 */
int native_http_request(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    char cmd[256];
    char line[256];
    FILE* f;
    int exit_code;
    int status_code = 0;
    char body[256];
    int in_body = 0;
    uint16_t response_obj;
    
    // URLを取得
    url = get_string_from_constant_pool(ctx, args[0]);
    
    // doscurl -v を実行（ヘッダー付き）
    sprintf(cmd, "doscurl.exe -v -o _http.tmp %s 2>_err.tmp", url);
    exit_code = system(cmd);
    
    if (exit_code != 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK, 
                                       "HTTP request failed");
    }
    
    // レスポンスファイルを解析
    f = fopen("_http.tmp", "r");
    if (!f) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO, 
                                       "Failed to read HTTP response");
    }
    
    // ヘッダーとボディを分離
    while (fgets(line, sizeof(line), f)) {
        if (!in_body) {
            // ステータスラインをパース
            if (strncmp(line, "HTTP/", 5) == 0) {
                // "HTTP/1.1 200 OK" から 200 を抽出
                sscanf(line, "HTTP/%*s %d", &status_code);
            }
            // 空行でボディ開始
            if (line[0] == '\n' || line[0] == '\r') {
                in_body = 1;
            }
        } else {
            // ボディの最初の行を保存
            strncpy(body, line, sizeof(body)-1);
            body[sizeof(body)-1] = '\0';
            break;
        }
    }
    
    fclose(f);
    remove("_http.tmp");
    remove("_err.tmp");
    
    // HttpResponseオブジェクトを作成
    response_obj = create_http_response_object(ctx, status_code, body);
    if (response_obj == 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_OUT_OF_MEMORY, 
                                       "Failed to create HttpResponse object");
    }
    
    *result = response_obj;
    return 0;
}

/**
 * HttpResponseオブジェクトを作成
 */
uint16_t create_http_response_object(ExecutionContext* ctx, int status_code, const char* body) {
    uint16_t obj_handle;
    uint16_t body_idx;
    
    // オブジェクト作成（簡略化 - 実際はobject.cの機能を使用）
    obj_handle = allocate_object(ctx, "HttpResponse");
    if (obj_handle == 0) {
        return 0;
    }
    
    // statusCodeフィールドを設定
    set_object_field_int(ctx, obj_handle, "statusCode", status_code);
    
    // bodyフィールドを設定
    body_idx = djc_add_string(ctx->djc_file, body);
    set_object_field_string(ctx, obj_handle, "body", body_idx);
    
    return obj_handle;
}
```

#### 4.2.3 テストプログラム

**ファイル:** `tests/httpresp.jav`

```java
class HttpResponseTest {
    public static void main() {
        System.out.println("=== HTTP Response Test ===");
        
        try {
            HttpResponse resp = Http.request("http://example.com/");
            
            int status = resp.getStatusCode();
            System.out.println("Status: ");
            System.out.println(status);
            
            String body = resp.getBody();
            System.out.println("Body: ");
            System.out.println(body);
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        System.out.println("=== Test Complete ===");
    }
}
```

### 成果物

- ✅ HttpResponseクラス実装
- ✅ `native_http_request()` 実装
- ✅ レスポンス解析機能
- ✅ テストプログラム

---

## Phase 4.3: POST/その他HTTPメソッド

### 目標
POST、PUT、DELETEメソッドをサポート

### 実装内容

#### 4.3.1 Java API設計

```java
class Http {
    /**
     * HTTP POSTリクエスト
     * @param url リクエスト先URL
     * @param data POSTデータ
     * @return レスポンスボディ
     */
    public static String post(String url, String data) {
        // Native method
    }
    
    /**
     * HTTP PUTリクエスト
     */
    public static String put(String url, String data) {
        // Native method
    }
    
    /**
     * HTTP DELETEリクエスト
     */
    public static String delete(String url) {
        // Native method
    }
}
```

#### 4.3.2 Native Method実装

**ファイル:** `src/vm/http.c` (追加)

```c
/**
 * Http.post(String url, String data) の実装
 */
int native_http_post(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    const char* url;
    const char* data;
    char cmd[512];
    char response[256];
    FILE* f;
    int exit_code;
    
    // 引数取得
    url = get_string_from_constant_pool(ctx, args[0]);
    data = get_string_from_constant_pool(ctx, args[1]);
    
    // doscurl -X POST -d "data" url
    sprintf(cmd, "doscurl.exe -X POST -d \"%s\" -o _http.tmp %s 2>_err.tmp", 
            data, url);
    
    exit_code = system(cmd);
    
    if (exit_code != 0) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_NETWORK, 
                                       "HTTP POST failed");
    }
    
    // レスポンス読み込み（Http.get()と同様）
    f = fopen("_http.tmp", "r");
    if (!f) {
        return throw_runtime_exception(ctx, EXCEPTION_TYPE_IO, 
                                       "Failed to read HTTP response");
    }
    
    fgets(response, sizeof(response), f);
    fclose(f);
    
    remove("_http.tmp");
    remove("_err.tmp");
    
    *result = djc_add_string(ctx->djc_file, response);
    return 0;
}
```

#### 4.3.3 テストプログラム

**ファイル:** `tests/httppost.jav`

```java
class HttpPostTest {
    public static void main() {
        System.out.println("=== HTTP POST Test ===");
        
        try {
            String response = Http.post("http://httpbin.org/post", 
                                       "name=test&value=123");
            System.out.println("Response: ");
            System.out.println(response);
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        System.out.println("=== Test Complete ===");
    }
}
```

### 成果物

- ✅ POST/PUT/DELETE実装
- ✅ テストプログラム
- ✅ サンプルプログラム

---

## Phase 4.4: 高度な機能（オプション）

### 目標
カスタムヘッダー、Basic認証、タイムアウト制御などの高度な機能

### 実装内容

#### 4.4.1 Java API設計

```java
class Http {
    /**
     * カスタムヘッダー付きGET
     * @param url リクエスト先URL
     * @param headers ヘッダー配列（例: ["Authorization: Bearer token"]）
     */
    public static String getWithHeaders(String url, String[] headers) {
        // Native method
    }
    
    /**
     * Basic認証付きGET
     */
    public static String getWithAuth(String url, String username, String password) {
        // Native method
    }
    
    /**
     * タイムアウト指定GET
     */
    public static String getWithTimeout(String url, int seconds) {
        // Native method
    }
}
```

### 成果物

- ✅ 高度な機能実装
- ✅ テストプログラム

---

## 制約事項と対策

### 1. レスポンスサイズ制限

**制約:**
- doscurl: 最大64KB
- DOSJava String buffer: 256バイト

**対策A: ファイル経由**
```java
class Http {
    // レスポンスをファイルに保存
    public static void getToFile(String url, String filename) {
        // doscurl -o filename url
    }
}

// 使用例
Http.getToFile("http://example.com/large.json", "data.txt");
BufferedReader reader = new BufferedReader(new FileInputStream("data.txt"));
```

**対策B: プレビュー機能**
```java
class Http {
    // 最初の256バイトのみ取得
    public static String getPreview(String url) {
        // 先頭部分のみ返す
    }
}
```

### 2. パフォーマンス

**制約:**
- 外部プロセス起動: 500ms～2秒
- ファイルI/O: 100ms～500ms
- **合計: 1～3秒/リクエスト**

**対策:**
- キャッシング機構
- バッチリクエスト
- 非同期処理（将来的に）

### 3. エラーハンドリング

**必要な例外:**
```c
EXCEPTION_TYPE_NETWORK = 6  // ネットワークエラー

// エラーケース
- doscurl.exeが見つからない
- ネットワーク接続エラー
- HTTPエラー（4xx, 5xx）
- タイムアウト
- レスポンスが大きすぎる
```

---

## 開発スケジュール

| Phase | 内容 | 推定工数 | 優先度 |
|-------|------|----------|--------|
| Phase 4.1 | 基本GET実装 | 2-3日 | 必須 |
| Phase 4.2 | レスポンス解析 | 2-3日 | 必須 |
| Phase 4.3 | POST/その他 | 1-2日 | 必須 |
| Phase 4.4 | 高度な機能 | 2-3日 | オプション |
| **合計** | | **5-8日（基本）** | |

---

## 検証計画

### 単体テスト
- ✅ Http.get() - 基本的なGET
- ✅ Http.request() - レスポンス情報取得
- ✅ Http.post() - POSTリクエスト
- ✅ エラーハンドリング - 各種エラーケース

### 統合テスト
- ✅ 複数リクエストの連続実行
- ✅ 大きなレスポンスの処理
- ✅ エラーリカバリー
- ✅ メモリリークチェック

### DOSBox-Xテスト
- ✅ 実環境での動作確認
- ✅ パフォーマンス測定
- ✅ 安定性確認

---

## 成果物

### コード
- ✅ `src/vm/http.c` - HTTP client実装
- ✅ `src/vm/http.h` - ヘッダーファイル
- ✅ NetworkException追加

### テスト
- ✅ `tests/httpget.jav` - GET test
- ✅ `tests/httpresp.jav` - Response test
- ✅ `tests/httppost.jav` - POST test

### サンプル
- ✅ `samples/http.jav` - 基本サンプル
- ✅ `samples/httpapi.jav` - API呼び出しサンプル

### ドキュメント
- ✅ `PHASE4_HTTP_PLAN.md` - この計画書
- ✅ `PHASE4_HTTP_COMPLETION.md` - 完了レポート
- ✅ README.md更新

---

## 次のステップ

1. Phase 4.1から実装開始
2. 各Phaseごとに動作確認
3. テストプログラムで検証
4. DOSBox-Xで実環境テスト
5. ドキュメント作成

---

*Phase 4 HTTP Client Implementation Plan - 2026-05-29*