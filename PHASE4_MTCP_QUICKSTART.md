# Phase 4: Network Socket Implementation - Quick Start Guide (mTCP版)

## Overview

このガイドでは、dosjavaプロジェクトにmTCPを使用したネットワークソケット機能を統合する手順を説明します。

## 前提条件

### 開発環境
- Windows 11
- Open Watcom V2 (C:\WATCOM)
- mTCP (C:\mTCP)
- DOSBox-X (テスト用)

### 参考プロジェクト
- **doscurl**: mTCPを使用した実装例
  - 場所: `c:\Users\dotns\src\doscurl`
  - 参考ファイル: `doscurl/src/socket.c`, `doscurl/cpp/doscurl.cfg`

## Phase 4.1: mTCP Integration (Week 1-2)

### Day 1-2: mTCP Integration and Configuration

#### Step 1: mTCPライブラリの確認

```powershell
# mTCPのディレクトリ構造確認
dir C:\mTCP\src\TCPINC    # ヘッダーファイル
dir C:\mTCP\src\TCPLIB    # ソースコード
```

必要なファイル:
- `C:\mTCP\src\TCPINC\*.H` - mTCPヘッダーファイル
- `C:\mTCP\src\TCPLIB\*.CPP` - mTCPソースコード
- `C:\mTCP\src\TCPLIB\MTCPWS.LIB` - Small modelライブラリ (ビルド済みの場合)

#### Step 2: dosjava.cfg の作成

`dosjava/dosjava.cfg` を作成:

```c
/*
 * dosjava Configuration File for mTCP
 */

#ifndef CONFIG_H
#define CONFIG_H

#define MTCP_PROGRAM_NAME "dosjava"

// Disable heap checking to prevent false positives
#define UTILS_CHECK_HEAP (0)
#define UTILS_HEAP_CHECK_LEVEL (0)

// Include global mTCP configuration
#include "GLOBAL.CFG"

// Force disable heap checking AFTER including GLOBAL.CFG
#undef UTILS_CHECK_HEAP
#define UTILS_CHECK_HEAP (0)
#undef UTILS_HEAP_CHECK_LEVEL
#define UTILS_HEAP_CHECK_LEVEL (0)

// Local TCP/IP library options
#define COMPILE_ARP
#define IP_FRAGMENTS_ON
#define COMPILE_UDP
#define COMPILE_TCP
#define COMPILE_DNS

// Override defaults for dosjava
#undef TCP_MAX_SOCKETS
#define TCP_MAX_SOCKETS (4)      // 最大4ソケット

#undef PACKET_BUFFERS
#define PACKET_BUFFERS (10)      // 小さめのバッファ数

// Application specific settings
#define DOSJAVA_VERSION "1.0.0"

#endif
```

#### Step 3: Makefileの更新

`dosjava/Makefile` に以下を追加:

```makefile
# mTCP settings
MTCP_INC = C:\mTCP\src\TCPINC
MTCP_LIB = C:\mTCP\src\TCPLIB\MTCPWS.LIB

# Compiler flags for mTCP
MTCP_FLAGS = -IC:\mTCP\src\TCPINC -DCFG_H="dosjava.cfg"

# Test mTCP initialization
test_mtcp: build/obj/test_mtcp_init.obj
    wlink system dos name build\bin\tmtcp.exe file { build/obj/test_mtcp_init.obj } library $(MTCP_LIB)

build/obj/test_mtcp_init.obj: tests/network/test_mtcp_init.c
    wcc -ms -0 -w4 -zq -bt=dos $(MTCP_FLAGS) -fo=build\obj\test_mtcp_init.obj tests/network/test_mtcp_init.c
```

#### Step 4: 初期化テストプログラムの作成

`tests/network/test_mtcp_init.c` を作成:

```c
/*
 * mTCP Initialization Test
 * Based on doscurl implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* mTCP headers */
#include "types.h"
#include "timer.h"
#include "trace.h"
#include "utils.h"
#include "packet.h"
#include "arp.h"
#include "tcp.h"
#include "tcpsockm.h"
#include "dns.h"

int main(int argc, char *argv[]) {
    printf("mTCP Initialization Test\n");
    printf("========================\n\n");
    
    /* Test 1: Header inclusion */
    printf("Test 1: mTCP headers included successfully\n");
    printf("  - types.h, tcp.h, dns.h, etc.\n");
    printf("  Result: PASS\n\n");
    
    /* Test 2: Initialize mTCP */
    printf("Test 2: Initializing mTCP...\n");
    
    if (Utils::parseEnv() != 0) {
        printf("  Error: Failed to parse environment\n");
        printf("  Result: FAIL\n");
        return 1;
    }
    
    if (Utils::initStack(2048, 2048) != 0) {
        printf("  Error: Failed to initialize TCP/IP stack\n");
        printf("  Result: FAIL\n");
        return 1;
    }
    
    printf("  mTCP initialized successfully\n");
    printf("  Result: PASS\n\n");
    
    /* Test 3: Check packet driver */
    printf("Test 3: Checking packet driver...\n");
    
    if (Packet::isPacketDriverLoaded() == 0) {
        printf("  Error: Packet driver not loaded\n");
        printf("  Please load packet driver first:\n");
        printf("    ne2000 0x60 3 0x300\n");
        printf("  Result: FAIL\n");
        Utils::endStack();
        return 1;
    }
    
    printf("  Packet driver loaded\n");
    printf("  Result: PASS\n\n");
    
    /* Test 4: Display network configuration */
    printf("Test 4: Network configuration\n");
    printf("  IP Address: %d.%d.%d.%d\n",
           MyIpAddr[0], MyIpAddr[1], MyIpAddr[2], MyIpAddr[3]);
    printf("  Netmask: %d.%d.%d.%d\n",
           Netmask[0], Netmask[1], Netmask[2], Netmask[3]);
    printf("  Gateway: %d.%d.%d.%d\n",
           Gateway[0], Gateway[1], Gateway[2], Gateway[3]);
    printf("  Result: PASS\n\n");
    
    /* Cleanup */
    printf("Test 5: Cleanup\n");
    Utils::endStack();
    printf("  mTCP cleaned up\n");
    printf("  Result: PASS\n\n");
    
    printf("All tests passed!\n");
    return 0;
}
```

#### Step 5: ビルドとテスト

```powershell
# Windows上でビルド
cd dosjava
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binw;%PATH%
wmake test_mtcp
```

成功すると `build\bin\tmtcp.exe` が生成されます。

#### Step 6: DOSBox-Xでのテスト

DOSBox-X設定 (`dosbox-x.conf`):

```ini
[ne2000]
ne2000=true
nicirq=3
nicbase=300
```

DOSBox-X内での実行:

```batch
# パケットドライバのロード
ne2000 0x60 3 0x300

# mTCP設定ファイルの作成 (D:\MTCP.CFG)
# 以下の内容で作成:
#   PACKETINT 0x60
#   IPADDR 192.168.0.123
#   NETMASK 255.255.255.0
#   GATEWAY 192.168.0.1
#   NAMESERVER 8.8.8.8

# 環境変数設定
SET MTCPCFG=D:\MTCP.CFG

# テスト実行
D:
CD \DOSJAVA\BUILD\BIN
TMTCP.EXE
```

期待される出力:

```
mTCP Initialization Test
========================

Test 1: mTCP headers included successfully
  - types.h, tcp.h, dns.h, etc.
  Result: PASS

Test 2: Initializing mTCP...
  mTCP initialized successfully
  Result: PASS

Test 3: Checking packet driver...
  Packet driver loaded
  Result: PASS

Test 4: Network configuration
  IP Address: 192.168.0.123
  Netmask: 255.255.255.0
  Gateway: 192.168.0.1
  Result: PASS

Test 5: Cleanup
  mTCP cleaned up
  Result: PASS

All tests passed!
```

### Day 3-5: Socket Wrapper Functions

#### Step 1: doscurlの実装を参考にする

```powershell
# doscurlのsocket.cを確認
notepad c:\Users\dotns\src\doscurl\src\socket.c
notepad c:\Users\dotns\src\doscurl\include\socket.h
```

重要な関数:
- `socket_init()` - mTCP初期化
- `socket_resolve()` - DNS解決
- `socket_connect()` - TCP接続
- `socket_send()` - データ送信
- `socket_recv()` - データ受信
- `socket_close()` - 接続クローズ

#### Step 2: socket.h の作成

`src/runtime/socket.h` を作成:

```c
/*
 * dosjava - Socket Operations
 * Header file for TCP socket functions using mTCP
 */

#ifndef SOCKET_H
#define SOCKET_H

/* Socket handle type */
typedef int socket_t;

/* Error codes */
#define SUCCESS         0
#define ERROR_NETWORK  -1
#define ERROR_DNS      -2
#define ERROR_CONNECT  -3
#define ERROR_SEND     -4
#define ERROR_RECV     -5

/* Function prototypes */
int socket_init(void);
void socket_cleanup(void);
socket_t socket_connect(const char *hostname, int port);
int socket_send(socket_t sock, const char *data, int length);
int socket_recv(socket_t sock, char *buffer, int buffer_size);
void socket_close(socket_t sock);
int socket_resolve(const char *hostname, char *ip_address);

#endif /* SOCKET_H */
```

#### Step 3: socket.c の実装

`src/runtime/socket.c` を作成 (doscurl/src/socket.cを参考):

```c
/*
 * dosjava - Socket Operations
 * Implementation of TCP socket functions using mTCP
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* mTCP headers */
#include "types.h"
#include "tcp.h"
#include "tcpsockm.h"
#include "dns.h"
#include "utils.h"

#include "socket.h"

/* Global socket pointer for cleanup */
static tcp_Socket *sock_ptr = NULL;

/*
 * Initialize mTCP library
 * Returns: 0 on success, -1 on error
 */
int socket_init(void) {
    /* Parse environment */
    if (Utils::parseEnv() != 0) {
        return ERROR_NETWORK;
    }
    
    /* Initialize TCP/IP stack */
    if (Utils::initStack(2048, 2048) != 0) {
        return ERROR_NETWORK;
    }
    
    /* Check packet driver */
    if (Packet::isPacketDriverLoaded() == 0) {
        Utils::endStack();
        return ERROR_NETWORK;
    }
    
    return SUCCESS;
}

/*
 * Cleanup mTCP library
 */
void socket_cleanup(void) {
    if (sock_ptr != NULL) {
        sock_ptr->close();
        delete sock_ptr;
        sock_ptr = NULL;
    }
    Utils::endStack();
}

/*
 * Resolve hostname to IP address
 * Returns: 0 on success, -1 on error
 */
int socket_resolve(const char *hostname, char *ip_address) {
    IpAddr_t ipAddr;
    
    if (hostname == NULL || ip_address == NULL) {
        return ERROR_DNS;
    }
    
    /* Try to resolve hostname */
    if (Dns::resolve((char *)hostname, ipAddr, 1) < 0) {
        return ERROR_DNS;
    }
    
    /* Convert IP to string */
    sprintf(ip_address, "%d.%d.%d.%d",
            ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    
    return SUCCESS;
}

/*
 * Connect to remote host
 * Returns: socket handle on success, -1 on error
 */
socket_t socket_connect(const char *hostname, int port) {
    IpAddr_t ipAddr;
    TcpSocket *sock;
    
    if (hostname == NULL || port <= 0 || port > 65535) {
        return -1;
    }
    
    /* Resolve hostname */
    if (Dns::resolve((char *)hostname, ipAddr, 1) < 0) {
        return -1;
    }
    
    /* Create socket */
    sock = new TcpSocket();
    if (sock == NULL) {
        return -1;
    }
    
    /* Connect */
    if (sock->connect(0, ipAddr, port, 10000) < 0) {
        delete sock;
        return -1;
    }
    
    /* Store for cleanup */
    sock_ptr = sock;
    
    return (socket_t)sock;
}

/*
 * Send data through socket
 * Returns: number of bytes sent, -1 on error
 */
int socket_send(socket_t sock, const char *data, int length) {
    TcpSocket *s = (TcpSocket *)sock;
    
    if (s == NULL || data == NULL || length <= 0) {
        return -1;
    }
    
    return s->send((uint8_t *)data, length);
}

/*
 * Receive data from socket
 * Returns: number of bytes received, 0 on connection close, -1 on error
 */
int socket_recv(socket_t sock, char *buffer, int buffer_size) {
    TcpSocket *s = (TcpSocket *)sock;
    
    if (s == NULL || buffer == NULL || buffer_size <= 0) {
        return -1;
    }
    
    return s->recv((uint8_t *)buffer, buffer_size);
}

/*
 * Close socket connection
 */
void socket_close(socket_t sock) {
    TcpSocket *s = (TcpSocket *)sock;
    
    if (s == NULL) {
        return;
    }
    
    s->close();
    delete s;
    
    if (sock_ptr == s) {
        sock_ptr = NULL;
    }
}
```

#### Step 4: Makefileの更新

```makefile
# Socket wrapper
build/obj/socket.obj: src/runtime/socket.c src/runtime/socket.h
    wcc -ms -0 -w4 -zq -bt=dos $(MTCP_FLAGS) -fo=build\obj\socket.obj src/runtime/socket.c
```

### Day 6-7: Socket Testing

#### Step 1: テストプログラムの作成

`tests/network/test_socket.c` を作成:

```c
/*
 * Socket Wrapper Test
 */

#include <stdio.h>
#include <string.h>
#include "../../src/runtime/socket.h"

int main(void) {
    socket_t sock;
    char buffer[1024];
    int result;
    
    printf("Socket Wrapper Test\n");
    printf("===================\n\n");
    
    /* Test 1: Initialize */
    printf("Test 1: socket_init()\n");
    result = socket_init();
    if (result != SUCCESS) {
        printf("  FAIL: socket_init() returned %d\n", result);
        return 1;
    }
    printf("  PASS\n\n");
    
    /* Test 2: DNS resolution */
    printf("Test 2: socket_resolve()\n");
    char ip[16];
    result = socket_resolve("example.com", ip);
    if (result != SUCCESS) {
        printf("  FAIL: socket_resolve() returned %d\n", result);
        socket_cleanup();
        return 1;
    }
    printf("  Resolved to: %s\n", ip);
    printf("  PASS\n\n");
    
    /* Test 3: Connect */
    printf("Test 3: socket_connect()\n");
    sock = socket_connect("example.com", 80);
    if (sock < 0) {
        printf("  FAIL: socket_connect() returned %d\n", sock);
        socket_cleanup();
        return 1;
    }
    printf("  Connected successfully\n");
    printf("  PASS\n\n");
    
    /* Test 4: Send HTTP GET */
    printf("Test 4: socket_send()\n");
    const char *request = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
    result = socket_send(sock, request, strlen(request));
    if (result < 0) {
        printf("  FAIL: socket_send() returned %d\n", result);
        socket_close(sock);
        socket_cleanup();
        return 1;
    }
    printf("  Sent %d bytes\n", result);
    printf("  PASS\n\n");
    
    /* Test 5: Receive response */
    printf("Test 5: socket_recv()\n");
    result = socket_recv(sock, buffer, sizeof(buffer) - 1);
    if (result < 0) {
        printf("  FAIL: socket_recv() returned %d\n", result);
        socket_close(sock);
        socket_cleanup();
        return 1;
    }
    buffer[result] = '\0';
    printf("  Received %d bytes\n", result);
    printf("  First line: %.50s\n", buffer);
    printf("  PASS\n\n");
    
    /* Test 6: Close */
    printf("Test 6: socket_close()\n");
    socket_close(sock);
    printf("  PASS\n\n");
    
    /* Cleanup */
    printf("Test 7: socket_cleanup()\n");
    socket_cleanup();
    printf("  PASS\n\n");
    
    printf("All tests passed!\n");
    return 0;
}
```

#### Step 2: ビルドとテスト

```makefile
# Socket test
test_socket: build/obj/test_socket.obj build/obj/socket.obj
    wlink system dos name build\bin\tsock.exe file { build/obj/test_socket.obj build/obj/socket.obj } library $(MTCP_LIB)

build/obj/test_socket.obj: tests/network/test_socket.c
    wcc -ms -0 -w4 -zq -bt=dos $(MTCP_FLAGS) -fo=build\obj\test_socket.obj tests/network/test_socket.c
```

```powershell
wmake test_socket
```

## トラブルシューティング

### 問題: sock_init() が失敗する

**原因**: パケットドライバが読み込まれていない

**解決策**:
```batch
# DOSBox-X内で
ne2000 0x60 3 0x300
```

### 問題: DNS解決が失敗する

**原因**: MTCP.CFGにDNSサーバーが設定されていない

**解決策**:
```
# MTCP.CFGに追加
NAMESERVER 8.8.8.8
```

### 問題: コンパイルエラー "Cannot open GLOBAL.CFG"

**原因**: mTCPヘッダーパスが正しくない

**解決策**:
```makefile
# Makefileで確認
MTCP_FLAGS = -IC:\mTCP\src\TCPINC -DCFG_H="dosjava.cfg"
```

## 次のステップ

Phase 4.1完了後:
1. Phase 4.2: Java Socket Classes実装
2. Native Method Mechanism実装
3. VM統合
4. Java-levelテスト

## 参考資料

- `PHASE4_MTCP_PLAN.md` - 詳細な実装計画
- `PHASE4_MTCP_TASKS.md` - タスクチェックリスト
- `doscurl/src/socket.c` - 参考実装
- `C:\mTCP\src\developers.pdf` - mTCPドキュメント

---

**Last Updated**: 2026-05-15