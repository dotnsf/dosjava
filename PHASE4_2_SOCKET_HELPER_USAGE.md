# Socket Helper Usage Guide
## Transparent External Helper Approach

## User Experience Goal

**ユーザーは従来通りの方法で実行できます：**
```
> djc sockconn.jav
> djvm sockconn.djc
```

外部ヘルパーの存在を意識する必要はありません。

## Implementation Design

### Approach 1: On-Demand Helper (推奨)

#### 動作フロー

1. **ユーザーが実行**:
   ```
   > djvm sockconn.djc
   ```

2. **djvm内部の動作**:
   ```
   Socket.create("192.168.1.1", 3000)
   ↓
   native_socket_create() in native.c
   ↓
   Check if sockhelp.exe exists
   ↓
   Execute: sockhelp.exe connect 192.168.1.1 3000
   ↓
   Wait for sockhelp.exe to complete
   ↓
   Read result from SOCK.OUT
   ↓
   Return socket handle to Java
   ```

3. **sockhelp.exeの動作**:
   ```
   - mTCP初期化
   - TCP接続確立
   - 結果をSOCK.OUTに書き込み
   - 終了
   ```

#### 実装例

**native.c (modified)**:
```c
static int native_socket_create(ExecutionContext* ctx, uint16_t* args, uint8_t arg_count, uint16_t* result) {
    char command[256];
    char host[64];
    uint16_t port;
    FILE* fp;
    int status;
    
    // Get host and port from arguments
    get_host_and_port(ctx, args, host, &port);
    
    // Build command
    sprintf(command, "sockhelp.exe connect %s %d", host, port);
    
    // Execute helper
    status = system(command);
    if (status != 0) {
        return -1;
    }
    
    // Read result from SOCK.OUT
    fp = fopen("SOCK.OUT", "r");
    if (!fp) {
        return -1;
    }
    
    // Parse result and return socket handle
    // ...
}
```

#### 利点
- ✅ ユーザーは何も意識しない
- ✅ 事前起動不要
- ✅ 各操作ごとに独立実行
- ✅ メモリ問題なし

#### 欠点
- ❌ 各操作でプロセス起動のオーバーヘッド
- ❌ パフォーマンスが低い

### Approach 2: Persistent Helper (高性能版)

#### 動作フロー

1. **djvm起動時に自動起動**:
   ```
   > djvm sockconn.djc
   ↓
   djvm内部で自動的に:
   start /b sockhelp.exe daemon
   ```

2. **sockhelp.exeがバックグラウンドで待機**:
   ```
   while (1) {
       // SOCK.INファイルを監視
       if (file_exists("SOCK.IN")) {
           // コマンド読み取り
           // 実行
           // 結果をSOCK.OUTに書き込み
           // SOCK.INを削除
       }
       sleep(100ms);
   }
   ```

3. **djvm終了時に自動終了**:
   ```
   // SOCK.INに"QUIT"コマンドを書き込み
   // sockhelp.exeが自動終了
   ```

#### 実装例

**djvm.c (modified)**:
```c
int main(int argc, char* argv[]) {
    // ... argument parsing ...
    
#ifdef ENABLE_SOCKETS
    // Start socket helper daemon
    system("start /b sockhelp.exe daemon");
    sleep(500);  // Wait for helper to start
#endif
    
    // ... normal VM execution ...
    
#ifdef ENABLE_SOCKETS
    // Stop socket helper daemon
    FILE* fp = fopen("SOCK.IN", "w");
    fprintf(fp, "QUIT\n");
    fclose(fp);
    sleep(500);  // Wait for helper to stop
#endif
    
    return 0;
}
```

#### 利点
- ✅ 高性能（プロセス起動は1回のみ）
- ✅ ユーザーは何も意識しない
- ✅ 複数の操作を効率的に処理

#### 欠点
- ❌ 実装が複雑
- ❌ プロセス管理が必要
- ❌ エラー処理が複雑

### Approach 3: Manual Helper (シンプル版)

#### 動作フロー

1. **ユーザーが手動で起動**:
   ```
   > sockhelp.exe daemon
   (別のウィンドウまたはバックグラウンドで実行)
   ```

2. **ユーザーがdjvmを実行**:
   ```
   > djvm sockconn.djc
   ```

3. **djvmがsockhelp.exeと通信**:
   - SOCK.INにコマンド書き込み
   - SOCK.OUTから結果読み取り

4. **終了時**:
   ```
   > sockhelp.exe quit
   ```

#### 利点
- ✅ 実装が最もシンプル
- ✅ デバッグが容易

#### 欠点
- ❌ ユーザーが手動操作必要
- ❌ 使いにくい

## 推奨実装：Approach 1 (On-Demand Helper)

### 理由

1. **ユーザー体験**: 従来通りの使い方
2. **実装の簡単さ**: system()呼び出しだけ
3. **信頼性**: 各操作が独立
4. **デバッグ**: sockhelp.exeを単独テスト可能

### パフォーマンス考慮

各socket操作でプロセス起動のオーバーヘッドがありますが：
- 16-bit DOSでは元々パフォーマンスは限定的
- 教育目的のプロジェクトには十分
- 実用的なネットワーク操作（HTTP GET等）は可能

### 実装ステップ

#### Step 1: sockhelp.exe作成
```c
// sockhelp.c
int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "connect") == 0) {
        return do_connect(argv[2], atoi(argv[3]));
    }
    else if (strcmp(argv[1], "send") == 0) {
        return do_send();
    }
    else if (strcmp(argv[1], "recv") == 0) {
        return do_recv();
    }
    else if (strcmp(argv[1], "close") == 0) {
        return do_close();
    }
    return -1;
}
```

#### Step 2: native.c修正
```c
// Socket.create() implementation
static int native_socket_create(...) {
    // Execute: sockhelp.exe connect host port
    // Read result from SOCK.OUT
    // Return socket handle
}

// Socket.send() implementation
static int native_socket_send(...) {
    // Write data to SOCK.IN
    // Execute: sockhelp.exe send
    // Read result from SOCK.OUT
}
```

#### Step 3: テスト
```
> sockhelp.exe connect 192.168.1.1 3000
> type SOCK.OUT
STATUS: OK
HANDLE: 1

> djvm sockconn.djc
Socket Connection Test
Connecting to 192.168.1.1:3000...
Connected successfully!
```

## 使用例

### 従来通りの実行方法

```batch
REM コンパイル
> djc sockconn.jav
Compiled: sockconn.jav -> sockconn.djc

REM 実行（sockhelp.exeは自動的に呼ばれる）
> djvm sockconn.djc
Socket Connection Test
Connecting to 192.168.1.1:3000...
Connected successfully!
Sending data...
Received: Hello from server
Connection closed.
```

### デバッグ時

```batch
REM sockhelp.exeを単独でテスト
> sockhelp.exe connect 192.168.1.1 3000
> type SOCK.OUT

REM djvmを実行
> djvm sockconn.djc
```

## まとめ

**ユーザーは何も変更する必要がありません：**
- コンパイル: `djc sockconn.jav`
- 実行: `djvm sockconn.djc`

sockhelp.exeは内部的に自動実行され、ユーザーからは透過的です。