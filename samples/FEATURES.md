# DOSJava 実装済み機能一覧

このドキュメントは、DOSJavaコンパイラ・VMで実装済みの機能を記録します。
各機能には対応するテストファイル（samples/ディレクトリ内）が存在します。

## Phase 1: 基本機能

### 1.1 基本データ型
- **int型** (Phase 1)
  - テスト: `int.jav`
  - 32ビット符号付き整数
  - 算術演算、比較演算対応

- **boolean型** (Phase 1)
  - テスト: `bool.jav`
  - true/false値
  - 論理演算対応

### 1.2 基本制御構文
- **if/else文** (Phase 1)
  - テスト: `if.jav`
  - 条件分岐
  - ネスト対応

- **while文** (Phase 1)
  - テスト: `while.jav`
  - 前判定ループ
  - break/continue対応

### 1.3 基本演算子
- **算術演算子** (Phase 1)
  - テスト: `arith.jav`
  - +, -, *, /, % (int型)

- **比較演算子** (Phase 1)
  - テスト: `compare.jav`
  - ==, !=, <, >, <=, >=

- **論理演算子** (Phase 1)
  - テスト: `logic.jav`
  - &&, ||, !

### 1.4 基本I/O
- **System.out.println** (Phase 1)
  - テスト: `println.jav`
  - 文字列・数値の出力

## Phase 2: 文字列とOOP基礎

### 2.1 String型
- **String型** (Phase 2)
  - テスト: `string.jav`
  - 文字列リテラル
  - 文字列連結（+演算子）
  - 文字列比較（equals）

- **String操作** (Phase 2)
  - テスト: `strops.jav`
  - length(), charAt(), substring()
  - indexOf(), lastIndexOf()
  - toUpperCase(), toLowerCase()

## Phase 3: 配列とI/O

### 3.1 配列
- **int配列** (Phase 3)
  - テスト: `intarr.jav`
  - 1次元配列の宣言・初期化・アクセス

- **boolean配列** (Phase 3)
  - テスト: `boolarr.jav`
  - 1次元配列の宣言・初期化・アクセス

### 3.2 ファイルI/O
- **ファイル読み込み** (Phase 3)
  - テスト: `fileread.jav`
  - BufferedReader, FileReader
  - readLine()

- **ファイル書き込み** (Phase 3)
  - テスト: `filewrit.jav`
  - PrintWriter, FileWriter
  - println(), print()

## Phase 5: LONG型と配列拡張

### 5.1 LONG型
- **long型** (Phase 5)
  - テスト: `long.jav`
  - 64ビット符号付き整数
  - 算術演算、比較演算対応

- **long配列** (Phase 5)
  - テスト: `longarr.jav`
  - 1次元配列の宣言・初期化・アクセス

## Phase 6: 浮動小数点とMath

### 6.1 FLOAT型
- **float型** (Phase 6.1)
  - テスト: `float.jav`
  - 32ビット浮動小数点数
  - 算術演算、比較演算対応

- **float配列** (Phase 6.1)
  - テスト: `floatarr.jav`
  - 1次元配列の宣言・初期化・アクセス

### 6.2 型変換
- **キャスト** (Phase 6.2)
  - テスト: `cast.jav`
  - int ↔ long ↔ float
  - 明示的・暗黙的キャスト

### 6.3 Math関数
- **基本Math関数** (Phase 6.3)
  - テスト: `math.jav`
  - abs(), min(), max()
  - sqrt(), pow()

- **三角関数** (Phase 6.3)
  - テスト: `trig.jav`
  - sin(), cos(), tan()
  - asin(), acos(), atan()

- **指数・対数関数** (Phase 6.3)
  - テスト: `explog.jav`
  - exp(), log(), log10()

## Phase 7: 制御構文拡張

### 7.1 for文
- **for文** (Phase 7.1)
  - テスト: `for.jav`
  - 初期化・条件・更新式
  - break/continue対応

### 7.2 例外処理
- **try/catch/finally** (Phase 7.2)
  - テスト: `trycatch.jav`
  - 例外のキャッチ
  - finallyブロック

- **throw文** (Phase 7.2)
  - テスト: `throw.jav`
  - 例外のスロー

### 7.3 switch文
- **int型switch** (Phase 7.3)
  - テスト: `swint.jav`
  - case/default/break
  - 複数case対応

- **long型switch** (Phase 7.3)
  - テスト: `swlong.jav`
  - 64ビット整数のswitch

- **String型switch** (Phase 7.3)
  - テスト: `swstring.jav`
  - 文字列のswitch

- **ネストswitch** (Phase 7.3)
  - テスト: `swnest.jav`
  - switch文のネスト

## 既知の制限事項

### Phase 7.3 switch文
- **Fall-through未実装**: 各caseにbreakが必須
  - 空のcase文からの自動fall-throughは動作しない
  - 将来のPhase 7.4で実装予定

### 配列
- **多次元配列未実装**: 1次元配列のみサポート

### OOP
- **クラス・継承未実装**: staticメソッドのみ
- **オブジェクト生成未実装**: new演算子なし

## テストファイル命名規則

- 8.3形式（DOS互換）
- 拡張子: `.jav`
- 各テストは成功時に `[ファイル名] worked correctly.` を出力

## 検証方法

```batch
cd samples
runtest.bat          REM 全テスト実行
runtest.bat int.jav  REM 個別テスト実行
```

## 更新履歴

- 2026-05-24: Phase 7.3完了時点の機能を記録