# DOSJava Feature Verification Tests

このディレクトリには、DOSJavaコンパイラ・VMの実装済み機能を検証するためのテストプログラムが含まれています。

## 概要

各テストファイルは特定の機能をテストし、正常に動作した場合は `[ファイル名] worked correctly.` というメッセージを出力します。

## ディレクトリ構成

```
samples/
├── README.md           # このファイル
├── FEATURES.md         # 実装済み機能の詳細ドキュメント
├── features.txt        # テスト一覧（機械可読形式）
├── runtest.bat         # 自動テストランナー（16bit DOS互換）
├── RESULT.TXT          # 最後のテスト実行結果（自動生成）
└── *.jav               # 各機能のテストファイル（全33個）
```

## 使用方法

### 全テストの実行

```batch
cd samples
runtest.bat
```

全33個のテストファイルを順次コンパイル・実行します。結果は画面に表示され、同時に`RESULT.TXT`に保存されます。

**実行例**:
```
DOSJava Feature Verification
============================

[TEST] int.jav
int.jav worked correctly.
[PASS] int.jav

[TEST] bool.jav
bool.jav worked correctly.
[PASS] bool.jav

...（全33テスト）...

============================
All tests completed
============================
```

### 個別テストの実行

```batch
cd samples
runtest.bat int.jav
```

特定のテストファイルのみを実行します。結果は`RESULT.TXT`に保存されます。

**実行例**:
```
Running single test: int.jav

[TEST] int.jav
int.jav worked correctly.
[PASS] int.jav
```

### テスト結果の確認

DOSBox-Xでは画面スクロールができないため、テスト結果は`RESULT.TXT`に保存されます。

```batch
type RESULT.TXT
```

`RESULT.TXT`には直前に実行した`runtest.bat`の結果が保存されています：
- **全テスト実行時**: 全33テストの結果
- **個別テスト実行時**: 指定したテストの結果のみ

### 手動でのテスト実行

```batch
cd samples
..\djc.exe int.jav
..\djvm.exe int.djc
```

## テストファイル一覧

### Phase 1: 基本機能
- `int.jav` - int型の変数と算術演算
- `bool.jav` - boolean型と論理演算
- `if.jav` - if/else文
- `while.jav` - whileループ、break、continue
- `arith.jav` - 算術演算子（+, -, *, /, %）
- `compare.jav` - 比較演算子（==, !=, <, >, <=, >=）
- `logic.jav` - 論理演算子（&&, ||, !）
- `println.jav` - System.out.println

### Phase 2: 文字列とOOP基礎
- `string.jav` - String型、連結、比較
- `strops.jav` - String操作メソッド

### Phase 3: 配列とI/O
- `intarr.jav` - int配列
- `boolarr.jav` - boolean配列
- `fileread.jav` - ファイル読み込み
- `filewrit.jav` - ファイル書き込み

### Phase 5: LONG型
- `long.jav` - long型の変数と演算
- `longarr.jav` - long配列

### Phase 6: 浮動小数点とMath
- `float.jav` - float型の変数と演算
- `floatarr.jav` - float配列
- `cast.jav` - 型変換（キャスト）
- `math.jav` - Math基本関数（abs, min, max, sqrt, pow）
- `trig.jav` - 三角関数（sin, cos, tan, asin, acos, atan）
- `explog.jav` - 指数・対数関数（exp, log, log10）

### Phase 7: 制御構文拡張
- `for.jav` - forループ、break、continue
- `trycatch.jav` - try/catch/finally
- `throw.jav` - throw文
- `swint.jav` - int型switch文
- `swlong.jav` - long型switch文
- `swstring.jav` - String型switch文
- `swnest.jav` - ネストしたswitch文

## テスト結果の見方

### 成功例
```
[TEST] int.jav
[PASS] int.jav
```

### 失敗例
```
[TEST] int.jav
[FAIL] int.jav - Expected output not found
  Expected: int.jav worked correctly.
  Got:
  Error: ...
```

### スキップ例
```
[SKIP] newfeature.jav - File not found
```

## 新しいテストの追加方法

1. **テストファイルを作成**
   - 8.3形式のファイル名（例: `newtest.jav`）
   - 成功時に `newtest.jav worked correctly.` を出力

2. **features.txtに追加**
   ```
   newtest.jav|newtest.jav worked correctly.
   ```

3. **FEATURES.mdを更新**
   - 該当するPhaseセクションに機能説明を追加

4. **features.txtに追加**
   ```
   newtest.jav|newtest.jav worked correctly.
   ```

5. **runtest.batに追加**
   - 全テスト実行セクションに追加:
     ```batch
     :nextN
     set TESTFILE=newtest.jav
     set BASENAME=newtest
     goto run_test
     ```
   - 個別テスト実行セクションに追加:
     ```batch
     if "%1"=="newtest.jav" set BASENAME=newtest
     ```
   - next_testセクションに追加:
     ```batch
     if "%TESTFILE%"=="prevtest.jav" goto nextN
     ```

6. **テストを実行**
   ```batch
   runtest.bat newtest.jav
   type RESULT.TXT
   ```

## 16bit DOS互換性

`runtest.bat`は16bit PC-DOS（DOSBox-X）で動作するように設計されています：

- ✅ サブルーチン呼び出し不使用（`call :label`非対応）
- ✅ 文字列置換不使用（`%VAR:.ext=%`非対応）
- ✅ 算術演算不使用（`set /a`非対応）
- ✅ 遅延環境変数展開不使用（`enabledelayedexpansion`非対応）
- ✅ シンプルなgotoとifのみで実装

## 注意事項

### ファイル名の制約
- DOS互換のため8.3形式（8文字.3文字）を使用
- 拡張子は `.jav`
- 例: `int.jav`, `swstring.jav`, `trycatch.jav`

### テスト設計のガイドライン
- 各テストは独立して実行可能であること
- 外部ファイルへの依存を最小限にすること
- 成功時は必ず期待されるメッセージを出力すること
- 失敗時は何も出力しないか、エラーメッセージを出力すること

### 浮動小数点数の比較
浮動小数点数の比較では、誤差を考慮した範囲チェックを使用してください：
```java
// 悪い例
if (result == 3.14f) { ... }

// 良い例
if (result > 3.13f && result < 3.15f) { ... }
```

### RESULT.TXTについて
- 各テスト実行後、自動的に生成・更新されます
- 画面がスクロールアウトした場合の確認用です
- 直前の`runtest.bat`実行結果のみが保存されます
- 手動で削除しても問題ありません（次回実行時に再生成）

## トラブルシューティング

### コンパイルエラー
- `djc.exe`が親ディレクトリに存在するか確認
- ファイル名が8.3形式か確認
- 構文エラーがないか確認

### 実行エラー
- `djvm.exe`が親ディレクトリに存在するか確認
- `.djc`ファイルが生成されているか確認
- メモリ不足の場合はテストを簡略化

### 画面がスクロールアウトした
- `TYPE RESULT.TXT`でテスト結果を確認
- DOSBox-Xの設定でバッファサイズを増やす

### テストが途中で止まる
- Ctrl+Cで中断された可能性があります
- `runtest.bat`を再実行してください

## 関連ドキュメント

- `FEATURES.md` - 実装済み機能の詳細
- `../README.md` - DOSJavaプロジェクト全体のドキュメント
- `../QUICKSTART.md` - クイックスタートガイド

## ライセンス

このテストスイートはDOSJavaプロジェクトの一部です。