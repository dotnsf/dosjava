# Phase 3: ストリームとI/O基盤 - 実装タスク

## Week 1: 基底クラスとファイルストリーム

### Day 1-2: 基底クラス実装

#### InputStream基底クラス
- [ ] `src/runtime/inputstream.h` 作成
  - [ ] `InputStream` 構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] ドキュメントコメント追加

- [ ] `src/runtime/inputstream.c` 実装
  - [ ] `inputstream_init()` - 初期化
  - [ ] `inputstream_close()` - クローズ
  - [ ] `inputstream_read()` - 1バイト読み込み（仮想関数）
  - [ ] `inputstream_read_bytes()` - バッファ読み込み（仮想関数）
  - [ ] `inputstream_available()` - 利用可能バイト数（仮想関数）
  - [ ] `inputstream_skip()` - スキップ

- [ ] 基本テスト作成
  - [ ] `tests/io/test_inputstream.c`
  - [ ] 初期化・クローズテスト
  - [ ] 仮想関数呼び出しテスト

#### OutputStream基底クラス
- [ ] `src/runtime/outputstream.h` 作成
  - [ ] `OutputStream` 構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] ドキュメントコメント追加

- [ ] `src/runtime/outputstream.c` 実装
  - [ ] `outputstream_init()` - 初期化
  - [ ] `outputstream_close()` - クローズ
  - [ ] `outputstream_flush()` - フラッシュ
  - [ ] `outputstream_write()` - 1バイト書き込み（仮想関数）
  - [ ] `outputstream_write_bytes()` - バッファ書き込み（仮想関数）

- [ ] 基本テスト作成
  - [ ] `tests/io/test_outputstream.c`
  - [ ] 初期化・クローズテスト
  - [ ] 仮想関数呼び出しテスト

### Day 3-4: FileInputStream実装

#### FileInputStream実装
- [ ] `src/runtime/fileinputstream.h` 作成
  - [ ] `FileInputStream` 構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] エラーコード定義

- [ ] `src/runtime/fileinputstream.c` 実装
  - [ ] `fileinputstream_new()` - ファイルオープン
    - [ ] DOS INT 21h AH=3Dh（ファイルオープン）
    - [ ] エラーハンドリング
    - [ ] ファイルサイズ取得
  - [ ] `fileinputstream_delete()` - リソース解放
  - [ ] `fileinputstream_read()` - 1バイト読み込み
    - [ ] DOS INT 21h AH=3Fh（ファイル読み込み）
    - [ ] EOF処理
  - [ ] `fileinputstream_read_bytes()` - バッファ読み込み
  - [ ] `fileinputstream_available()` - 残りバイト数計算
  - [ ] `fileinputstream_close()` - ファイルクローズ
    - [ ] DOS INT 21h AH=3Eh（ファイルクローズ）

#### テストケース作成
- [ ] `tests/io/test_fileinput.c` - C言語テスト
  - [ ] ファイルオープンテスト
  - [ ] 1バイト読み込みテスト
  - [ ] バッファ読み込みテスト
  - [ ] EOF処理テスト
  - [ ] エラーハンドリングテスト

- [ ] `tests/io/fileread.jav` - Javaテスト
  ```java
  class fileread {
      public static void main() {
          FileInputStream fis = new FileInputStream("TEST.TXT");
          int b = fis.read();
          while (b != -1) {
              System.out.print((char)b);
              b = fis.read();
          }
          fis.close();
      }
  }
  ```

### Day 5-7: FileOutputStream実装

#### FileOutputStream実装
- [ ] `src/runtime/fileoutputstream.h` 作成
  - [ ] `FileOutputStream` 構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] モード定義（新規/追記）

- [ ] `src/runtime/fileoutputstream.c` 実装
  - [ ] `fileoutputstream_new()` - ファイル作成
    - [ ] DOS INT 21h AH=3Ch（ファイル作成）
    - [ ] エラーハンドリング
  - [ ] `fileoutputstream_new_append()` - 追記モードオープン
    - [ ] DOS INT 21h AH=3Dh（ファイルオープン）
    - [ ] DOS INT 21h AH=42h（ファイル末尾へシーク）
  - [ ] `fileoutputstream_delete()` - リソース解放
  - [ ] `fileoutputstream_write()` - 1バイト書き込み
    - [ ] DOS INT 21h AH=40h（ファイル書き込み）
  - [ ] `fileoutputstream_write_bytes()` - バッファ書き込み
  - [ ] `fileoutputstream_flush()` - フラッシュ（DOS自動フラッシュ）
  - [ ] `fileoutputstream_close()` - ファイルクローズ

#### テストケース作成
- [ ] `tests/io/test_fileoutput.c` - C言語テスト
  - [ ] ファイル作成テスト
  - [ ] 1バイト書き込みテスト
  - [ ] バッファ書き込みテスト
  - [ ] 追記モードテスト
  - [ ] エラーハンドリングテスト

- [ ] `tests/io/filewrite.jav` - Javaテスト（新規作成）
  ```java
  class filewrite {
      public static void main() {
          FileOutputStream fos = new FileOutputStream("OUTPUT.TXT");
          fos.write("Hello File");
          fos.close();
      }
  }
  ```

- [ ] `tests/io/fileappend.jav` - Javaテスト（追記）
  ```java
  class fileappend {
      public static void main() {
          FileOutputStream fos = new FileOutputStream("OUTPUT.TXT", true);
          fos.write("Appended");
          fos.close();
      }
  }
  ```

## Week 2: バッファリングとVM統合

### Day 8-10: BufferedReader/Writer実装

#### BufferedReader実装
- [ ] `src/runtime/bufferedreader.h` 作成
  - [ ] `BufferedReader` 構造体定義
  - [ ] バッファサイズ定義（512バイト）
  - [ ] 関数プロトタイプ宣言

- [ ] `src/runtime/bufferedreader.c` 実装
  - [ ] `bufferedreader_new()` - 初期化
  - [ ] `bufferedreader_delete()` - リソース解放
  - [ ] `bufferedreader_read()` - 1文字読み込み
    - [ ] バッファ管理
    - [ ] 自動リフィル
  - [ ] `bufferedreader_readline()` - 1行読み込み
    - [ ] 改行検出（CR+LF対応）
    - [ ] 動的メモリ確保
  - [ ] `bufferedreader_ready()` - 読み込み可能チェック
  - [ ] `bufferedreader_close()` - クローズ

#### BufferedWriter実装
- [ ] `src/runtime/bufferedwriter.h` 作成
  - [ ] `BufferedWriter` 構造体定義
  - [ ] バッファサイズ定義（512バイト）
  - [ ] 関数プロトタイプ宣言

- [ ] `src/runtime/bufferedwriter.c` 実装
  - [ ] `bufferedwriter_new()` - 初期化
  - [ ] `bufferedwriter_delete()` - リソース解放
  - [ ] `bufferedwriter_write()` - 1文字書き込み
    - [ ] バッファ管理
    - [ ] 自動フラッシュ
  - [ ] `bufferedwriter_write_string()` - 文字列書き込み
  - [ ] `bufferedwriter_newline()` - 改行書き込み（CR+LF）
  - [ ] `bufferedwriter_flush()` - 明示的フラッシュ
  - [ ] `bufferedwriter_close()` - クローズ

#### テストケース作成
- [ ] `tests/io/test_buffered.c` - C言語テスト
  - [ ] BufferedReaderテスト
  - [ ] BufferedWriterテスト
  - [ ] バッファリング効果確認

- [ ] `tests/io/bufread.jav` - Javaテスト
  ```java
  class bufread {
      public static void main() {
          FileInputStream fis = new FileInputStream("INPUT.TXT");
          BufferedReader br = new BufferedReader(fis);
          String line = br.readLine();
          while (line != null) {
              System.out.println(line);
              line = br.readLine();
          }
          br.close();
      }
  }
  ```

- [ ] `tests/io/bufwrite.jav` - Javaテスト
  ```java
  class bufwrite {
      public static void main() {
          FileOutputStream fos = new FileOutputStream("OUTPUT.TXT");
          BufferedWriter bw = new BufferedWriter(fos);
          bw.write("Line 1");
          bw.newLine();
          bw.write("Line 2");
          bw.close();
      }
  }
  ```

### Day 11-12: VM拡張

#### オペコード追加
- [ ] `src/format/opcodes.h` 更新
  - [ ] `OP_FILE_OPEN_READ` (0x90) 定義
  - [ ] `OP_FILE_OPEN_WRITE` (0x91) 定義
  - [ ] `OP_FILE_READ` (0x92) 定義
  - [ ] `OP_FILE_WRITE` (0x93) 定義
  - [ ] `OP_FILE_CLOSE` (0x94) 定義
  - [ ] `OP_FILE_FLUSH` (0x95) 定義

- [ ] `src/format/opcodes.c` 更新
  - [ ] オペコード名追加
  - [ ] オペコード長定義

#### インタープリタ拡張
- [ ] `src/vm/interpreter.c` 更新
  - [ ] `OP_FILE_OPEN_READ` 実装
    - [ ] スタックからファイル名取得
    - [ ] `FileInputStream` 作成
    - [ ] エラーハンドリング
  - [ ] `OP_FILE_OPEN_WRITE` 実装
    - [ ] スタックからファイル名取得
    - [ ] `FileOutputStream` 作成
    - [ ] エラーハンドリング
  - [ ] `OP_FILE_READ` 実装
    - [ ] ストリームから読み込み
    - [ ] 結果をスタックにプッシュ
  - [ ] `OP_FILE_WRITE` 実装
    - [ ] スタックからデータ取得
    - [ ] ストリームへ書き込み
  - [ ] `OP_FILE_CLOSE` 実装
    - [ ] ストリームクローズ
    - [ ] リソース解放
  - [ ] `OP_FILE_FLUSH` 実装
    - [ ] バッファフラッシュ

#### コンパイラ拡張
- [ ] `tools/compiler/codegen.c` 更新
  - [ ] `FileInputStream` コンストラクタ認識
  - [ ] `FileOutputStream` コンストラクタ認識
  - [ ] `BufferedReader` コンストラクタ認識
  - [ ] `BufferedWriter` コンストラクタ認識
  - [ ] `read()` メソッド認識
  - [ ] `write()` メソッド認識
  - [ ] `readLine()` メソッド認識
  - [ ] `newLine()` メソッド認識
  - [ ] `close()` メソッド認識
  - [ ] `flush()` メソッド認識

- [ ] `tools/compiler/semantic.c` 更新
  - [ ] I/Oクラスの型チェック
  - [ ] メソッドシグネチャ検証

### Day 13-14: 統合テストとデバッグ

#### エンドツーエンドテスト
- [ ] `tests/io/e2e_write.jav` - 書き込みテスト
  ```java
  class e2e_write {
      public static void main() {
          FileOutputStream fos = new FileOutputStream("E2E.TXT");
          BufferedWriter bw = new BufferedWriter(fos);
          bw.write("Test Line 1");
          bw.newLine();
          bw.write("Test Line 2");
          bw.newLine();
          bw.close();
          System.out.println("Write complete");
      }
  }
  ```

- [ ] `tests/io/e2e_read.jav` - 読み込みテスト
  ```java
  class e2e_read {
      public static void main() {
          FileInputStream fis = new FileInputStream("E2E.TXT");
          BufferedReader br = new BufferedReader(fis);
          String line = br.readLine();
          int count = 0;
          while (line != null) {
              System.out.println(line);
              count = count + 1;
              line = br.readLine();
          }
          br.close();
          System.out.print("Read ");
          System.out.print(count);
          System.out.println(" lines");
      }
  }
  ```

- [ ] `tests/io/e2e_copy.jav` - ファイルコピーテスト
  ```java
  class e2e_copy {
      public static void main() {
          FileInputStream fis = new FileInputStream("SOURCE.TXT");
          FileOutputStream fos = new FileOutputStream("DEST.TXT");
          int b = fis.read();
          while (b != -1) {
              fos.write(b);
              b = fis.read();
          }
          fis.close();
          fos.close();
          System.out.println("Copy complete");
      }
  }
  ```

#### パフォーマンステスト
- [ ] バッファリングあり/なしの比較
- [ ] 大きなファイル（10KB+）の処理
- [ ] 複数ファイル同時オープン

#### メモリリークテスト
- [ ] ファイルオープン/クローズ繰り返し
- [ ] 例外発生時のリソース解放確認
- [ ] 長時間実行テスト

#### バグ修正
- [ ] 発見されたバグの修正
- [ ] エラーメッセージの改善
- [ ] ドキュメント更新

## ビルドシステム統合

### Makefile更新
- [ ] ランタイムライブラリのビルドルール追加
  - [ ] `inputstream.obj`
  - [ ] `outputstream.obj`
  - [ ] `fileinputstream.obj`
  - [ ] `fileoutputstream.obj`
  - [ ] `bufferedreader.obj`
  - [ ] `bufferedwriter.obj`

- [ ] `djvm.exe` の依存関係更新
  - [ ] 新しいランタイムオブジェクトを追加

- [ ] テストプログラムのビルドルール追加
  - [ ] `test_io.exe`

### テストスクリプト作成
- [ ] `tests/io/run_tests.bat` - DOSBox用テストスクリプト
  ```batch
  @echo off
  echo Running I/O tests...
  
  echo Test 1: File write
  djc filewrite.jav
  djvm filewrite.djc
  
  echo Test 2: File read
  djc fileread.jav
  djvm fileread.jav
  
  echo Test 3: Buffered write
  djc bufwrite.jav
  djvm bufwrite.djc
  
  echo Test 4: Buffered read
  djc bufread.jav
  djvm bufread.djc
  
  echo All tests complete!
  ```

## ドキュメント作成

### API ドキュメント
- [ ] `docs/API_IO.md` 作成
  - [ ] InputStream API
  - [ ] OutputStream API
  - [ ] FileInputStream API
  - [ ] FileOutputStream API
  - [ ] BufferedReader API
  - [ ] BufferedWriter API
  - [ ] 使用例

### クイックスタートガイド
- [ ] `docs/IO_QUICKSTART.md` 作成
  - [ ] 基本的なファイル読み書き
  - [ ] バッファリングの使い方
  - [ ] エラーハンドリング
  - [ ] ベストプラクティス

### トラブルシューティング
- [ ] `docs/IO_TROUBLESHOOTING.md` 作成
  - [ ] よくあるエラーと対処法
  - [ ] ファイルハンドルリーク
  - [ ] パフォーマンス問題

## 完了チェックリスト

### 機能完成度
- [ ] すべての基底クラスが実装されている
- [ ] FileInputStream/OutputStreamが動作する
- [ ] BufferedReader/Writerが動作する
- [ ] VM拡張が完了している
- [ ] コンパイラ拡張が完了している

### テスト
- [ ] すべてのユニットテストがパスする
- [ ] すべての統合テストがパスする
- [ ] エンドツーエンドテストがパスする
- [ ] パフォーマンステストが完了している
- [ ] メモリリークテストがパスする

### 品質
- [ ] メモリリークがない
- [ ] ファイルハンドルが正しく管理されている
- [ ] エラーハンドリングが適切
- [ ] コードレビュー完了
- [ ] ドキュメントが完備されている

### ビルド
- [ ] Makefileが更新されている
- [ ] クリーンビルドが成功する
- [ ] すべてのターゲットがビルドできる

### リリース準備
- [ ] README.md更新
- [ ] CHANGELOG.md更新
- [ ] バージョン番号更新
- [ ] リリースノート作成

---

**作成日**: 2026-05-11  
**最終更新**: 2026-05-11