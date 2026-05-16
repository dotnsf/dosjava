# Phase 4: Network Socket Implementation Plan (mTCP版)

## Overview

Phase 4 implements TCP/IP socket functionality for dosjava using **mTCP library**, enabling network communication on 16-bit PC-DOS. This phase is divided into two sub-phases:

- **Phase 4.1**: mTCP Integration and C-level Socket API (2 weeks)
- **Phase 4.2**: Java Socket Classes and VM Integration (2 weeks)

## 変更点: WattcpからmTCPへの移行

### 移行理由
1. **実績**: doscurlプロジェクトでmTCPを使用して動作確認済み
2. **互換性**: mTCPはWattcp互換APIを提供
3. **安定性**: DOSBox-X環境での動作が確認されている
4. **問題解決**: Wattcpのsock_init()失敗問題を回避

### 技術的変更点

| 項目 | Wattcp版 | mTCP版 |
|------|----------|--------|
| ライブラリ | wattcpws.lib | MTCPWS.LIB (Small model) |
| ヘッダーパス | C:\WATCOM\h\wattcp | C:\mTCP\src\TCPINC |
| 設定ファイル | wattcp.cfg | mtcp.cfg + dosjava.cfg |
| API | Wattcp API | mTCP API (Wattcp互換) |
| 参考実装 | なし | doscurl/src/socket.c |

## Goals

### Primary Goals
1. mTCP TCP/IPライブラリをdosjavaに統合
2. C-level socket wrapper functionsを実装
3. Java socket classes (Socket, ServerSocket)を作成
4. Javaコードからのネットワーク通信を実現
5. クライアント・サーバーテストプログラムで検証

### Success Criteria
- C-level socket testsがパス (tsock.exe)
- JavaクライアントがHTTPサーバーに接続可能
- Javaサーバーが接続を受け入れ可能
- 双方向データ転送が動作
- メモリ使用量がDOS制限内 (640KB)

## Technical Background

### mTCP Library
- **Purpose**: BSD Socket-compatible TCP/IP stack for DOS
- **API**: TcpSocket, DNS, Utils等のC++ API + Wattcp互換C API
- **Requirements**: Packet driver (e.g., NE2000 emulation in DOSBox-X)
- **License**: GPL v3
- **Documentation**: C:\mTCP\src\developers.pdf
- **Source**: C:\mTCP\src\TCPLIB\*.CPP
- **Headers**: C:\mTCP\src\TCPINC\*.H

### mTCP vs Wattcp API比較

#### 共通API (Wattcp互換)
```c
/* 初期化 */
void sock_init(void);

/* TCP接続 */
int tcp_open(tcp_Socket *s, word lport, longword ina, word port, dataHandler_t datahandler);
int sock_established(sock_type *s);
int sock_close(sock_type *s);

/* データ転送 */
int sock_write(sock_type *s, const byte *dp, int len);
int sock_read(sock_type *s, byte *dp, int len);
int sock_dataready(sock_type *s);

/* DNS */
longword resolve(const char *name);

/* ユーティリティ */
int tcp_tick(sock_type *s);
```

#### mTCP固有API (C++)
```cpp
/* TcpSocketクラス */
class TcpSocket {
  int8_t connect(uint16_t localPort, IpAddr_t host, uint16_t port, uint16_t timeout);
  int16_t send(uint8_t *buf, uint16_t len);
  int16_t recv(uint8_t *buf, uint16_t len);
  void close(void);
};

/* DNSクラス */
class Dns {
  static int8_t resolve(char *name, IpAddr_t ipAddr, uint8_t retry);
};
```

### Network Stack Architecture

```
┌─────────────────────────────────────┐
│   Java Application (*.jav)          │
│   Socket/ServerSocket classes       │
└─────────────────┬───────────────────┘
                  │ Java API
┌─────────────────▼───────────────────┐
│   dosjava VM (djvm.exe)             │
│   - OP_INVOKE_NATIVE bytecode       │
│   - Socket object management        │
└─────────────────┬───────────────────┘
                  │ Native calls
┌─────────────────▼───────────────────┐
│   C Runtime Socket Wrappers         │
│   - socket_create(), socket_bind()  │
│   - socket_connect(), socket_send() │
│   (doscurl/src/socket.cを参考)      │
└─────────────────┬───────────────────┘
                  │ mTCP Wattcp互換API
┌─────────────────▼───────────────────┐
│   mTCP TCP/IP Stack                 │
│   - TCP/IP protocol implementation  │
│   - Buffer management               │
│   - C:\mTCP\src\TCPLIB\*.CPP        │
└─────────────────┬───────────────────┘
                  │ Packet Driver API
┌─────────────────▼───────────────────┐
│   Packet Driver (NE2000 emulation)  │
│   - Network interface               │
└─────────────────────────────────────┘
```

## Phase 4.1: mTCP Integration and C-level Socket API

**Duration**: 2 weeks (Day 1-14)

### Week 1: mTCP Setup and Basic Socket API

#### Day 1-2: mTCP Integration and Configuration
**Goal**: mTCPライブラリのセットアップと基本動作確認

Tasks:
1. mTCPライブラリとヘッダーの確認
   - C:\mTCP\src\TCPLIB\ にソースコード
   - C:\mTCP\src\TCPINC\ にヘッダーファイル
   - MTCPWS.LIB (Small model) の場所確認

2. dosjava用設定ファイルの作成
   - `dosjava/dosjava.cfg` を作成 (doscurl/cpp/doscurl.cfgを参考)
   - mTCP設定のカスタマイズ
   - ヒープチェック無効化設定

3. Makefileの更新
   - インクルードパス: `-IC:\mTCP\src\TCPINC`
   - ライブラリリンク: `C:\mTCP\src\TCPLIB\MTCPWS.LIB`
   - コンパイルフラグ: `-DCFG_H="dosjava.cfg"`

4. 初期化テストプログラムの作成
   - `tests/network/test_mtcp_init.c` を作成
   - sock_init()の動作確認
   - _watt_do_exitフラグのチェック
   - パケットドライバ検出確認

5. DOSBox-Xでの動作確認
   - NE2000エミュレーション設定
   - パケットドライバロード
   - mtcp.cfg設定
   - テストプログラム実行

Deliverables:
- `dosjava/dosjava.cfg` - mTCP設定ファイル
- 更新された`Makefile` - mTCPリンク設定
- `tests/network/test_mtcp_init.c` - 初期化テスト
- `build/bin/tmtcp.exe` - テストプログラム
- 動作確認レポート

#### Day 3-5: C-level Socket Wrapper Functions
**Goal**: doscurlを参考にソケットラッパー関数を実装

Tasks:
1. doscurlのsocket.cを分析
   - `doscurl/src/socket.c` の実装を確認
   - `doscurl/include/socket.h` のAPIを確認
   - mTCP APIの使用方法を理解

2. dosjava用socket wrapper設計
   - doscurlのAPIをベースに設計
   - dosjavaの要件に合わせて調整
   - エラーハンドリング戦略

3. socket.h/socket.cの実装
   - `src/runtime/socket.h` を作成
   - `src/runtime/socket.c` を作成
   - doscurlのコードを参考に実装

4. 実装する関数
   ```c
   /* 初期化とクリーンアップ */
   int socket_init(void);
   void socket_cleanup(void);
   
   /* DNS解決 */
   int socket_resolve(const char *hostname, char *ip_address);
   
   /* クライアント接続 */
   socket_t socket_connect(const char *hostname, int port);
   
   /* データ転送 */
   int socket_send(socket_t sock, const char *data, int length);
   int socket_recv(socket_t sock, char *buffer, int buffer_size);
   
   /* クローズ */
   void socket_close(socket_t sock);
   ```

5. エラーハンドリング
   - mTCPのエラーコード処理
   - タイムアウト処理
   - 接続失敗処理

Files to create:
- `src/runtime/socket.h` - Socket wrapper API declarations
- `src/runtime/socket.c` - Socket wrapper implementations (doscurl参考)

Key implementation points:
```c
/* doscurlからの参考実装 */

// 初期化
int socket_init(void) {
    sock_init();
    if (_watt_do_exit) {
        return ERROR_NETWORK;
    }
    return SUCCESS;
}

// 接続
socket_t socket_connect(const char *hostname, int port) {
    DWORD host_ip = resolve(hostname);
    tcp_Socket *s = (tcp_Socket *)malloc(sizeof(tcp_Socket));
    
    if (!_w32_tcp_open(s, 0, host_ip, port, NULL)) {
        free(s);
        return -1;
    }
    
    // 接続確立待ち
    int timeout = sock_delay;
    while (!sock_established((sock_type *)s) && timeout > 0) {
        if (!tcp_tick((sock_type *)s)) {
            sock_close((sock_type *)s);
            free(s);
            return -1;
        }
        timeout--;
    }
    
    return (socket_t)s;
}

// 送信
int socket_send(socket_t sock, const char *data, int length) {
    sock_type *s = (sock_type *)sock;
    int sent = sock_write(s, (BYTE *)data, length);
    _w32_sock_flush(s);
    return sent;
}

// 受信
int socket_recv(socket_t sock, char *buffer, int buffer_size) {
    sock_type *s = (sock_type *)sock;
    
    // データ待ち
    int timeout = sock_delay;
    while (sock_dataready(s) <= 0 && timeout > 0) {
        if (!sock_established(s)) {
            return 0;  // 接続クローズ
        }
        if (!tcp_tick(s)) {
            return 0;
        }
        timeout--;
    }
    
    return sock_read(s, (BYTE *)buffer, buffer_size);
}
```

#### Day 6-7: C-level Socket Testing
**Goal**: ソケットラッパー関数の動作確認

Tasks:
1. テストプログラムの作成
   - `tests/network/test_socket.c` を作成
   - doscurlの動作を参考にテストケース設計

2. テストケース
   - Test 1: Socket initialization (socket_init)
   - Test 2: DNS resolution (socket_resolve)
   - Test 3: TCP connection (socket_connect)
   - Test 4: Data send (socket_send)
   - Test 5: Data receive (socket_recv)
   - Test 6: Socket close (socket_close)
   - Test 7: Error handling
   - Test 8: Multiple connections

3. DOSBox-Xでのテスト
   - HTTPサーバーへの接続テスト
   - エコーサーバーとの通信テスト
   - エラーケースのテスト

4. テスト結果のドキュメント化
   - 成功/失敗の記録
   - パフォーマンス測定
   - 問題点の洗い出し

Deliverables:
- `tests/network/test_socket.c` - Socket wrapper tests
- `build/bin/tsock.exe` - Compiled test program
- `tests/network/TEST_RESULTS.md` - Test results documentation

### Week 2: Advanced Socket Features and Integration

#### Day 8-10: Non-blocking I/O and Timeouts
**Goal**: 非ブロッキングI/Oとタイムアウト機能の追加

Tasks:
1. タイムアウト機能の実装
   - `socket_set_timeout()` 関数
   - 接続タイムアウト
   - 送受信タイムアウト

2. 非ブロッキングモードの実装
   - `socket_set_nonblocking()` 関数
   - ポーリングベースの実装
   - tcp_tick()を使用した状態確認

3. データ可用性チェック
   - `socket_available()` 関数
   - sock_dataready()の活用

4. テストとデバッグ
   - タイムアウト動作の確認
   - 非ブロッキング動作の確認

Deliverables:
- 拡張されたsocket.h/socket.c
- タイムアウト/非ブロッキングのテスト

#### Day 11-12: Memory Management and Optimization
**Goal**: DOSメモリ制約に対する最適化

Tasks:
1. メモリ使用量の分析
   - 現在のメモリ使用量測定
   - mTCPのバッファ設定確認
   - ソケット構造体のサイズ確認

2. バッファサイズの最適化
   - 送受信バッファサイズの調整
   - dosjava.cfgでの設定
   - Small modelの制約考慮

3. ソケットプールの実装
   - ソケット再利用機構
   - 最大同時接続数の制限
   - メモリリーク防止

4. プロファイリング
   - メモリ使用量の測定
   - パフォーマンス測定
   - 最適化の効果確認

Deliverables:
- 最適化されたdosjava.cfg
- メモリプロファイリング結果
- 最適化ドキュメント

#### Day 13-14: Documentation and Cleanup
**Goal**: C-level socket APIのドキュメント化

Tasks:
1. API仕様書の作成
   - `PHASE4_SOCKET_API.md` を作成
   - 全関数の詳細説明
   - 使用例とサンプルコード

2. 実装ノートの作成
   - mTCP統合の詳細
   - doscurlとの違い
   - トラブルシューティング

3. コードクリーンアップ
   - デバッグコードの削除
   - コメントの整理
   - コーディング規約の確認

4. Phase 4.2の準備
   - Java統合の設計レビュー
   - 必要な変更の洗い出し

Deliverables:
- `PHASE4_SOCKET_API.md` - C-level API documentation
- `PHASE4_MTCP_NOTES.md` - Implementation notes
- クリーンアップされたソースコード

## Phase 4.2: Java Socket Classes and VM Integration

**Duration**: 2 weeks (Day 15-28)

### Week 3: Java Socket Classes

#### Day 15-17: Socket and ServerSocket Classes
**Goal**: Java socket classesの実装

Tasks:
1. Java socket class設計
   - Socket class (client)
   - ServerSocket class (server)
   - SocketInputStream/SocketOutputStream

2. VM内部構造の設計
   - Socket objectの表現
   - Native method binding
   - Object lifecycle management

3. 実装
   - `src/runtime/socket_java.h` - Java socket structures
   - `src/runtime/socket_java.c` - Java socket implementations

Java API design:
```java
class Socket {
    Socket(String host, int port);  // Client socket
    void close();
    InputStream getInputStream();
    OutputStream getOutputStream();
}

class ServerSocket {
    ServerSocket(int port);  // Server socket
    Socket accept();
    void close();
}

class SocketInputStream extends InputStream {
    int read();
    int available();
}

class SocketOutputStream extends OutputStream {
    void write(int b);
    void flush();
}
```

4. テスト
   - 基本的なSocket作成テスト
   - InputStream/OutputStreamテスト

Files to create:
- `src/runtime/socket_java.h`
- `src/runtime/socket_java.c`

#### Day 18-19: Native Method Mechanism
**Goal**: VMからnative methodを呼び出す機構の実装

Tasks:
1. OP_INVOKE_NATIVE bytecodeの追加
   - `src/format/opcodes.h` に定義追加
   - コンパイラでの生成サポート

2. Native method registryの実装
   - `src/vm/native.h` - Native method registry
   - `src/vm/native.c` - Registry implementation

3. Native method dispatcherの実装
   - `src/vm/interpreter.c` でOP_INVOKE_NATIVE処理
   - Method name/signatureからnative functionへのマッピング

4. Socket native methodsの登録
   - Socket.connect() -> socket_connect()
   - Socket.send() -> socket_send()
   - Socket.recv() -> socket_recv()
   - etc.

VM changes:
- `src/format/opcodes.h` - Add OP_INVOKE_NATIVE
- `src/vm/interpreter.c` - Implement native method dispatch
- `src/vm/native.h` - Native method registry
- `src/vm/native.c` - Native method implementations

#### Day 20-21: VM Socket Integration
**Goal**: Socket objectsとVMの統合

Tasks:
1. OP_NEWでのSocket object作成
   - Socket/ServerSocket用の特別処理
   - Native handleの割り当て

2. Socket method callsの実装
   - Method dispatchの確認
   - 引数/戻り値の変換

3. Socket cleanupの実装
   - VM shutdown時のクリーンアップ
   - GC時のSocket close

4. Lifecycle testing
   - Socket作成から破棄までのテスト
   - メモリリークチェック

### Week 4: Testing and Finalization

#### Day 22-24: Java Socket Testing
**Goal**: 包括的なJava socketテストの作成

Tasks:
1. Echo serverテストの作成
   - `tests/network/echoserv.jav` - Echo server
   - ServerSocketでポート待ち受け
   - 受信データをそのまま返送

2. Echo clientテストの作成
   - `tests/network/echocli.jav` - Echo client
   - Socketで接続
   - データ送受信テスト

3. HTTP GETクライアントの作成
   - `tests/network/httpget.jav` - HTTP GET client
   - 実際のHTTPサーバーへの接続
   - レスポンス受信と表示

4. エラーハンドリングテスト
   - 接続失敗のテスト
   - タイムアウトのテスト
   - 不正なホスト名のテスト

Test files:
- `tests/network/echoserv.jav` - Echo server
- `tests/network/echocli.jav` - Echo client
- `tests/network/httpget.jav` - HTTP GET client
- `tests/network/README.md` - Test documentation

#### Day 25-26: Integration Testing
**Goal**: エンドツーエンド機能の検証

Tasks:
1. 外部サーバーとのテスト
   - HTTPサーバーへの接続
   - レスポンスの正確性確認

2. 複数同時接続のテスト
   - 複数Socketの同時使用
   - メモリ使用量の確認

3. 大容量データ転送のテスト
   - 大きなファイルのダウンロード
   - バッファリングの確認

4. パフォーマンス測定
   - スループット測定
   - レイテンシ測定
   - メモリ使用量測定

#### Day 27-28: Documentation and Cleanup
**Goal**: Phase 4ドキュメントの完成

Tasks:
1. ユーザーガイドの作成
   - `PHASE4_NETWORK_QUICKSTART.md` - Quick start guide
   - Socket programmingの基本
   - サンプルプログラムの説明

2. 制限事項と既知の問題
   - DOSの制約
   - mTCPの制限
   - 既知のバグ

3. サンプルプログラムの作成
   - 簡単なHTTPクライアント
   - 簡単なTCPサーバー
   - チャットプログラム

4. コードの最適化とクリーンアップ
   - 最終的なコードレビュー
   - パフォーマンス最適化
   - ドキュメントの完成

Deliverables:
- `PHASE4_NETWORK_QUICKSTART.md` - Quick start guide
- `PHASE4_MTCP_TASKS.md` - Task checklist (updated)
- Working socket implementation
- Example programs
- Complete documentation

## Memory Considerations

### DOS Memory Constraints
- **Total**: 640KB conventional memory
- **VM**: ~100KB (code + data)
- **mTCP**: ~50KB (TCP/IP stack)
- **Buffers**: ~50KB (socket buffers)
- **Application**: ~400KB remaining

### mTCP Configuration (dosjava.cfg)
```c
#define MTCP_PROGRAM_NAME "dosjava"

// Disable heap checking
#define UTILS_CHECK_HEAP (0)
#define UTILS_HEAP_CHECK_LEVEL (0)

// Include global mTCP configuration
#include "GLOBAL.CFG"

// Local TCP/IP library options
#define COMPILE_ARP
#define IP_FRAGMENTS_ON
#define COMPILE_UDP
#define COMPILE_TCP
#define COMPILE_DNS

// Override defaults for dosjava
#undef TCP_MAX_SOCKETS
#define TCP_MAX_SOCKETS (4)  // 最大4ソケット

#undef PACKET_BUFFERS
#define PACKET_BUFFERS (10)  // 小さめのバッファ数

// Application specific settings
#define DOSJAVA_VERSION "1.0.0"
```

### Optimization Strategies
1. Small socket buffers (1-2KB per socket)
2. Limit maximum concurrent connections (4)
3. Reuse socket objects when possible
4. Implement buffer pooling
5. Profile and optimize hot paths
6. Use Small memory model (MTCPWS.LIB)

## Testing Strategy

### Test Levels

1. **Unit Tests** (C-level)
   - Socket wrapper functions
   - Individual API calls
   - Error conditions

2. **Integration Tests** (C-level)
   - Multiple socket operations
   - Client-server communication
   - Memory management

3. **System Tests** (Java-level)
   - Java Socket classes
   - Real-world scenarios
   - Performance testing

### Test Environment

DOSBox-X Configuration:
```ini
[ne2000]
ne2000=true
nicirq=3
nicbase=300

[autoexec]
# Load packet driver
ne2000 0x60 3 0x300

# Set mTCP configuration
SET MTCPCFG=D:\MTCP.CFG

# Run tests
D:
CD \DOSJAVA\BUILD\BIN
```

mTCP Configuration (MTCP.CFG):
```
PACKETINT 0x60
IPADDR 192.168.0.123
NETMASK 255.255.255.0
GATEWAY 192.168.0.1
NAMESERVER 8.8.8.8
```

## Reference Implementation: doscurl

### Key Files to Reference
- `doscurl/src/socket.c` - Socket wrapper implementation
- `doscurl/include/socket.h` - Socket API definitions
- `doscurl/cpp/doscurl.cfg` - mTCP configuration
- `doscurl/Makefile` - Build configuration

### API Mapping

| dosjava Function | doscurl Function | mTCP API |
|------------------|------------------|----------|
| socket_init() | socket_init() | sock_init() |
| socket_resolve() | socket_resolve() | resolve() |
| socket_connect() | socket_connect() | _w32_tcp_open() |
| socket_send() | socket_send() | sock_write() |
| socket_recv() | socket_recv() | sock_read() |
| socket_close() | socket_close() | sock_close() |

## Risk Mitigation

### Identified Risks

1. **Memory Constraints**
   - Risk: mTCP + VM exceeds 640KB
   - Mitigation: Use Small model, optimize buffers, limit connections

2. **API Compatibility**
   - Risk: mTCP API differences from Wattcp
   - Mitigation: Use doscurl as reference, test thoroughly

3. **Performance**
   - Risk: Slow network performance
   - Mitigation: Optimize buffer sizes, use non-blocking I/O

4. **Stability**
   - Risk: Crashes or hangs
   - Mitigation: Extensive testing, proper error handling

## Success Metrics

### Phase 4.1 Success Criteria
- [ ] mTCP successfully integrated
- [ ] All C-level socket tests pass
- [ ] Memory usage < 150KB for socket layer
- [ ] Can connect to external HTTP server
- [ ] Can send and receive data reliably

### Phase 4.2 Success Criteria
- [ ] Java Socket class works
- [ ] Java ServerSocket class works
- [ ] Echo server/client test passes
- [ ] HTTP GET client works
- [ ] Memory usage stays within limits
- [ ] No memory leaks detected

## Deliverables Summary

### Phase 4.1 Deliverables
1. mTCP integration (Makefile, dosjava.cfg)
2. Socket wrapper library (socket.h, socket.c)
3. C-level tests (test_mtcp_init.c, test_socket.c)
4. Documentation (PHASE4_SOCKET_API.md, PHASE4_MTCP_NOTES.md)

### Phase 4.2 Deliverables
1. Java socket classes (socket_java.h, socket_java.c)
2. Native method mechanism (native.h, native.c)
3. VM integration (interpreter.c updates)
4. Java tests (echoserv.jav, echocli.jav, httpget.jav)
5. Documentation (PHASE4_NETWORK_QUICKSTART.md)

## Next Steps

After Phase 4 completion:
1. Review and test all functionality
2. Optimize performance
3. Document lessons learned
4. Plan Phase 5 (if needed)

---

**Note**: This plan is based on successful doscurl implementation using mTCP. The approach has been proven to work in DOSBox-X environment with NE2000 emulation.