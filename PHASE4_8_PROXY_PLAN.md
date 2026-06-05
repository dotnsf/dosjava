# Phase 4.8: HTTP Proxy Server Support

## 概要

DOSJavaのHttpクラスに、プロクシーサーバー経由でHTTPリクエストを送信する機能を追加します。
`doscurl.exe`の`--proxy`オプションを利用して実装します。

## 目的

- プロクシーサーバーを指定してHTTPリクエストを送信できるようにする
- 企業ネットワークなどプロクシー経由でのインターネットアクセスが必要な環境に対応

## 実装方針

### 1. プロクシー設定の保持

グローバル変数でプロクシーURLを保持：

```c
// http.c
static char proxy_url[256] = "";  // Empty string = no proxy
```

### 2. 新しいネイティブメソッド

#### Http.setProxy(String proxyUrl)
- プロクシーサーバーのURLを設定
- 例: `Http.setProxy("http://proxy.example.com:8080")`
- 空文字列を渡すとプロクシー設定をクリア

#### Http.clearProxy()
- プロクシー設定をクリア
- `Http.setProxy("")`と同等

#### Http.getProxy() → String
- 現在のプロクシー設定を取得
- プロクシーが設定されていない場合は空文字列を返す

### 3. 既存メソッドの拡張

すべてのHTTPメソッドでプロクシー設定を考慮：
- `native_http_get()`
- `native_http_get_with_headers()`
- `native_http_post()`
- `native_http_post_with_headers()`
- `native_http_put()`
- `native_http_put_with_headers()`
- `native_http_delete()`
- `native_http_delete_with_headers()`

プロクシーが設定されている場合、doscurlコマンドに`--proxy <proxy_url>`オプションを追加。

### 4. 実装の詳細

#### バッチファイル生成の修正

現在の実装：
```c
fprintf(batch_file, "doscurl.exe%s -o _http.tmp %s 2>_err.tmp\n", header_opts, url);
```

プロクシー対応後：
```c
if (proxy_url[0] != '\0') {
    fprintf(batch_file, "doscurl.exe --proxy %s%s -o _http.tmp %s 2>_err.tmp\n", 
            proxy_url, header_opts, url);
} else {
    fprintf(batch_file, "doscurl.exe%s -o _http.tmp %s 2>_err.tmp\n", 
            header_opts, url);
}
```

#### ヘルパー関数の追加

```c
/**
 * Build doscurl command with proxy support
 * @param batch_file Output file handle
 * @param method HTTP method (GET, POST, PUT, DELETE)
 * @param url Request URL
 * @param header_opts Header options string (may be empty)
 * @param data_file Data file path (for POST/PUT, may be NULL)
 */
static void write_doscurl_command(FILE* batch_file, const char* method, 
                                   const char* url, const char* header_opts,
                                   const char* data_file);
```

## 使用例

### 基本的な使用

```java
class ProxyDemo {
    public static void main() {
        try {
            // プロクシーサーバーを設定
            Http.setProxy("http://proxy.company.com:8080");
            
            // プロクシー経由でGETリクエスト
            String response = Http.get("http://example.com/api");
            System.out.println(response);
            
            // プロクシー設定をクリア
            Http.clearProxy();
            
            // 直接接続でGETリクエスト
            String response2 = Http.get("http://example.com/api");
            System.out.println(response2);
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        return;
    }
}
```

### プロクシー設定の確認

```java
class ProxyCheck {
    public static void main() {
        // プロクシーを設定
        Http.setProxy("http://proxy.example.com:8080");
        
        // 現在の設定を確認
        String currentProxy = Http.getProxy();
        System.out.println("Current proxy: ");
        System.out.println(currentProxy);
        
        return;
    }
}
```

## 実装ファイル

### 1. src/vm/http.h
- `int native_http_setProxy()` 宣言追加
- `int native_http_clearProxy()` 宣言追加
- `int native_http_getProxy()` 宣言追加

### 2. src/vm/http.c
- グローバル変数 `proxy_url` 追加
- `native_http_setProxy()` 実装
- `native_http_clearProxy()` 実装
- `native_http_getProxy()` 実装
- `write_doscurl_command()` ヘルパー関数実装
- 既存の8つのHTTPメソッドでプロクシー対応

### 3. src/vm/native.c
- プロクシーメソッドの登録
  - `Http.setProxy(String)`
  - `Http.clearProxy()`
  - `Http.getProxy()`

### 4. tools/compiler/semantic.c
- シンボルテーブルにプロクシーメソッドを追加（必要に応じて）

## テストプログラム

### tests/httpprox.jav
```java
class httpprox {
    public static void main() {
        try {
            // Test 1: Set proxy
            System.out.println("Test 1: Set proxy");
            Http.setProxy("http://proxy.example.com:8080");
            String proxy = Http.getProxy();
            System.out.println("Proxy: ");
            System.out.println(proxy);
            
            // Test 2: Clear proxy
            System.out.println("Test 2: Clear proxy");
            Http.clearProxy();
            String proxy2 = Http.getProxy();
            System.out.println("Proxy after clear: ");
            System.out.println(proxy2);
            
            // Test 3: HTTP request with proxy
            System.out.println("Test 3: Request with proxy");
            Http.setProxy("http://proxy.example.com:8080");
            String response = Http.get("http://httpbin.org/get");
            System.out.println("Response: ");
            System.out.println(response);
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        return;
    }
}
```

### samples/httpprox.jav
実用的なプロクシー使用例

## 制限事項

1. **プロクシー認証**: 基本認証のみサポート（doscurlの制限）
   - 認証が必要な場合: `http://user:pass@proxy.example.com:8080`

2. **プロキシURL長**: 最大255文字

3. **グローバル設定**: プロクシー設定はプログラム全体で共有
   - スレッドセーフではない（DOSは単一スレッド環境）

## 実装スケジュール

1. **Day 1**: http.h/http.c の実装
   - プロクシー設定関数の実装
   - 既存メソッドのプロクシー対応

2. **Day 2**: native.c の更新とテスト
   - ネイティブメソッド登録
   - テストプログラム作成・実行

3. **Day 3**: ドキュメント更新
   - README.md更新
   - API_REFERENCE.md更新
   - PHASE4_8_COMPLETION.md作成

## 期待される成果

- プロクシー経由でのHTTP通信が可能になる
- 企業ネットワーク環境での利用が容易になる
- 既存のHTTPメソッドとの互換性を維持
- シンプルで使いやすいAPI

## 参考

- doscurl.exe documentation: `--proxy <[protocol://]host[:port]>`
- Phase 4.7: POST/PUT/DELETE Custom Header Support
- Phase 4 Completion: HTTP Client Functionality