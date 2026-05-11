# Phase 2: String型完全サポート - 詳細タスクリスト

このドキュメントは [`PHASE2_STRING_PLAN.md`](PHASE2_STRING_PLAN.md) の実装計画を具体的なタスクに分解したものです。

## タスク概要

- **総タスク数**: 約 40 タスク
- **見積もり工数**: 11-16日
- **実装順序**: Step 2.1 → 2.2 → 2.3 → 2.4

---

## Step 2.1: String パラメータのサポート（2-3日）

### 2.1.1 Semantic Analysis の拡張

#### Task 2.1.1.1: パラメータ型チェックの拡張
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_method_declaration()`
- **内容**:
  - パラメータの型チェックで `TYPE_CLASS` (String) を許可
  - String パラメータのシンボルテーブル登録
  - エラーメッセージの更新
- **テスト**: String パラメータを持つメソッドがコンパイルエラーにならないこと

#### Task 2.1.1.2: メソッド呼び出し時の引数型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_call()`
- **内容**:
  - 引数の型チェックで String を許可
  - String 引数と String パラメータの型一致チェック
  - 型不一致時のエラーメッセージ
- **テスト**: String 引数を渡すメソッド呼び出しが型チェックを通過すること

#### Task 2.1.1.3: シンボルテーブルの String パラメータ対応
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **内容**:
  - String パラメータのシンボル情報を正しく格納
  - パラメータインデックスの計算（String は参照型）
- **テスト**: シンボルテーブルに String パラメータが正しく登録されること

### 2.1.2 Code Generation の拡張

#### Task 2.1.2.1: メソッドディスクリプタの String パラメータ対応
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_method_descriptor()`
- **内容**:
  - String パラメータを `Ljava/lang/String;` として追加
  - 複数パラメータの順序を正しく処理
  - ディスクリプタ文字列のバッファオーバーフロー対策
- **テスト**: 生成されたディスクリプタが正しい形式であること

#### Task 2.1.2.2: String 引数のスタック配置
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_call()`
- **内容**:
  - String 引数をスタックに正しく配置
  - String ハンドルのプッシュ
- **テスト**: String 引数が正しくスタックに配置されること

#### Task 2.1.2.3: ローカル変数インデックスの計算
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `get_local_index()`
- **内容**:
  - String パラメータのローカル変数インデックス計算
  - インスタンスメソッドでの `this` を考慮
- **テスト**: String パラメータが正しいローカル変数スロットに配置されること

### 2.1.3 VM の対応確認

#### Task 2.1.3.1: OP_INVOKE_STATIC の String 引数対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `OP_INVOKE_STATIC` が String ハンドルを正しく処理できるか確認
  - 必要に応じて修正
- **テスト**: String 引数を持つ static メソッドが正しく呼び出されること

#### Task 2.1.3.2: OP_INVOKE_VIRTUAL の String 引数対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `OP_INVOKE_VIRTUAL` が String ハンドルを正しく処理できるか確認
  - 必要に応じて修正
- **テスト**: String 引数を持つインスタンスメソッドが正しく呼び出されること

#### Task 2.1.3.3: ディスクリプタ解析の String 対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **関数**: `descriptor_param_count()`
- **内容**:
  - `Ljava/lang/String;` を正しくカウント
  - 既存実装で対応済みか確認
- **テスト**: String パラメータを含むディスクリプタが正しく解析されること

### 2.1.4 テストケース作成

#### Task 2.1.4.1: 基本的な String パラメータテスト
- **ファイル**: `tests/strprm1.jav`
- **内容**: 1つの String パラメータを持つメソッド
- **期待**: コンパイル成功、実行成功

#### Task 2.1.4.2: 複数 String パラメータテスト
- **ファイル**: `tests/strprm2.jav`
- **内容**: 複数の String パラメータを持つメソッド
- **期待**: コンパイル成功、実行成功

#### Task 2.1.4.3: String と int 混在パラメータテスト
- **ファイル**: `tests/strprm3.jav`
- **内容**: String と int を混在させたパラメータ
- **期待**: コンパイル成功、実行成功

### 2.1.5 ビルドと動作確認

#### Task 2.1.5.1: クロスコンパイル
- **コマンド**: `wmake all`
- **確認**: ビルドエラーがないこと

#### Task 2.1.5.2: DOSBox-X での動作確認
- **内容**: 作成したテストケースを DOSBox-X で実行
- **確認**: 期待通りの出力が得られること

---

## Step 2.2: String 戻り値のサポート（2-3日）

### 2.2.1 Semantic Analysis の拡張

#### Task 2.2.1.1: メソッド宣言での String 戻り値許可
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_method_declaration()`
- **内容**:
  - 戻り値型で `TYPE_CLASS` (String) を許可
  - String 戻り値のシンボルテーブル登録
- **テスト**: String を返すメソッドがコンパイルエラーにならないこと

#### Task 2.2.1.2: return 文での String 型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_return()`
- **内容**:
  - String 型の return 値チェック
  - メソッドの戻り値型との一致確認
- **テスト**: String を返す return 文が型チェックを通過すること

#### Task 2.2.1.3: メソッド呼び出しの戻り値型設定
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_call()`
- **内容**:
  - String を返すメソッドの戻り値型を正しく設定
  - 戻り値の代入先との型チェック
- **テスト**: String を返すメソッドの戻り値が正しく扱われること

### 2.2.2 Code Generation の拡張

#### Task 2.2.2.1: メソッドディスクリプタの String 戻り値対応
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_method_descriptor()`
- **内容**:
  - String 戻り値を `Ljava/lang/String;` として追加
  - 戻り値部分の正しい生成
- **テスト**: 生成されたディスクリプタが正しい形式であること

#### Task 2.2.2.2: OP_RETURN での String ハンドル返却
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_return()`
- **内容**:
  - String 型の return 処理
  - String ハンドルのスタック配置
- **テスト**: String を返す return が正しいバイトコードを生成すること

#### Task 2.2.2.3: 呼び出し側での String ハンドル受け取り
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_call()`
- **内容**:
  - String を返すメソッド呼び出し後の処理
  - スタックからの String ハンドル取得
- **テスト**: String 戻り値が正しく受け取られること

### 2.2.3 VM の対応確認

#### Task 2.2.3.1: OP_RETURN の String 対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `OP_RETURN` が String ハンドルを正しく返せるか確認
  - 既存実装で対応済みか確認
- **テスト**: String を返すメソッドが正しく動作すること

#### Task 2.2.3.2: 戻り値のスタック処理確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - 呼び出し側がスタックから String ハンドルを受け取れるか確認
- **テスト**: String 戻り値が正しくスタックに配置されること

### 2.2.4 テストケース作成

#### Task 2.2.4.1: 基本的な String 戻り値テスト
- **ファイル**: `tests/strret1.jav`
- **内容**: String を返すメソッド
- **期待**: コンパイル成功、実行成功

#### Task 2.2.4.2: String 戻り値の連鎖呼び出しテスト
- **ファイル**: `tests/strret2.jav`
- **内容**: String を返すメソッドの戻り値を別のメソッドに渡す
- **期待**: コンパイル成功、実行成功

#### Task 2.2.4.3: String パラメータと戻り値の組み合わせテスト
- **ファイル**: `tests/strret3.jav`
- **内容**: String を受け取り String を返すメソッド
- **期待**: コンパイル成功、実行成功

### 2.2.5 ビルドと動作確認

#### Task 2.2.5.1: クロスコンパイル
- **コマンド**: `wmake all`
- **確認**: ビルドエラーがないこと

#### Task 2.2.5.2: DOSBox-X での動作確認
- **内容**: 作成したテストケースを DOSBox-X で実行
- **確認**: 期待通りの出力が得られること

---

## Step 2.3: String + int 混合演算のサポート（3-4日）

### 2.3.1 Semantic Analysis の拡張

#### Task 2.3.1.1: String + int の型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_binary_op()`
- **内容**:
  - `BINOP_ADD` で `String + int` を許可
  - 結果型を String として設定
- **テスト**: `String + int` が型チェックを通過すること

#### Task 2.3.1.2: int + String の型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_binary_op()`
- **内容**:
  - `BINOP_ADD` で `int + String` を許可
  - 結果型を String として設定
- **テスト**: `int + String` が型チェックを通過すること

#### Task 2.3.1.3: 連鎖的な混合演算の型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_binary_op()`
- **内容**:
  - `"text" + int + "text"` のような連鎖を正しく処理
  - 左から右への評価順序を考慮
- **テスト**: 連鎖的な混合演算が型チェックを通過すること

### 2.3.2 Runtime の拡張

#### Task 2.3.2.1: integer_tostring() の実装確認
- **ファイル**: [`src/runtime/string.c`](src/runtime/string.c)
- **関数**: `string_fromint()`
- **内容**:
  - 既存の `string_fromint()` が正しく動作するか確認
  - 必要に応じて修正
- **テスト**: int から String への変換が正しく動作すること

### 2.3.3 Code Generation の拡張

#### Task 2.3.3.1: String + int のコード生成
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_binary_op()`
- **内容**:
  - int を String に変換するネイティブ呼び出し
  - 2つの String を連結するネイティブ呼び出し
- **テスト**: `String + int` が正しいバイトコードを生成すること

#### Task 2.3.3.2: int + String のコード生成
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_binary_op()`
- **内容**:
  - int を String に変換するネイティブ呼び出し
  - 2つの String を連結するネイティブ呼び出し
- **テスト**: `int + String` が正しいバイトコードを生成すること

#### Task 2.3.3.3: ネイティブメソッドの定数プール登録
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **内容**:
  - `Integer.toString(I)Ljava/lang/String;` を定数プールに追加
  - メソッド参照の正しい生成
- **テスト**: ネイティブメソッドが正しく定数プールに登録されること

### 2.3.4 VM の実装

#### Task 2.3.4.1: native_integer_tostring() の実装
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - int を String に変換するネイティブメソッド
  - `string_fromint()` を使用
  - String ハンドルをスタックにプッシュ
- **テスト**: int から String への変換が正しく動作すること

#### Task 2.3.4.2: ネイティブメソッドテーブルへの登録
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `native_integer_tostring` をネイティブメソッドテーブルに追加
  - メソッド名とディスクリプタの登録
- **テスト**: ネイティブメソッドが正しく呼び出されること

#### Task 2.3.4.3: String 連結の動作確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - 既存の String 連結ネイティブメソッドが正しく動作するか確認
- **テスト**: String 連結が正しく動作すること

### 2.3.5 テストケース作成

#### Task 2.3.5.1: String + int テスト
- **ファイル**: `tests/strint1.jav`
- **内容**: `"text" + 42` のような演算
- **期待**: コンパイル成功、実行成功

#### Task 2.3.5.2: int + String テスト
- **ファイル**: `tests/strint2.jav`
- **内容**: `42 + "text"` のような演算
- **期待**: コンパイル成功、実行成功

#### Task 2.3.5.3: 連鎖的な混合演算テスト
- **ファイル**: `tests/strint3.jav`
- **内容**: `"Count: " + 42 + " items"` のような演算
- **期待**: コンパイル成功、実行成功

#### Task 2.3.5.4: 複雑な混合演算テスト
- **ファイル**: `tests/strint4.jav`
- **内容**: 変数を使った混合演算
- **期待**: コンパイル成功、実行成功

### 2.3.6 ビルドと動作確認

#### Task 2.3.6.1: クロスコンパイル
- **コマンド**: `wmake all`
- **確認**: ビルドエラーがないこと

#### Task 2.3.6.2: DOSBox-X での動作確認
- **内容**: 作成したテストケースを DOSBox-X で実行
- **確認**: 期待通りの出力が得られること

---

## Step 2.4: String フィールドのサポート（2-3日）

### 2.4.1 Semantic Analysis の拡張

#### Task 2.4.1.1: フィールド宣言での String 型許可
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_field_declaration()`
- **内容**:
  - `TYPE_CLASS` (String) をフィールド型として許可
  - String フィールドのシンボルテーブル登録
- **テスト**: String フィールドがコンパイルエラーにならないこと

#### Task 2.4.1.2: フィールドアクセスの String 型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_field_access()`
- **内容**:
  - String フィールドの型情報を返す
  - フィールドアクセスの型チェック
- **テスト**: String フィールドへのアクセスが型チェックを通過すること

#### Task 2.4.1.3: フィールド代入の String 型チェック
- **ファイル**: [`tools/compiler/semantic.c`](tools/compiler/semantic.c)
- **関数**: `check_assignment()`
- **内容**:
  - String フィールドへの代入チェック
  - 代入値の型チェック
- **テスト**: String フィールドへの代入が型チェックを通過すること

### 2.4.2 Code Generation の拡張

#### Task 2.4.2.1: フィールドディスクリプタの String 対応
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_field_descriptor()`
- **内容**:
  - String フィールドを `Ljava/lang/String;` として追加
  - フィールド情報の正しい生成
- **テスト**: 生成されたフィールドディスクリプタが正しい形式であること

#### Task 2.4.2.2: OP_GET_FIELD の String 対応
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_field_access()`
- **内容**:
  - String フィールドの読み取り
  - String ハンドルのスタック配置
- **テスト**: String フィールドの読み取りが正しいバイトコードを生成すること

#### Task 2.4.2.3: OP_PUT_FIELD の String 対応
- **ファイル**: [`tools/compiler/codegen.c`](tools/compiler/codegen.c)
- **関数**: `generate_field_assignment()`
- **内容**:
  - String フィールドの書き込み
  - String ハンドルの格納
- **テスト**: String フィールドの書き込みが正しいバイトコードを生成すること

### 2.4.3 VM の対応確認

#### Task 2.4.3.1: OP_GET_FIELD の String 対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `OP_GET_FIELD` が String ハンドルを正しく読み取れるか確認
  - 既存実装で対応済みか確認
- **テスト**: String フィールドの読み取りが正しく動作すること

#### Task 2.4.3.2: OP_PUT_FIELD の String 対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - `OP_PUT_FIELD` が String ハンドルを正しく書き込めるか確認
  - 既存実装で対応済みか確認
- **テスト**: String フィールドの書き込みが正しく動作すること

#### Task 2.4.3.3: フィールドストレージの String 対応確認
- **ファイル**: [`src/vm/interpreter.c`](src/vm/interpreter.c)
- **内容**:
  - フィールドストレージで String ハンドルを正しく管理できるか確認
  - メモリ管理の確認
- **テスト**: String フィールドが正しく格納・取得されること

### 2.4.4 テストケース作成

#### Task 2.4.4.1: 基本的な String フィールドテスト
- **ファイル**: `tests/strfld1.jav`
- **内容**: String フィールドを持つクラス
- **期待**: コンパイル成功、実行成功

#### Task 2.4.4.2: 複数 String フィールドテスト
- **ファイル**: `tests/strfld2.jav`
- **内容**: 複数の String フィールドを持つクラス
- **期待**: コンパイル成功、実行成功

#### Task 2.4.4.3: String と int フィールド混在テスト
- **ファイル**: `tests/strfld3.jav`
- **内容**: String と int フィールドを混在させたクラス
- **期待**: コンパイル成功、実行成功

### 2.4.5 ビルドと動作確認

#### Task 2.4.5.1: クロスコンパイル
- **コマンド**: `wmake all`
- **確認**: ビルドエラーがないこと

#### Task 2.4.5.2: DOSBox-X での動作確認
- **内容**: 作成したテストケースを DOSBox-X で実行
- **確認**: 期待通りの出力が得られること

---

## 総合テストと最終調整（2-3日）

### Task 2.5.1: 総合テストケースの作成
- **ファイル**: `tests/strall1.jav`
- **内容**: 全機能を組み合わせた総合テスト
- **期待**: コンパイル成功、実行成功

### Task 2.5.2: エッジケースのテスト
- **内容**:
  - 空文字列の処理
  - 長い文字列の処理
  - null チェック（将来の拡張）
- **期待**: 適切に処理されること

### Task 2.5.3: パフォーマンステスト
- **内容**:
  - 大量の String 操作
  - メモリ使用量の確認
- **期待**: 16-bit DOS の制約内で動作すること

### Task 2.5.4: ドキュメント更新
- **ファイル**: [`README.md`](README.md)
- **内容**: Phase 2 の機能を反映

### Task 2.5.5: ドキュメント更新
- **ファイル**: [`COMPILER_IMPLEMENTATION_STATUS.md`](COMPILER_IMPLEMENTATION_STATUS.md)
- **内容**: 実装状況を更新

### Task 2.5.6: テストガイド更新
- **ファイル**: [`tests/TESTING_GUIDE.md`](tests/TESTING_GUIDE.md)
- **内容**: 新しいテストケースの説明を追加

### Task 2.5.7: 最終ビルド確認
- **コマンド**: `wmake clean && wmake all`
- **確認**: クリーンビルドが成功すること

### Task 2.5.8: 全テストケースの実行
- **内容**: 全テストケースを DOSBox-X で実行
- **確認**: 全テストが成功すること

---

## チェックリスト

### Step 2.1 完了チェック
- [ ] String パラメータを持つメソッドがコンパイルできる
- [ ] String 引数を渡すメソッド呼び出しが動作する
- [ ] 複数の String パラメータが正しく処理される
- [ ] String と int の混在パラメータが動作する
- [ ] 全テストケースが成功する

### Step 2.2 完了チェック
- [ ] String を返すメソッドがコンパイルできる
- [ ] String 戻り値が正しく受け取られる
- [ ] String 戻り値の連鎖呼び出しが動作する
- [ ] String パラメータと戻り値の組み合わせが動作する
- [ ] 全テストケースが成功する

### Step 2.3 完了チェック
- [ ] `String + int` が動作する
- [ ] `int + String` が動作する
- [ ] 連鎖的な混合演算が動作する
- [ ] 変数を使った混合演算が動作する
- [ ] 全テストケースが成功する

### Step 2.4 完了チェック
- [ ] String フィールドを持つクラスがコンパイルできる
- [ ] String フィールドへのアクセスが動作する
- [ ] String フィールドへの代入が動作する
- [ ] 複数の String フィールドが正しく処理される
- [ ] 全テストケースが成功する

### Phase 2 完了チェック
- [ ] 全ステップが完了している
- [ ] 総合テストケースが成功する
- [ ] ドキュメントが更新されている
- [ ] ビルド時に警告が出ない
- [ ] DOSBox-X での動作確認が完了している

---

## 進捗管理

### 実装開始日
- 記録: ____年__月__日

### 各ステップの完了日
- Step 2.1: ____年__月__日
- Step 2.2: ____年__月__日
- Step 2.3: ____年__月__日
- Step 2.4: ____年__月__日
- 総合テスト: ____年__月__日

### Phase 2 完了日
- 記録: ____年__月__日

---

## 注意事項

1. **各タスクは順番に実行する**
   - 依存関係があるため、順序を守ること

2. **テストを重視する**
   - 各タスク完了後、必ずテストを実行
   - 問題があれば即座に修正

3. **デバッグ出力を活用する**
   - `[DEBUG]` プレフィックスでデバッグ情報を出力
   - 問題の早期発見に役立てる

4. **メモリ管理に注意**
   - 16-bit DOS の制約を常に意識
   - メモリリークを防ぐ

5. **ドキュメントを随時更新**
   - 実装内容を記録
   - 後から見返せるようにする

6. **定期的にバックアップ**
   - Git commit を活用
   - 重要なマイルストーンで commit

---

## 参考資料

- [`PHASE2_STRING_PLAN.md`](PHASE2_STRING_PLAN.md) - 実装計画
- [`README.md`](README.md) - プロジェクト概要
- [`COMPILER_IMPLEMENTATION_STATUS.md`](COMPILER_IMPLEMENTATION_STATUS.md) - 実装状況
- [`.clinerules`](.clinerules) - コーディング規約