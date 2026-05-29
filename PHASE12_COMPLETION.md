# Phase 12: Exception Line Number Information - 完了報告

## 概要

Phase 12では、例外メッセージに**ソースコードの行番号**を追加する機能を実装しました。これにより、例外が発生した正確な位置を特定できるようになり、デバッグが大幅に容易になりました。

## 実装内容

### Phase 12.1: DJC Format Extension

#### DJCファイルフォーマットの拡張
- **バージョン**: v0x0001 → v0x0002
- **新規追加**: 行番号テーブル（Line Number Table）

#### 行番号テーブルの構造
```c
typedef struct {
    uint16_t pc;        // バイトコードオフセット（Program Counter）
    uint16_t line_no;   // ソースコード行番号
} LineNumberEntry;
```

#### DJCヘッダーの拡張
```c
typedef struct {
    uint16_t magic;                    // 0x444A ('DJ')
    uint16_t version;                  // 0x0002（Phase 12で拡張）
    uint32_t bytecode_size;
    uint16_t constant_pool_count;
    uint16_t method_count;
    uint16_t line_number_table_count;  // 新規追加
} DJCHeader;
```

#### ファイルレイアウト
```
[DJCHeader]
[Bytecode]
[Constant Pool]
[Method Table]
[Line Number Table]  ← Phase 12で追加
```

### Phase 12.2: Compiler Line Number Tracking

#### 行番号追跡機能
コンパイラが各ステートメントのコンパイル時に以下を記録：
- ソースコード行番号
- 対応するバイトコードオフセット（グローバルPC）

#### 重要な修正: グローバルPC計算
**問題**: 初期実装では、行番号テーブルのPC値が不正確でした。

**原因**: `add_line_number_entry()`が`codegen->bytecode->size`（前のメソッドまでのサイズ）のみを使用していた。

**解決策**: 現在のメソッドのローカルオフセットを加算
```c
// 修正後のコード
if (codegen->context && codegen->context->code) {
    current_pc = codegen->bytecode->size + codegen->context->code->size;
} else {
    current_pc = codegen->bytecode->size;
}
```

#### 行番号テーブルの生成
- 各ステートメントの開始時に`update_line_number()`を呼び出し
- 行番号が変わった場合のみエントリを追加（重複排除）
- PC順にソート済み（バイナリサーチ用）

### Phase 12.3: VM Line Number Lookup

#### 例外PC保存機能
**課題**: 例外発生時のPCを正確に保存する必要がある。

**実装**:
```c
// ExecutionContextに追加
uint8_t* exception_pc;      // 例外発生時のPC
int exception_pending;      // 例外処理中フラグ
```

#### exception_pendingフラグの役割
**問題**: catch ブロック実行中に`exception_pc`が上書きされる。

**解決策**: 
1. 例外スロー時に`exception_pending = 1`を設定
2. `interpreter_step()`で`exception_pending`が立っている間は`exception_pc`を更新しない
3. catch ブロックへのジャンプ後に`exception_pending = 0`をクリア

```c
// interpreter_step()での保護
if (!ctx->exception_pending) {
    ctx->exception_pc = ctx->pc;
}
```

#### 行番号ルックアップ
```c
uint16_t djc_get_source_line(DJCFile* file, uint16_t pc) {
    // バイナリサーチで行番号を検索
    // O(log n)の効率的な検索
}
```

#### 例外メッセージのフォーマット
```c
// 行番号付きメッセージの生成
if (line_no > 0) {
    sprintf(ctx->exception_message, "%s (line %u)", base_msg, line_no);
} else {
    strcpy(ctx->exception_message, base_msg);
}
```

### Phase 12.4: Testing and Validation

#### テストプログラム

**tests/excline.jav**: 詳細テストプログラム
- 全6種類の例外をテスト
- 各例外のメッセージと行番号を表示
- デバッグ用

**samples/exctest.jav**: サンプルプログラム
- 全6種類の例外をテスト
- 例外メッセージに"line"が含まれているかをチェック
- 全テスト成功時: "exctest.jav worked correctly."

#### テスト結果（DOSBox-X）
```
1. Testing NullPointerException: (line 41)
2. Testing ArrayIndexOutOfBoundsException: (line 54)
3. Testing NumberFormatException: (line 67)
4. Testing IllegalArgumentException: (line 80)
5. Testing StringIndexOutOfBounds: (line 93)
6. Testing ArithmeticException: (line 108)
```

全ての例外で正確な行番号が報告されました！

### Phase 12.5: Documentation

#### 例外メッセージの形式
```
<例外タイプ> (line <行番号>)
```

例:
- `"NullPointerException (line 42)"`
- `"Array index out of bounds (line 54)"`
- `"Division by zero (line 108)"`

#### 後方互換性
- 旧フォーマット（v0x0001）のDJCファイルも実行可能
- `line_numbers == NULL`の場合は行番号なしで動作
- 既存のプログラムは再コンパイル不要

## 技術的な詳細

### ファイル変更一覧

#### フォーマット層
- `src/format/djc.h`: LineNumberEntry構造体、DJC_VERSION_2定義
- `src/format/djc.c`: 行番号テーブルの読み書き、バイナリサーチ

#### VM層
- `src/vm/interpreter.h`: exception_pc、exception_pendingフィールド追加
- `src/vm/interpreter.c`: PC保存、行番号ルックアップ、フラグ管理

#### コンパイラ層
- `tools/compiler/codegen.h`: LineNumberTable構造体
- `tools/compiler/codegen.c`: 行番号追跡、グローバルPC計算修正

#### テスト
- `tests/excline.jav`: 詳細テストプログラム
- `tests/rexcline.bat`: 実行スクリプト
- `tests/rexcdbg.bat`: デバッグモード実行
- `samples/exctest.jav`: サンプルプログラム

### 主要なアルゴリズム

#### バイナリサーチ（行番号ルックアップ）
```c
int left = 0;
int right = file->line_number_table_count - 1;
int best_match = -1;

while (left <= right) {
    int mid = left + (right - left) / 2;
    if (file->line_numbers[mid].pc <= pc) {
        best_match = mid;
        left = mid + 1;
    } else {
        right = mid - 1;
    }
}
```

時間計算量: O(log n)

#### 例外PC保護メカニズム
```
1. 例外発生
   ↓
2. exception_pc = 現在のPC
   exception_pending = 1
   ↓
3. catch ブロックへジャンプ
   ↓
4. catch ブロック実行中
   （exception_pcは保護される）
   ↓
5. catch ブロック終了
   exception_pending = 0
```

## デバッグの改善

### Before Phase 12
```
Exception caught!
Type: 2
Message: Array index out of bounds
```
→ どの行で例外が発生したか不明

### After Phase 12
```
Exception caught!
Type: 2
Message: Array index out of bounds (line 54)
```
→ 54行目で例外が発生したことが明確！

## 使用例

```java
class ExceptionDemo {
    public static void main() {
        try {
            int[] arr = new int[5];
            int x = arr[10];  // Line 5: 例外発生
        } catch (Exception e) {
            String msg = e.getMessage();
            System.out.println(msg);
            // 出力: "Array index out of bounds (line 5)"
        }
    }
}
```

## パフォーマンス

### メモリオーバーヘッド
- 行番号テーブル: 約4バイト/エントリ
- 典型的なプログラム（100行）: 約400バイト
- 16-bit DOS環境でも十分に小さい

### 実行時オーバーヘッド
- 行番号ルックアップ: O(log n)
- 例外発生時のみ実行（通常実行には影響なし）
- バイナリサーチにより高速

## 制限事項

### 行番号の精度
- ステートメント単位の行番号
- 式の途中での例外は、そのステートメントの開始行を報告
- 複数行にまたがるステートメントは最初の行を報告

### ファイルサイズ
- 行番号テーブルによりDJCファイルが若干大きくなる
- 増加量: 約4バイト × ステートメント数

## 今後の拡張可能性

### 考えられる改善
1. **カラム番号の追加**: 行内の正確な位置を特定
2. **スタックトレース**: 複数のメソッド呼び出しの追跡
3. **ソースファイル名**: 複数ファイルのサポート時に有用

## まとめ

Phase 12により、DOSJavaの例外処理システムが大幅に改善されました：

✅ **実装完了**:
- DJCフォーマットv0x0002
- 行番号テーブルの生成と読み込み
- 例外メッセージへの行番号追加
- 包括的なテスト

✅ **品質保証**:
- 全6種類の例外で正確な行番号報告
- 後方互換性の維持
- 効率的なバイナリサーチ実装

✅ **ユーザビリティ向上**:
- デバッグが容易に
- エラー箇所の即座の特定
- 開発効率の向上

Phase 12は完全に完了し、本番環境で使用可能です！