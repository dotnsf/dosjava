# Phase 4.1: Memory Optimization Report

## Overview

This document describes the memory optimization strategies for the dosjava socket implementation, targeting 16-bit PC-DOS with 640KB RAM constraint.

## Current Memory Usage

### Socket Implementation

**Configuration** (from `sample.cfg`):
- `TCP_MAX_SOCKETS`: 2
- `PACKET_BUFFERS`: 10
- `TCP_MAX_XMIT_BUFS`: 4
- `TCP_RECV_BUFFER`: 4096 bytes per socket

**Socket Structure** (from `socket.cpp`):
```c
typedef struct {
    int type;              /* 2 bytes */
    int state;             /* 2 bytes */
    TcpSocket* tcp_socket; /* 2 bytes (16-bit pointer) */
    void* udp_socket;      /* 2 bytes (16-bit pointer) */
    int in_use;            /* 2 bytes */
} Socket;                  /* Total: 10 bytes per socket */
```

**Global Data**:
- Socket table: `Socket g_sockets[2]` = 20 bytes
- Error message buffer: `char g_error_msg[256]` = 256 bytes
- Total static data: ~276 bytes

### mTCP Stack Memory

**Per Socket**:
- TCP receive buffer: 4,096 bytes
- TCP socket ring buffer: Defined by `TCP_SOCKET_RING_SIZE` in GLOBAL.CFG
- Internal mTCP structures: ~1-2 KB

**Total per socket**: ~5-6 KB

**For 2 sockets**: ~10-12 KB

**Packet buffers** (10 buffers):
- Each buffer: ~1,500 bytes (Ethernet MTU)
- Total: ~15 KB

**Total mTCP overhead**: ~25-30 KB

### Program Size

**Executables**:
- `tsendrcv.exe`: 61,850 bytes (~60 KB)
- `tmemprof.exe`: 61,370 bytes (~60 KB)
- `tnetdiag.exe`: 62,762 bytes (~61 KB)

**Code + Data**: ~60-65 KB per program

### Total Memory Footprint

**Estimated breakdown**:
- Program code/data: 60-65 KB
- mTCP stack: 25-30 KB
- Socket buffers: 10-12 KB
- Packet buffers: 15 KB
- **Total**: ~110-122 KB

**Well within 640KB DOS limit** ✓

## Optimization Strategies

### 1. Buffer Size Optimization

**Current**: 4,096 bytes per socket
**Rationale**: 
- Matches doscurl implementation
- Good balance for HTTP traffic
- Allows receiving typical HTTP headers in one buffer

**Alternative options**:
- 2,048 bytes: Reduces memory by 50%, still adequate for most protocols
- 1,024 bytes: Minimal viable size, may require more recv() calls

**Recommendation**: Keep 4,096 bytes for Phase 4.1, consider 2,048 bytes if memory becomes constrained in Phase 4.2.

### 2. Socket Count Configuration

**Current**: `MAX_SOCKETS = 2` (in socket.h)
**Rationale**:
- Sufficient for basic client operations
- Matches sample.cfg configuration
- Low memory overhead

**Scalability**:
- Can increase to 4-8 sockets if needed
- Each additional socket: ~6 KB
- 8 sockets would use ~48 KB (still acceptable)

**Recommendation**: Keep 2 sockets for Phase 4.1, increase to 4 in Phase 4.2 for Java applications.

### 3. Packet Buffer Configuration

**Current**: `PACKET_BUFFERS = 10`
**Rationale**:
- Standard mTCP configuration
- Handles burst traffic well
- ~15 KB total

**Alternative**:
- Reduce to 5-6 buffers: Saves ~7-9 KB
- May impact performance under high load

**Recommendation**: Keep 10 buffers for reliable operation.

### 4. Code Size Optimization

**Current approach**:
- Compiler flags: `-os` (optimize for size)
- Small memory model: `-ms`
- No debug info in release builds

**Already optimized** ✓

### 5. Memory Reuse Patterns

**Socket pooling** (already implemented):
```c
static Socket g_sockets[MAX_SOCKETS];

socket_handle_t allocate_socket(void) {
    for (i = 0; i < MAX_SOCKETS; i++) {
        if (!g_sockets[i].in_use) {
            g_sockets[i].in_use = 1;
            return i;
        }
    }
    return INVALID_SOCKET;
}
```

**Benefits**:
- No dynamic allocation
- Predictable memory usage
- Fast allocation/deallocation

**Already optimal** ✓

## Memory Profiling Results

### Test Program: tmemprof.exe

**Purpose**: Measure memory usage at each stage

**Measurements**:
1. Baseline memory
2. After socket_init()
3. After creating MAX_SOCKETS sockets
4. After closing all sockets
5. After socket_shutdown()

**Expected results** (to be measured in DOSBox-X):
- Init overhead: ~25-30 KB (mTCP stack)
- Per socket: ~6 KB
- Cleanup: Should free all allocated memory

## Comparison with doscurl

**doscurl configuration**:
- TCP_RECV_BUFFER: 4,096 bytes (same)
- Uses malloc/free for large buffers
- Similar mTCP configuration

**dosjava advantages**:
- Static allocation (no heap fragmentation)
- Predictable memory usage
- Simpler memory management

## Memory Budget for Phase 4.2

**Available**: 640 KB total DOS memory

**Reserved**:
- DOS system: ~100 KB
- Program code: ~60-70 KB
- mTCP stack: ~30 KB
- Socket buffers: ~25 KB (4 sockets)
- **Subtotal**: ~215 KB

**Available for Java VM**: ~425 KB

**Sufficient for**:
- VM code: ~50 KB
- Java bytecode: ~50 KB
- Java heap: ~300 KB
- Stack: ~25 KB

**Conclusion**: Memory budget is adequate for Phase 4.2 ✓

## Recommendations

### For Phase 4.1 (Current)

1. ✅ Keep current configuration (optimal)
2. ✅ Use static allocation (no malloc/free)
3. ✅ Maintain 2 sockets
4. ✅ Keep 4KB receive buffers

### For Phase 4.2 (Java Integration)

1. Increase MAX_SOCKETS to 4
2. Consider reducing TCP_RECV_BUFFER to 2KB if memory tight
3. Profile actual Java application memory usage
4. Implement socket timeout/cleanup for long-lived connections

### Future Optimizations (if needed)

1. **Lazy buffer allocation**: Allocate receive buffers only when needed
2. **Shared buffers**: Use single large buffer for all sockets
3. **Configurable buffer sizes**: Allow runtime configuration
4. **Memory pooling**: Implement custom allocator for small objects

## Testing Checklist

- [x] Build tmemprof.exe
- [ ] Run tmemprof.exe in DOSBox-X
- [ ] Measure baseline memory
- [ ] Measure init overhead
- [ ] Measure per-socket overhead
- [ ] Verify cleanup frees memory
- [ ] Document actual measurements
- [ ] Compare with estimates

## Conclusion

The current socket implementation is well-optimized for 16-bit DOS:

- **Total overhead**: ~110-122 KB (estimated)
- **Well within 640KB limit**: ✓
- **Efficient memory usage**: ✓
- **No dynamic allocation**: ✓
- **Predictable behavior**: ✓

No immediate optimizations required. Memory budget is sufficient for Phase 4.2 Java integration.

## Next Steps

1. Run tmemprof.exe in DOSBox-X to get actual measurements
2. Update this document with real profiling data
3. Proceed to Phase 4.1 Day 13-14 (Documentation)
4. Plan Phase 4.2 with confirmed memory budget