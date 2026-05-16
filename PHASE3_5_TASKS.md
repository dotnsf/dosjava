# Phase 3.5: Exception Handling and Date Support - Task Checklist

## Phase 3.5.1: Exception Handling (Day 1-5)

### Day 1-2: Compiler Support for Exception Syntax

#### Lexer Updates
- [ ] `tools/compiler/lexer.h` の更新
  - [ ] TOKEN_TRY定義追加
  - [ ] TOKEN_CATCH定義追加
  - [ ] TOKEN_FINALLY定義追加
  - [ ] TOKEN_THROW定義追加

- [ ] `tools/compiler/lexer.c` の更新
  - [ ] "try" キーワード認識
  - [ ] "catch" キーワード認識
  - [ ] "finally" キーワード認識
  - [ ] "throw" キーワード認識
  - [ ] キーワードテーブルへの追加

#### AST Node Types
- [ ] `tools/compiler/ast.h` の更新
  - [ ] NODE_TRY_STMT定義追加
  - [ ] NODE_CATCH_CLAUSE定義追加
  - [ ] NODE_FINALLY_BLOCK定義追加
  - [ ] NODE_THROW_STMT定義追加
  - [ ] TryStmtNode構造体定義
  - [ ] CatchClauseNode構造体定義
  - [ ] FinallyBlockNode構造体定義
  - [ ] ThrowStmtNode構造体定義

#### Parser Implementation
- [ ] `tools/compiler/parser.c` の更新
  - [ ] parse_try_stmt()関数実装
    - [ ] 'try'キーワードのパース
    - [ ] tryブロックのパース
    - [ ] catchクローズのパース（オプション）
    - [ ] finallyブロックのパース（オプション）
    - [ ] catch/finallyの存在チェック
    - [ ] ASTノード作成
  
  - [ ] parse_catch_clause()関数実装
    - [ ] 'catch'キーワードのパース
    - [ ] '('のパース
    - [ ] 例外型のパース（"Exception"）
    - [ ] 例外変数名のパース
    - [ ] ')'のパース
    - [ ] catchブロックのパース
    - [ ] ASTノード作成
  
  - [ ] parse_finally_block()関数実装
    - [ ] 'finally'キーワードのパース
    - [ ] finallyブロックのパース
    - [ ] ASTノード作成
  
  - [ ] parse_throw_stmt()関数実装
    - [ ] 'throw'キーワードのパース
    - [ ] 例外式のパース
    - [ ] ';'のパース
    - [ ] ASTノード作成
  
  - [ ] parse_statement()への統合
    - [ ] TOKEN_TRYの処理追加
    - [ ] TOKEN_THROWの処理追加

#### Exception Table Generation
- [ ] `tools/compiler/codegen.h` の更新
  - [ ] ExceptionHandler構造体定義
  - [ ] 例外テーブル管理構造追加

- [ ] `tools/compiler/codegen.c` の更新
  - [ ] codegen_try_stmt()関数実装
    - [ ] tryブロックのコード生成
    - [ ] catchブロックのコード生成
    - [ ] finallyブロックのコード生成
    - [ ] ジャンプ命令の生成
    - [ ] 例外ハンドラの登録
  
  - [ ] codegen_catch_clause()関数実装
    - [ ] 例外変数への代入
    - [ ] catchブロックのコード生成
  
  - [ ] codegen_throw_stmt()関数実装
    - [ ] 例外式の評価
    - [ ] OP_THROW命令の生成
  
  - [ ] add_exception_handler()関数実装
    - [ ] 例外ハンドラの追加
    - [ ] 例外テーブルの管理
  
  - [ ] write_exception_table()関数実装
    - [ ] 例外テーブルのバイトコードへの書き込み

#### Compilation Test
- [ ] 簡単なtry-catchプログラムのコンパイル
- [ ] コンパイルエラーのチェック
- [ ] 生成されたバイトコードの確認

**Deliverables:**
- 更新されたlexer.h/lexer.c
- 更新されたast.h
- 更新されたparser.c
- 更新されたcodegen.h/codegen.c
- コンパイル成功確認

### Day 3-4: VM Exception Handling

#### New Opcodes
- [ ] `src/format/opcodes.h` の更新
  - [ ] OP_TRY_BEGIN定義追加 (0x90)
  - [ ] OP_TRY_END定義追加 (0x91)
  - [ ] OP_CATCH_BEGIN定義追加 (0x92)
  - [ ] OP_CATCH_END定義追加 (0x93)
  - [ ] OP_FINALLY_BEGIN定義追加 (0x94)
  - [ ] OP_FINALLY_END定義追加 (0x95)
  - [ ] OP_THROW定義追加 (0x96)

- [ ] `src/format/opcodes.c` の更新
  - [ ] opcode_name()への追加
  - [ ] opcode_length()への追加

#### Exception Handler Structure
- [ ] `src/vm/interpreter.c` の更新
  - [ ] ExceptionHandler構造体定義
  - [ ] ExceptionContext構造体定義
  - [ ] VM構造体への追加
    - [ ] exception_ctx フィールド
    - [ ] try_depth フィールド
    - [ ] pending_catch_pc フィールド

#### Exception Handling Implementation
- [ ] `src/vm/interpreter.c` の実装
  - [ ] vm_throw_exception()関数実装
    - [ ] 例外状態の設定
    - [ ] 例外ハンドラの検索
    - [ ] finallyブロックへのジャンプ
    - [ ] catchブロックへのジャンプ
    - [ ] 未処理例外の処理
  
  - [ ] find_exception_handler()関数実装
    - [ ] PCに対応するハンドラの検索
    - [ ] 例外テーブルの走査
  
  - [ ] load_exception_table()関数実装
    - [ ] バイトコードから例外テーブルの読み込み
    - [ ] ExceptionContextへの格納
  
  - [ ] OP_THROW実装
    - [ ] スタックから例外値をポップ
    - [ ] vm_throw_exception()呼び出し
  
  - [ ] OP_TRY_BEGIN実装
    - [ ] tryブロック開始マーク
    - [ ] try_depthインクリメント
  
  - [ ] OP_TRY_END実装
    - [ ] tryブロック終了マーク
    - [ ] try_depthデクリメント
    - [ ] finallyブロックへのジャンプ（必要な場合）
  
  - [ ] OP_CATCH_BEGIN実装
    - [ ] catchブロック開始マーク
  
  - [ ] OP_CATCH_END実装
    - [ ] catchブロック終了マーク
  
  - [ ] OP_FINALLY_BEGIN実装
    - [ ] finallyブロック開始マーク
  
  - [ ] OP_FINALLY_END実装
    - [ ] finallyブロック終了マーク
    - [ ] pending catchへのジャンプ処理

#### VM Initialization
- [ ] vm_init()の更新
  - [ ] ExceptionContextの初期化
  - [ ] 例外テーブルのロード

#### VM Cleanup
- [ ] vm_cleanup()の更新
  - [ ] 例外関連リソースの解放

**Deliverables:**
- 更新されたopcodes.h/opcodes.c
- 更新されたinterpreter.c
- 例外処理機能の実装完了

### Day 5: Exception Testing

#### Basic Exception Tests
- [ ] `tests/exception/` ディレクトリ作成

- [ ] `tests/exception/test_basic.jav` 作成
  - [ ] Test 1: 基本的なtry-catch
  - [ ] Test 2: try-finallyのみ（例外なし）
  - [ ] Test 3: try-catch-finally（例外あり）
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

- [ ] `tests/exception/test_throw.jav` 作成
  - [ ] Test 1: throw文の基本動作
  - [ ] Test 2: throw後のコードが実行されないこと
  - [ ] Test 3: catchで例外をキャッチ
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Nested Exception Tests
- [ ] `tests/exception/test_nested.jav` 作成
  - [ ] Test 1: ネストしたtry-catch
  - [ ] Test 2: 内側の例外処理
  - [ ] Test 3: 外側の例外処理
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Finally Guarantee Tests
- [ ] `tests/exception/test_finally.jav` 作成
  - [ ] Test 1: finallyが常に実行されること
  - [ ] Test 2: return前のfinally実行
  - [ ] Test 3: 例外発生時のfinally実行
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Edge Case Tests
- [ ] `tests/exception/test_edge.jav` 作成
  - [ ] Test 1: 未処理例外
  - [ ] Test 2: catchブロック内での例外
  - [ ] Test 3: finallyブロック内での例外
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Test Documentation
- [ ] `tests/exception/README.md` 作成
  - [ ] テスト概要
  - [ ] 各テストの説明
  - [ ] 実行方法
  - [ ] 期待される結果

#### Regression Testing
- [ ] 既存のテストが全てパスすることを確認
- [ ] メモリリークチェック
- [ ] パフォーマンス測定

**Deliverables:**
- tests/exception/test_basic.jav
- tests/exception/test_throw.jav
- tests/exception/test_nested.jav
- tests/exception/test_finally.jav
- tests/exception/test_edge.jav
- tests/exception/README.md
- 全テストパス確認

## Phase 3.5.2: Date Support (Day 6-8)

### Day 6: DOS Time API Integration

#### DOS Time API Wrapper
- [ ] `src/runtime/dostime.h` 作成
  - [ ] DOSDateTime構造体定義
  - [ ] dos_get_datetime()宣言
  - [ ] dos_datetime_to_timestamp()宣言
  - [ ] dos_timestamp_to_datetime()宣言

- [ ] `src/runtime/dostime.c` 作成
  - [ ] dos_get_datetime()実装
    - [ ] INT 21h, AH=2Ah (Get Date)
    - [ ] INT 21h, AH=2Ch (Get Time)
    - [ ] DOSDateTime構造体への格納
  
  - [ ] dos_datetime_to_timestamp()実装
    - [ ] 1970-01-01からの日数計算
    - [ ] 秒への変換
    - [ ] ミリ秒への変換
  
  - [ ] dos_timestamp_to_datetime()実装
    - [ ] タイムスタンプから日時への変換
    - [ ] DOSDateTime構造体への格納

#### DOS Time API Testing
- [ ] `tests/dostime/test_dostime.c` 作成
  - [ ] Test 1: dos_get_datetime()
  - [ ] Test 2: dos_datetime_to_timestamp()
  - [ ] Test 3: dos_timestamp_to_datetime()
  - [ ] Test 4: 往復変換の精度確認

- [ ] Makefileの更新
  - [ ] dostime.cのコンパイル設定
  - [ ] test_dostimeターゲット追加

- [ ] ビルドとテスト
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 結果確認

**Deliverables:**
- src/runtime/dostime.h
- src/runtime/dostime.c
- tests/dostime/test_dostime.c
- DOS時刻API動作確認

### Day 7: Date Class Implementation

#### Date Object Structure
- [ ] `src/runtime/date.h` 作成
  - [ ] DateObject構造体定義
  - [ ] date_new()宣言
  - [ ] date_new_with_time()宣言
  - [ ] date_get_time()宣言
  - [ ] date_set_time()宣言
  - [ ] date_get_full_year()宣言
  - [ ] date_get_month()宣言
  - [ ] date_get_date()宣言
  - [ ] date_get_hours()宣言
  - [ ] date_get_minutes()宣言
  - [ ] date_get_seconds()宣言
  - [ ] date_to_string()宣言

- [ ] `src/runtime/date.c` 作成
  - [ ] date_new()実装
    - [ ] DateObjectの割り当て
    - [ ] 現在時刻の取得
    - [ ] 初期化
  
  - [ ] date_new_with_time()実装
    - [ ] DateObjectの割り当て
    - [ ] 指定時刻での初期化
  
  - [ ] date_get_time()実装
    - [ ] time_msの返却
  
  - [ ] date_set_time()実装
    - [ ] time_msの設定
    - [ ] キャッシュの無効化
  
  - [ ] ensure_cache()実装
    - [ ] キャッシュの有効性チェック
    - [ ] 必要に応じて再計算
  
  - [ ] date_get_full_year()実装
    - [ ] キャッシュの確保
    - [ ] 年の返却
  
  - [ ] date_get_month()実装
    - [ ] キャッシュの確保
    - [ ] 月の返却（0-based）
  
  - [ ] date_get_date()実装
    - [ ] キャッシュの確保
    - [ ] 日の返却
  
  - [ ] date_get_hours()実装
    - [ ] キャッシュの確保
    - [ ] 時の返却
  
  - [ ] date_get_minutes()実装
    - [ ] キャッシュの確保
    - [ ] 分の返却
  
  - [ ] date_get_seconds()実装
    - [ ] キャッシュの確保
    - [ ] 秒の返却
  
  - [ ] date_to_string()実装
    - [ ] キャッシュの確保
    - [ ] フォーマット済み文字列の生成

#### VM Integration
- [ ] `src/vm/interpreter.c` の更新
  - [ ] OP_NEWでのDate object作成
  - [ ] Date methodsのnative実装
    - [ ] Date.getTime()
    - [ ] Date.setTime()
    - [ ] Date.getFullYear()
    - [ ] Date.getMonth()
    - [ ] Date.getDate()
    - [ ] Date.getHours()
    - [ ] Date.getMinutes()
    - [ ] Date.getSeconds()
    - [ ] Date.toString()

#### Compiler Support
- [ ] `tools/compiler/semantic.c` の更新
  - [ ] Date型の認識
  - [ ] Dateメソッドの型チェック

- [ ] `tools/compiler/codegen.c` の更新
  - [ ] Date constructorのコード生成
  - [ ] Date methodsのコード生成

#### Makefile Updates
- [ ] Makefileの更新
  - [ ] date.cのコンパイル設定
  - [ ] dostime.cとのリンク
  - [ ] 依存関係の設定

**Deliverables:**
- src/runtime/date.h
- src/runtime/date.c
- 更新されたinterpreter.c
- 更新されたsemantic.c
- 更新されたcodegen.c
- 更新されたMakefile

### Day 8: Date Testing and Documentation

#### Date Basic Tests
- [ ] `tests/date/` ディレクトリ作成

- [ ] `tests/date/test_date.jav` 作成
  - [ ] Test 1: Date作成（現在時刻）
  - [ ] Test 2: 時刻コンポーネントの取得
  - [ ] Test 3: getTime/setTime
  - [ ] Test 4: 時刻差分の計算
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

- [ ] `tests/date/test_date_ops.jav` 作成
  - [ ] Test 1: Date比較
  - [ ] Test 2: Date演算
  - [ ] Test 3: toString()
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Integration Tests
- [ ] `tests/date/test_date_exception.jav` 作成
  - [ ] Test 1: Dateと例外処理の組み合わせ
  - [ ] Test 2: try-finallyでのDate使用
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

- [ ] `tests/date/test_date_io.jav` 作成
  - [ ] Test 1: DateとI/Oの組み合わせ
  - [ ] Test 2: ログへのタイムスタンプ出力
  - [ ] コンパイル確認
  - [ ] 実行確認
  - [ ] 出力確認

#### Performance Tests
- [ ] `tests/date/test_date_perf.jav` 作成
  - [ ] Test 1: Date作成のパフォーマンス
  - [ ] Test 2: getTime()のパフォーマンス
  - [ ] Test 3: メモリ使用量
  - [ ] 実行確認
  - [ ] 結果測定

#### Test Documentation
- [ ] `tests/date/README.md` 作成
  - [ ] テスト概要
  - [ ] 各テストの説明
  - [ ] 実行方法
  - [ ] 期待される結果
  - [ ] パフォーマンス結果

#### API Documentation
- [ ] `PHASE3_5_DATE_API.md` 作成
  - [ ] Date classの概要
  - [ ] コンストラクタ
  - [ ] メソッド一覧
  - [ ] 使用例
  - [ ] 制限事項

#### Regression Testing
- [ ] 既存のテストが全てパスすることを確認
- [ ] 例外処理テストの再実行
- [ ] メモリリークチェック
- [ ] パフォーマンス測定

**Deliverables:**
- tests/date/test_date.jav
- tests/date/test_date_ops.jav
- tests/date/test_date_exception.jav
- tests/date/test_date_io.jav
- tests/date/test_date_perf.jav
- tests/date/README.md
- PHASE3_5_DATE_API.md
- 全テストパス確認

## Final Integration and Documentation

### Integration Testing
- [ ] 例外処理とDateの統合テスト
- [ ] 既存機能との互換性確認
- [ ] メモリ使用量の測定
- [ ] パフォーマンスの測定

### Documentation
- [ ] `PHASE3_5_SUMMARY.md` 作成
  - [ ] 実装内容のサマリー
  - [ ] 追加された機能
  - [ ] API変更点
  - [ ] 制限事項
  - [ ] 既知の問題

- [ ] `PHASE3_5_QUICKSTART.md` 作成
  - [ ] クイックスタートガイド
  - [ ] 例外処理の使い方
  - [ ] Dateの使い方
  - [ ] サンプルコード

- [ ] README.mdの更新
  - [ ] Phase 3.5の追加
  - [ ] 機能一覧の更新

### Code Cleanup
- [ ] デバッグコードの削除
- [ ] コメントの整理
- [ ] コーディング規約の確認
- [ ] 未使用コードの削除

### Final Testing
- [ ] 全テストスイートの実行
  - [ ] Phase 1テスト
  - [ ] Phase 2テスト
  - [ ] Phase 3テスト
  - [ ] Phase 3.5テスト
- [ ] DOSBox-Xでの動作確認
- [ ] メモリリークの最終確認
- [ ] パフォーマンスの最終測定

### Phase 4 Preparation
- [ ] Phase 4で使用する例外処理の確認
- [ ] Phase 4で使用するDate機能の確認
- [ ] Phase 4実装計画の見直し
- [ ] Phase 4開始準備完了

## Progress Tracking

### Phase 3.5.1: Exception Handling
- [ ] Day 1-2: Compiler Support (0%)
  - [ ] Lexer updates
  - [ ] AST node types
  - [ ] Parser implementation
  - [ ] Exception table generation
- [ ] Day 3-4: VM Exception Handling (0%)
  - [ ] New opcodes
  - [ ] Exception handler structure
  - [ ] Exception handling implementation
- [ ] Day 5: Exception Testing (0%)
  - [ ] Basic tests
  - [ ] Nested tests
  - [ ] Finally tests
  - [ ] Edge case tests

### Phase 3.5.2: Date Support
- [ ] Day 6: DOS Time API Integration (0%)
  - [ ] DOS time wrapper
  - [ ] Time conversion functions
  - [ ] DOS time testing
- [ ] Day 7: Date Class Implementation (0%)
  - [ ] Date object structure
  - [ ] Date methods
  - [ ] VM integration
- [ ] Day 8: Date Testing (0%)
  - [ ] Basic tests
  - [ ] Integration tests
  - [ ] Performance tests
  - [ ] Documentation

### Overall Progress
- [ ] Phase 3.5.1 Complete (0%)
- [ ] Phase 3.5.2 Complete (0%)
- [ ] Integration Testing Complete (0%)
- [ ] Documentation Complete (0%)
- [ ] Ready for Phase 4 (0%)

## Success Criteria Checklist

### Exception Handling
- [ ] try-catch-finally構文が正しくパースされる
- [ ] 例外がthrowされ、catchでキャッチされる
- [ ] finallyブロックが常に実行される
- [ ] ネストした例外処理が動作する
- [ ] 未処理例外が適切に処理される
- [ ] メモリリークがない
- [ ] 全テストがパスする

### Date Support
- [ ] Date objectが作成できる
- [ ] 現在時刻が取得できる
- [ ] 時刻コンポーネントが取得できる
- [ ] getTime/setTimeが動作する
- [ ] toString()が正しい形式で出力する
- [ ] DOS時刻APIが正しく動作する
- [ ] メモリリークがない
- [ ] 全テストがパスする

### Integration
- [ ] 例外処理とDateが統合して動作する
- [ ] 既存機能に影響がない
- [ ] メモリ使用量が許容範囲内（+3KB以下）
- [ ] パフォーマンスが許容範囲内
- [ ] ドキュメントが完全

### Phase 4 Readiness
- [ ] 例外処理がPhase 4で使用可能
- [ ] Date機能がPhase 4で使用可能
- [ ] 全ての準備が完了

## Notes

### Important Considerations
- 例外処理は複雑なので、段階的にテストしながら実装
- DOS時刻APIの精度に注意（秒単位）
- メモリ制約を常に意識
- 既存機能への影響を最小限に

### Known Limitations
- 例外型は"Exception"のみ（IOException等は未サポート）
- Date精度はDOS時刻APIに依存（秒単位）
- タイムゾーンは未サポート
- 閏秒は未考慮

### Future Enhancements
- 複数の例外型のサポート
- より高精度な時刻取得
- タイムゾーンサポート
- Date演算の拡張

---

**Last Updated**: 2026-05-15
**Status**: Ready to begin Phase 3.5.1 Day 1
**Next Task**: Lexer updates for exception keywords