# Phase 4: Network Socket Implementation - Task Checklist (mTCP版)

## Phase 4.1: mTCP Integration and C-level Socket API (Week 1-2)

### Week 1: mTCP Setup and Basic Socket API

#### Day 1-2: mTCP Integration and Configuration
- [ ] mTCPライブラリとヘッダーの確認
  - [ ] C:\mTCP\src\TCPLIB\ のソースコード確認
  - [ ] C:\mTCP\src\TCPINC\ のヘッダーファイル確認
  - [ ] MTCPWS.LIB (Small model) の場所確認
  - [ ] doscurlプロジェクトの参照

- [ ] dosjava用設定ファイルの作成
  - [ ] `dosjava/dosjava.cfg` を作成
  - [ ] doscurl/cpp/doscurl.cfgを参考に設定
  - [ ] MTCP_PROGRAM_NAME = "dosjava"
  - [ ] UTILS_CHECK_HEAP = 0 (ヒープチェック無効化)
  - [ ] TCP_MAX_SOCKETS = 4
  - [ ] PACKET_BUFFERS = 10

- [ ] Makefileの更新
  - [ ] インクルードパス追加: `-IC:\mTCP\src\TCPINC`
  - [ ] ライブラリリンク追加: `C:\mTCP\src\TCPLIB\MTCPWS.LIB`
  - [ ] コンパイルフラグ追加: `-DCFG_H="dosjava.cfg"`
  - [ ] Small modelの確認: `-ms`

- [ ] 初期化テストプログラムの作成
  - [ ] `tests/network/test_mtcp_init.c` を作成
  - [ ] sock_init()の呼び出し
  - [ ] _watt_do_exitフラグのチェック
  - [ ] パケットドライバ検出確認
  - [ ] エラーメッセージの表示

- [ ] Makefileにテストターゲット追加
  - [ ] test_mtcp ターゲットを追加
  - [ ] tmtcp.exe のビルド設定
  - [ ] mTCPライブラリのリンク

- [ ] DOSBox-Xでの動作確認
  - [ ] NE2000エミュレーション設定確認
  - [ ] パケットドライバロード (ne2000 0x60 3 0x300)
  - [ ] MTCP.CFG設定ファイル作成
  - [ ] MTCPCFG環境変数設定
  - [ ] tmtcp.exe実行とsock_init()成功確認

**Deliverables:**
- dosjava.cfg (mTCP設定)
- 更新されたMakefile
- test_mtcp_init.c
- tmtcp.exe (動作確認済み)
- DOSBox-X動作確認レポート

#### Day 3-5: C-level Socket Wrapper Functions
- [ ] doscurlのsocket.c分析
  - [ ] `doscurl/src/socket.c` の実装確認
  - [ ] `doscurl/include/socket.h` のAPI確認
  - [ ] mTCP APIの使用方法理解
  - [ ] エラーハンドリング方法の確認

- [ ] dosjava用socket wrapper設計
  - [ ] APIインターフェース設計
  - [ ] エラーコード定義
  - [ ] データ構造設計
  - [ ] メモリ管理戦略

- [ ] `src/runtime/socket.h` の作成
  - [ ] socket_t型定義
  - [ ] 関数プロトタイプ宣言
  - [ ] エラーコード定義
  - [ ] ドキュメントコメント

- [ ] `src/runtime/socket.c` の実装
  - [ ] 必要なヘッダーインクルード
  - [ ] グローバル変数定義
  - [ ] 各関数の実装

- [ ] socket_init() 実装
  - [ ] sock_init()呼び出し
  - [ ] _watt_do_exitチェック
  - [ ] エラーハンドリング
  - [ ] デバッグログ出力

- [ ] socket_cleanup() 実装
  - [ ] 全ソケットのクローズ
  - [ ] リソース解放
  - [ ] デバッグログ出力

- [ ] socket_resolve() 実装
  - [ ] resolve()呼び出し
  - [ ] IPアドレス文字列変換
  - [ ] エラーハンドリング
  - [ ] タイムアウト処理

- [ ] socket_connect() 実装
  - [ ] DNS解決 (resolve)
  - [ ] tcp_Socket構造体割り当て
  - [ ] _w32_tcp_open()呼び出し
  - [ ] 接続確立待ち (sock_established)
  - [ ] tcp_tick()によるポーリング
  - [ ] タイムアウト処理
  - [ ] エラーハンドリング

- [ ] socket_send() 実装
  - [ ] sock_write()呼び出し
  - [ ] _w32_sock_flush()呼び出し
  - [ ] 送信バイト数の返却
  - [ ] エラーハンドリング

- [ ] socket_recv() 実装
  - [ ] sock_dataready()でデータ確認
  - [ ] tcp_tick()によるポーリング
  - [ ] sock_read()呼び出し
  - [ ] タイムアウト処理
  - [ ] 接続クローズ検出
  - [ ] エラーハンドリング

- [ ] socket_close() 実装
  - [ ] sock_close()呼び出し
  - [ ] メモリ解放
  - [ ] デバッグログ出力

- [ ] エラーハンドリングの実装
  - [ ] エラーメッセージ管理
  - [ ] socket_get_error()関数
  - [ ] エラーコードの定義

- [ ] デバッグログの実装
  - [ ] 条件付きコンパイル (#ifdef DEBUG)
  - [ ] 詳細なログ出力
  - [ ] エラー時の詳細情報

- [ ] Makefileの更新
  - [ ] socket.cのコンパイル設定
  - [ ] mTCPヘッダーパス
  - [ ] mTCPライブラリリンク

**Deliverables:**
- src/runtime/socket.h (完全なAPI定義)
- src/runtime/socket.c (全関数実装済み)
- 更新されたMakefile
- doscurl参考実装ノート

#### Day 6-7: C-level Socket Testing
- [ ] テストディレクトリの確認
  - [ ] `tests/network/` ディレクトリ存在確認
  - [ ] 必要に応じて作成

- [ ] `tests/network/test_socket.c` の作成
  - [ ] テストフレームワーク構造
  - [ ] 各テストケースの実装
  - [ ] 結果レポート機能

- [ ] Test 1: Socket initialization
  - [ ] socket_init()呼び出し
  - [ ] 成功確認
  - [ ] エラーケーステスト

- [ ] Test 2: DNS resolution
  - [ ] socket_resolve()テスト
  - [ ] 既知のホスト名で確認
  - [ ] 不正なホスト名でエラー確認

- [ ] Test 3: TCP connection
  - [ ] socket_connect()テスト
  - [ ] HTTPサーバーへの接続
  - [ ] 接続成功確認
  - [ ] 不正なホストでエラー確認

- [ ] Test 4: Data send
  - [ ] socket_send()テスト
  - [ ] HTTP GETリクエスト送信
  - [ ] 送信バイト数確認

- [ ] Test 5: Data receive
  - [ ] socket_recv()テスト
  - [ ] HTTPレスポンス受信
  - [ ] 受信データ確認

- [ ] Test 6: Socket close
  - [ ] socket_close()テスト
  - [ ] リソース解放確認
  - [ ] 再接続可能確認

- [ ] Test 7: Error handling
  - [ ] 接続失敗テスト
  - [ ] タイムアウトテスト
  - [ ] エラーメッセージ確認

- [ ] Test 8: Multiple connections
  - [ ] 複数ソケット同時使用
  - [ ] メモリ使用量確認
  - [ ] リソースリーク確認

- [ ] Makefileにテストターゲット追加
  - [ ] test_socket ターゲット
  - [ ] tsock.exe ビルド設定
  - [ ] 依存関係設定

- [ ] DOSBox-Xでのテスト実行
  - [ ] 全テストケース実行
  - [ ] 結果の記録
  - [ ] 問題点の洗い出し

- [ ] テスト結果のドキュメント化
  - [ ] `tests/network/TEST_RESULTS.md` 作成
  - [ ] 各テストの結果記録
  - [ ] パフォーマンス測定結果
  - [ ] 問題点と対策

**Deliverables:**
- tests/network/test_socket.c (8テスト実装)
- build/bin/tsock.exe (動作確認済み)
- tests/network/TEST_RESULTS.md
- 問題点リスト

### Week 2: Advanced Socket Features and Integration

#### Day 8-10: Non-blocking I/O and Timeouts
- [ ] タイムアウト機能の設計
  - [ ] タイムアウト値の管理方法
  - [ ] タイムアウト検出方法
  - [ ] デフォルト値の決定

- [ ] socket_set_timeout() 実装
  - [ ] 関数プロトタイプ追加
  - [ ] タイムアウト値の保存
  - [ ] 各関数での適用

- [ ] 接続タイムアウトの実装
  - [ ] socket_connect()でのタイムアウト
  - [ ] sock_delay変数の活用
  - [ ] タイムアウト時のエラー処理

- [ ] 送受信タイムアウトの実装
  - [ ] socket_send()でのタイムアウト
  - [ ] socket_recv()でのタイムアウト
  - [ ] tcp_tick()ベースの実装

- [ ] 非ブロッキングモードの設計
  - [ ] ポーリングベースの実装
  - [ ] 状態管理方法
  - [ ] API設計

- [ ] socket_set_nonblocking() 実装
  - [ ] 関数プロトタイプ追加
  - [ ] 非ブロッキングフラグ管理
  - [ ] 各関数での動作変更

- [ ] socket_available() 実装
  - [ ] sock_dataready()の活用
  - [ ] 利用可能バイト数の返却
  - [ ] エラーハンドリング

- [ ] テストプログラムの作成
  - [ ] タイムアウト動作テスト
  - [ ] 非ブロッキング動作テスト
  - [ ] socket_available()テスト

- [ ] DOSBox-Xでのテスト
  - [ ] 各機能の動作確認
  - [ ] パフォーマンス測定
  - [ ] 問題点の洗い出し

- [ ] ドキュメント更新
  - [ ] socket.hのコメント更新
  - [ ] 使用例の追加
  - [ ] 注意事項の記載

**Deliverables:**
- 拡張されたsocket.h/socket.c
- タイムアウト/非ブロッキングテスト
- 更新されたドキュメント

#### Day 11-12: Memory Management and Optimization
- [ ] 現在のメモリ使用量測定
  - [ ] VMのメモリ使用量
  - [ ] mTCPのメモリ使用量
  - [ ] ソケットバッファのメモリ使用量
  - [ ] 合計メモリ使用量

- [ ] dosjava.cfgの最適化
  - [ ] TCP_MAX_SOCKETSの調整
  - [ ] PACKET_BUFFERSの調整
  - [ ] その他バッファサイズの調整
  - [ ] メモリ使用量の再測定

- [ ] バッファサイズの最適化
  - [ ] 送信バッファサイズ
  - [ ] 受信バッファサイズ
  - [ ] Small modelの制約考慮
  - [ ] パフォーマンスとのバランス

- [ ] ソケットプールの設計
  - [ ] ソケット再利用機構
  - [ ] 最大同時接続数制限
  - [ ] プール管理構造

- [ ] ソケットプールの実装
  - [ ] socket_pool構造体
  - [ ] socket_pool_init()
  - [ ] socket_pool_get()
  - [ ] socket_pool_release()

- [ ] メモリリーク検出
  - [ ] 全ソケットのクローズ確認
  - [ ] メモリ解放の確認
  - [ ] 長時間実行テスト

- [ ] プロファイリング
  - [ ] メモリ使用量の詳細測定
  - [ ] パフォーマンスボトルネック特定
  - [ ] 最適化の効果測定

- [ ] 最適化の実施
  - [ ] 特定されたボトルネックの改善
  - [ ] 不要なメモリコピーの削減
  - [ ] バッファリング戦略の改善

- [ ] 最適化後のテスト
  - [ ] 全テストの再実行
  - [ ] メモリ使用量の確認
  - [ ] パフォーマンスの確認

- [ ] ドキュメント作成
  - [ ] メモリ最適化レポート
  - [ ] 推奨設定値
  - [ ] トレードオフの説明

**Deliverables:**
- 最適化されたdosjava.cfg
- ソケットプール実装
- メモリプロファイリング結果
- 最適化レポート

#### Day 13-14: Documentation and Cleanup
- [ ] PHASE4_SOCKET_API.md の作成
  - [ ] API概要
  - [ ] 各関数の詳細説明
  - [ ] 引数と戻り値
  - [ ] エラーコード一覧
  - [ ] 使用例

- [ ] PHASE4_MTCP_NOTES.md の作成
  - [ ] mTCP統合の詳細
  - [ ] doscurlとの違い
  - [ ] 実装上の注意点
  - [ ] トラブルシューティング
  - [ ] 既知の問題

- [ ] サンプルコードの作成
  - [ ] 簡単なHTTP GETクライアント
  - [ ] エコーサーバー
  - [ ] エコークライアント
  - [ ] コメント付き説明

- [ ] コードクリーンアップ
  - [ ] デバッグprintfの削除/条件化
  - [ ] 未使用コードの削除
  - [ ] コメントの整理
  - [ ] インデントの統一

- [ ] コーディング規約チェック
  - [ ] .clinerules準拠確認
  - [ ] 命名規則確認
  - [ ] ファイル構造確認

- [ ] Phase 4.2の準備
  - [ ] Java統合の設計レビュー
  - [ ] 必要な変更の洗い出し
  - [ ] タスクリストの作成

- [ ] 最終テスト
  - [ ] 全テストの実行
  - [ ] ドキュメントの確認
  - [ ] ビルドの確認

**Deliverables:**
- PHASE4_SOCKET_API.md (完全版)
- PHASE4_MTCP_NOTES.md (実装ノート)
- サンプルコード集
- クリーンアップされたソースコード
- Phase 4.2準備完了

## Phase 4.2: Java Socket Classes and VM Integration (Week 3-4)

### Week 3: Java Socket Classes

#### Day 15-17: Socket and ServerSocket Classes
- [ ] Java socket class設計
  - [ ] Socket class構造
  - [ ] ServerSocket class構造
  - [ ] InputStream/OutputStream統合
  - [ ] オブジェクトライフサイクル

- [ ] VM内部構造の設計
  - [ ] Socket objectの表現方法
  - [ ] Native handleの管理
  - [ ] GCとの統合
  - [ ] メモリ管理

- [ ] `src/runtime/socket_java.h` の作成
  - [ ] Socket object構造体定義
  - [ ] ServerSocket object構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] マクロ定義

- [ ] `src/runtime/socket_java.c` の作成
  - [ ] Socket object作成関数
  - [ ] ServerSocket object作成関数
  - [ ] メソッド実装関数
  - [ ] ヘルパー関数

- [ ] Socket class実装
  - [ ] Socket(String host, int port) コンストラクタ
  - [ ] getInputStream() メソッド
  - [ ] getOutputStream() メソッド
  - [ ] close() メソッド

- [ ] ServerSocket class実装
  - [ ] ServerSocket(int port) コンストラクタ
  - [ ] accept() メソッド
  - [ ] close() メソッド

- [ ] SocketInputStream実装
  - [ ] read() メソッド
  - [ ] available() メソッド
  - [ ] InputStreamとの統合

- [ ] SocketOutputStream実装
  - [ ] write(int b) メソッド
  - [ ] flush() メソッド
  - [ ] OutputStreamとの統合

- [ ] エラーハンドリング
  - [ ] Java例外への変換
  - [ ] エラーメッセージ
  - [ ] スタックトレース

- [ ] 基本テスト
  - [ ] Socket作成テスト
  - [ ] InputStream/OutputStreamテスト
  - [ ] メモリ管理テスト

**Deliverables:**
- src/runtime/socket_java.h
- src/runtime/socket_java.c
- 基本テスト結果

#### Day 18-19: Native Method Mechanism
- [ ] OP_INVOKE_NATIVE設計
  - [ ] オペコード定義
  - [ ] 引数の渡し方
  - [ ] 戻り値の処理
  - [ ] エラーハンドリング

- [ ] `src/format/opcodes.h` 更新
  - [ ] OP_INVOKE_NATIVE定義追加
  - [ ] オペコード番号割り当て
  - [ ] ドキュメントコメント

- [ ] Native method registry設計
  - [ ] 登録データ構造
  - [ ] 検索方法
  - [ ] 初期化方法

- [ ] `src/vm/native.h` の作成
  - [ ] NativeMethod構造体定義
  - [ ] 関数プロトタイプ宣言
  - [ ] マクロ定義

- [ ] `src/vm/native.c` の作成
  - [ ] Native method registry実装
  - [ ] native_register()関数
  - [ ] native_lookup()関数
  - [ ] native_init()関数

- [ ] Socket native methods登録
  - [ ] Socket.connect() -> socket_connect()
  - [ ] Socket.send() -> socket_send()
  - [ ] Socket.recv() -> socket_recv()
  - [ ] Socket.close() -> socket_close()
  - [ ] ServerSocket.bind() -> socket_bind()
  - [ ] ServerSocket.listen() -> socket_listen()
  - [ ] ServerSocket.accept() -> socket_accept()

- [ ] `src/vm/interpreter.c` 更新
  - [ ] OP_INVOKE_NATIVE処理追加
  - [ ] Method name/signature取得
  - [ ] Native function lookup
  - [ ] 引数の準備
  - [ ] Native function呼び出し
  - [ ] 戻り値の処理
  - [ ] エラーハンドリング

- [ ] コンパイラ更新
  - [ ] Native method認識
  - [ ] OP_INVOKE_NATIVE生成
  - [ ] メソッドシグネチャ処理

- [ ] テスト
  - [ ] Native method呼び出しテスト
  - [ ] 引数渡しテスト
  - [ ] 戻り値テスト
  - [ ] エラーハンドリングテスト

**Deliverables:**
- 更新されたopcodes.h
- native.h/native.c
- 更新されたinterpreter.c
- Native method呼び出しテスト

#### Day 20-21: VM Socket Integration
- [ ] OP_NEWでのSocket object作成
  - [ ] Socket classの特別処理
  - [ ] ServerSocket classの特別処理
  - [ ] Native handleの割り当て
  - [ ] 初期化処理

- [ ] Socket method callsの実装
  - [ ] Method dispatch確認
  - [ ] 引数の型変換
  - [ ] 戻り値の型変換
  - [ ] エラー処理

- [ ] Socket cleanupの実装
  - [ ] VM shutdown時の処理
  - [ ] 全ソケットのクローズ
  - [ ] リソース解放
  - [ ] メモリリーク防止

- [ ] GC統合
  - [ ] Socket objectのマーク
  - [ ] 未使用Socketの検出
  - [ ] 自動クローズ処理

- [ ] Lifecycle testing
  - [ ] Socket作成から破棄まで
  - [ ] 複数Socketの管理
  - [ ] メモリリークチェック
  - [ ] リソースリークチェック

- [ ] エラーハンドリング統合
  - [ ] C-levelエラーからJava例外へ
  - [ ] エラーメッセージの伝播
  - [ ] スタックトレース

- [ ] デバッグ機能
  - [ ] Socket状態の表示
  - [ ] デバッグログ
  - [ ] トレース機能

**Deliverables:**
- 更新されたVM (interpreter.c等)
- Socket lifecycle管理
- GC統合
- デバッグ機能

### Week 4: Testing and Finalization

#### Day 22-24: Java Socket Testing
- [ ] Echo serverテストの作成
  - [ ] `tests/network/echoserv.jav` 作成
  - [ ] ServerSocketでポート待ち受け
  - [ ] accept()で接続受け入れ
  - [ ] 受信データをそのまま返送
  - [ ] エラーハンドリング

- [ ] Echo clientテストの作成
  - [ ] `tests/network/echocli.jav` 作成
  - [ ] Socketで接続
  - [ ] データ送信
  - [ ] データ受信
  - [ ] 結果確認

- [ ] HTTP GETクライアントの作成
  - [ ] `tests/network/httpget.jav` 作成
  - [ ] HTTPサーバーへの接続
  - [ ] HTTP GETリクエスト送信
  - [ ] レスポンス受信
  - [ ] レスポンス表示

- [ ] エラーハンドリングテスト
  - [ ] 接続失敗テスト
  - [ ] タイムアウトテスト
  - [ ] 不正なホスト名テスト
  - [ ] ポート使用中テスト

- [ ] コンパイルとテスト
  - [ ] 全.javファイルのコンパイル
  - [ ] djc.exeでのコンパイル確認
  - [ ] .djcファイル生成確認

- [ ] DOSBox-Xでのテスト
  - [ ] Echo server起動
  - [ ] Echo client実行
  - [ ] HTTP GET実行
  - [ ] 結果確認

- [ ] テスト結果のドキュメント化
  - [ ] `tests/network/JAVA_TEST_RESULTS.md` 作成
  - [ ] 各テストの結果
  - [ ] スクリーンショット
  - [ ] 問題点

**Deliverables:**
- tests/network/echoserv.jav
- tests/network/echocli.jav
- tests/network/httpget.jav
- tests/network/JAVA_TEST_RESULTS.md

#### Day 25-26: Integration Testing
- [ ] 外部サーバーとのテスト
  - [ ] 実際のHTTPサーバーへの接続
  - [ ] レスポンスの正確性確認
  - [ ] 各種HTTPステータスコード
  - [ ] 大きなレスポンスの処理

- [ ] 複数同時接続のテスト
  - [ ] 複数Socketの同時使用
  - [ ] メモリ使用量の確認
  - [ ] パフォーマンスの測定
  - [ ] リソース管理の確認

- [ ] 大容量データ転送のテスト
  - [ ] 大きなファイルのダウンロード
  - [ ] バッファリングの確認
  - [ ] メモリ使用量の監視
  - [ ] タイムアウトの確認

- [ ] 長時間実行テスト
  - [ ] 連続接続テスト
  - [ ] メモリリークチェック
  - [ ] 安定性確認

- [ ] パフォーマンス測定
  - [ ] スループット測定
  - [ ] レイテンシ測定
  - [ ] メモリ使用量測定
  - [ ] CPU使用率測定

- [ ] ストレステスト
  - [ ] 最大同時接続数
  - [ ] 高負荷時の動作
  - [ ] エラー回復

- [ ] 統合テスト結果のドキュメント化
  - [ ] `tests/network/INTEGRATION_TEST_RESULTS.md` 作成
  - [ ] パフォーマンス測定結果
  - [ ] 問題点と対策
  - [ ] 推奨設定

**Deliverables:**
- 統合テスト結果
- パフォーマンスレポート
- 問題点リスト
- 推奨設定ドキュメント

#### Day 27-28: Documentation and Cleanup
- [ ] PHASE4_NETWORK_QUICKSTART.md の作成
  - [ ] クイックスタートガイド
  - [ ] 環境設定手順
  - [ ] 基本的な使い方
  - [ ] サンプルプログラム
  - [ ] トラブルシューティング

- [ ] 制限事項と既知の問題のドキュメント化
  - [ ] DOSの制約
  - [ ] mTCPの制限
  - [ ] 既知のバグ
  - [ ] 回避策

- [ ] サンプルプログラムの作成
  - [ ] 簡単なHTTPクライアント
  - [ ] 簡単なTCPサーバー
  - [ ] チャットプログラム
  - [ ] コメント付き説明

- [ ] APIリファレンスの作成
  - [ ] Java Socket API
  - [ ] Java ServerSocket API
  - [ ] InputStream/OutputStream
  - [ ] 使用例

- [ ] 最終的なコードレビュー
  - [ ] 全ソースコードのレビュー
  - [ ] コーディング規約確認
  - [ ] コメントの確認
  - [ ] ドキュメントの確認

- [ ] 最終的なクリーンアップ
  - [ ] 不要なファイルの削除
  - [ ] デバッグコードの削除
  - [ ] テストコードの整理
  - [ ] ドキュメントの整理

- [ ] 最終テスト
  - [ ] 全テストの実行
  - [ ] ビルドの確認
  - [ ] ドキュメントの確認
  - [ ] リリース準備

- [ ] Phase 4完了レポート
  - [ ] 実装内容のサマリー
  - [ ] 達成した目標
  - [ ] 残された課題
  - [ ] 今後の展望

**Deliverables:**
- PHASE4_NETWORK_QUICKSTART.md
- サンプルプログラム集
- APIリファレンス
- Phase 4完了レポート
- リリース準備完了

## Progress Tracking

### Phase 4.1 Progress
- [ ] Day 1-2: mTCP Integration (0%)
- [ ] Day 3-5: Socket Wrapper Functions (0%)
- [ ] Day 6-7: Socket Testing (0%)
- [ ] Day 8-10: Non-blocking I/O (0%)
- [ ] Day 11-12: Memory Optimization (0%)
- [ ] Day 13-14: Documentation (0%)

### Phase 4.2 Progress
- [ ] Day 15-17: Java Socket Classes (0%)
- [ ] Day 18-19: Native Method Mechanism (0%)
- [ ] Day 20-21: VM Integration (0%)
- [ ] Day 22-24: Java Testing (0%)
- [ ] Day 25-26: Integration Testing (0%)
- [ ] Day 27-28: Final Documentation (0%)

## Notes

### Important References
- doscurl/src/socket.c - Socket wrapper reference implementation
- doscurl/include/socket.h - Socket API reference
- doscurl/cpp/doscurl.cfg - mTCP configuration reference
- C:\mTCP\src\developers.pdf - mTCP documentation

### Key Differences from Wattcp Plan
1. Using mTCP library instead of Wattcp
2. Reference implementation available (doscurl)
3. Proven to work in DOSBox-X environment
4. Different configuration approach (dosjava.cfg)
5. Small model library (MTCPWS.LIB)

### Success Criteria
- [ ] All C-level tests pass
- [ ] All Java-level tests pass
- [ ] Memory usage < 200KB total
- [ ] Can connect to external servers
- [ ] Can accept incoming connections
- [ ] No memory leaks detected
- [ ] Documentation complete

---

**Last Updated**: 2026-05-15
**Status**: Ready to begin Phase 4.1 Day 1-2