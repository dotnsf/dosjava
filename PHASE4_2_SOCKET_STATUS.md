# Phase 4.2 Socket Implementation - Current Status

## Date: 2026-05-21

## Summary
Socket API実装において、外部ヘルパープログラム（sockhelp.exe/sockhlp2.exe）を使用するアプローチを採用しましたが、接続タイムアウトの問題が未解決のまま残っています。

## Current Situation

### What Works
1. ✅ **mTCP初期化**: 正常に動作
2. ✅ **DNS解決**: Dns::resolve()が正常に動作（IPアドレスを192.168.0.2に解決）
3. ✅ **ソケット取得**: TcpSocketMgr::getSocket()が成功
4. ✅ **バッファ設定**: setRecvBuffer()が成功
5. ✅ **接続開始**: connectNonBlocking()が成功
6. ✅ **タイムアウト計算**: TIMER_MS_TO_TICKS()が正しく動作（1090 ticks）
7. ✅ **doscurl.exe**: 同じ環境で正常に接続・通信可能

### What Doesn't Work
1. ❌ **TCP接続完了**: isConnectComplete()が常に0のまま
2. ❌ **サーバー応答**: サーバーからのSYN-ACKが届いていない可能性
3. ❌ **接続タイムアウト**: 約1秒後（1091 ticks）にタイムアウト

## Technical Details

### Test Environment
- **OS**: 16-bit PC-DOS (DOSBox-X)
- **Network**: mTCP with packet driver
- **Server**: 192.168.0.2:8080 (HTTP server)
- **Configuration**: doscurl.cfg (Large memory model, 4 TCP sockets, 4 transmit buffers)

### Debug Output (sockhlp2.exe)
```
Initializing mTCP...
mTCP initialized successfully
Resolving host: 192.168.0.2
Resolved to: 192.168.0.2
Connecting to server...
DEBUG: CONNECT_TIMEOUT constant = -5536  ← 16-bit int overflow (should use UL suffix)
DEBUG: CONNECT_TIMEOUT as unsigned long = 60000
DEBUG: Local port = 34689
DEBUG: start ticks = 4
DEBUG: timeout_ticks = 1090
DEBUG: Expected timeout at ~1092 ticks (60000ms * 18.2 / 1000)
DEBUG: Loop 0 - elapsed=0, timeout=1090, complete=0, closed=0
DEBUG: Loop 1000 - elapsed=988, timeout=1090, complete=0, closed=0
connection timeout after 60000 ms (elapsed ticks=1091, timeout ticks=1090)
Connection failed
```

### Comparison: doscurl.exe vs sockhlp2.exe

| Aspect | doscurl.exe | sockhlp2.exe | Status |
|--------|-------------|--------------|--------|
| mTCP initialization | initStack(2, 4, ...) | initStack(2, 4, ...) | ✅ Same |
| Configuration file | doscurl.cfg | doscurl.cfg | ✅ Same |
| Memory model | Large (-ml) | Large (-ml) | ✅ Same |
| DNS resolution | Dns::resolve() | Dns::resolve() | ✅ Same |
| Connection logic | connectNonBlocking() | connectNonBlocking() | ✅ Same |
| Packet processing | PACKET_PROCESS_SINGLE | PACKET_PROCESS_SINGLE | ✅ Same |
| Result | ✅ Success | ❌ Timeout | ❌ Different |

## Possible Root Causes

### 1. Timing Issue
- sockhlp2が接続を試みるタイミングが早すぎる可能性
- mTCPの内部状態が完全に準備されていない

### 2. Network Stack State
- doscurlが何らかの初期化を行っている可能性
- sockhlp2が見落としている初期化ステップがある

### 3. Packet Driver Issue
- パケットドライバーの状態が影響している可能性
- 連続実行時の状態管理の問題

### 4. Memory Layout
- Large memory modelでのセグメント配置の違い
- スタックサイズ（8KB）が不十分な可能性

### 5. Unknown mTCP Requirement
- mTCPが要求する、ドキュメント化されていない初期化手順
- doscurlが偶然満たしている条件

## Files Created

### Source Files
- `tools/sockhelp.c` - Original socket helper (684 lines)
- `tools/sockhelp_v2.c` - doscurl-based version (376 lines)

### Build Scripts
- `build_sockhelp.bat` - Build original sockhelp.exe
- `build_sockhelp_v2.bat` - Build sockhlp2.exe

### Test Scripts
- `build/bin/tsockh2.bat` - Test script for sockhlp2.exe

### Documentation
- `SOCKET_INIT_DEBUG_PLAN.md` - Initial debug plan
- `PHASE4_2_SOCKET_WORKAROUND_PLAN.md` - External helper approach
- `PHASE4_2_SOCKET_HELPER_USAGE.md` - Usage guide
- `SOCKHLP2_TEST_PLAN.md` - Test plan for sockhlp2
- `PHASE4_2_SOCKET_STATUS.md` - This file

## Next Steps (Deferred)

### Short-term Options
1. **Add more debug output**: Trace mTCP internal state
2. **Test with delay**: Add sleep before connection attempt
3. **Compare binary**: Analyze doscurl.exe vs sockhlp2.exe
4. **Packet capture**: Use DOSBox-X network logging

### Long-term Options
1. **Alternative approach**: Use doscurl.exe directly with wrapper
2. **Direct integration**: Attempt mTCP integration in djvm.exe again
3. **Different library**: Consider alternative TCP/IP stack
4. **Simplified protocol**: Implement custom lightweight protocol

## Decision
**Priority downgraded**: Focus on fixing existing functionality affected by memory model changes and other recent modifications.

Socket API implementation will be revisited after stabilizing the core VM functionality.

## Related Issues
- Memory model changes (Medium → Large)
- Heap size adjustments
- Stack size modifications
- Build system updates

---
Made with Bob