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