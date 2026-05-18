# Socket Configuration Guide

## Overview

This guide explains how to configure the dosjava socket subsystem for optimal performance in 16-bit PC-DOS environments.

**Target**: 16-bit PC-DOS (640KB RAM)  
**Library**: mTCP TCP/IP stack  
**Phase**: 4.1

## Table of Contents

1. [Quick Configuration](#quick-configuration)
2. [Configuration Files](#configuration-files)
3. [Network Settings](#network-settings)
4. [Socket Settings](#socket-settings)
5. [Memory Tuning](#memory-tuning)
6. [Performance Optimization](#performance-optimization)
7. [Common Configurations](#common-configurations)

---

## Quick Configuration

### Minimal Setup (2 Sockets)

**For simple client applications**:

1. **socket.h**:
```c
#define MAX_SOCKETS 2
```

2. **socket.cpp**:
```cpp
Utils::initStack(2, TCP_SOCKET_RING_SIZE, NULL, NULL);
```

3. **sample.cfg**:
```c
#define TCP_MAX_SOCKETS (2)
```

**Memory Usage**: ~52 KB total

### Standard Setup (4 Sockets)

**For typical applications** (current default):

1. **socket.h**:
```c
#define MAX_SOCKETS 4
```

2. **socket.cpp**:
```cpp
Utils::initStack(4, TCP_SOCKET_RING_SIZE, NULL, NULL);
```

3. **sample.cfg**:
```c
#define TCP_MAX_SOCKETS (4)
```

**Memory Usage**: ~64 KB total

### Maximum Setup (8 Sockets)

**For server applications or multiple connections**:

1. **socket.h**:
```c
#define MAX_SOCKETS 8
```

2. **socket.cpp**:
```cpp
Utils::initStack(8, TCP_SOCKET_RING_SIZE, NULL, NULL);
```

3. **sample.cfg**:
```c
#define TCP_MAX_SOCKETS (8)
```

**Memory Usage**: ~88 KB total

---

## Configuration Files

### 1. socket.h

**Location**: `dosjava/src/network/socket.h`

**Key Settings**:

```c
/* Maximum number of concurrent sockets */
#define MAX_SOCKETS 4

/* Socket types */
#define SOCKET_TYPE_TCP 1
#define SOCKET_TYPE_UDP 2

/* Socket states */
#define SOCKET_STATE_CLOSED     0
#define SOCKET_STATE_LISTENING  1
#define SOCKET_STATE_CONNECTING 2
#define SOCKET_STATE_CONNECTED  3
#define SOCKET_STATE_CLOSING    4
```

**When to Modify**:
- Change MAX_SOCKETS to adjust concurrent socket limit
- Must synchronize with socket.cpp and sample.cfg

### 2. socket.cpp

**Location**: `dosjava/src/network/socket.cpp`

**Key Settings**:

```cpp
/* Line 77-85: mTCP initialization */
if (Utils::initStack(4,                    /* MAX_SOCKETS - MUST MATCH socket.h */
                     TCP_SOCKET_RING_SIZE, /* Ring buffer size */
                     NULL,                 /* Hostname (NULL = use config) */
                     NULL) != 0) {         /* Domain (NULL = use config) */
    set_error("Failed to initialize mTCP stack");
    return SOCKET_ERR_INIT;
}
```

**When to Modify**:
- First parameter MUST match MAX_SOCKETS in socket.h
- TCP_SOCKET_RING_SIZE can be adjusted for performance

### 3. sample.cfg

**Location**: `dosjava/tests/network/sample.cfg`

**Full Configuration**:

```c
/* Packet driver settings */
#define PACKET_INT 0x60

/* Network interface */
#define MTU 1500
#define PACKETBUF_SIZE 2048

/* TCP/IP stack settings */
#define TCP_MAX_SOCKETS (4)        /* MUST MATCH socket.h MAX_SOCKETS */
#define TCP_SOCKET_RING_SIZE (4096)
#define PACKET_BUFFERS (10)
#define TCP_MAX_XMIT_BUFS (4)

/* Timeouts (milliseconds) */
#define TCP_CONNECT_TIMEOUT (60000)
#define TCP_CLOSE_TIMEOUT (10000)

/* IP address (0.0.0.0 = DHCP) */
IPADDR 0.0.0.0
NETMASK 0.0.0.0
GATEWAY 0.0.0.0
NAMESERVER 0.0.0.0
```

**When to Modify**:
- TCP_MAX_SOCKETS: Must match socket.h
- PACKET_BUFFERS: Increase for high throughput
- TCP_MAX_XMIT_BUFS: Increase for large sends
- Network settings: Set static IP if needed

### 4. MTCP.CFG Environment Variable

**Purpose**: Points to mTCP configuration file

**Setup**:

```batch
REM In AUTOEXEC.BAT or before running program
SET MTCP.CFG=C:\MTCP\MTCP.CFG
```

**DOSBox-X**:
```
[autoexec]
SET MTCP.CFG=C:\MTCP\MTCP.CFG
```

**Verification**:
```batch
ECHO %MTCP.CFG%
```

---

## Network Settings

### DHCP Configuration (Recommended)

**sample.cfg**:
```c
IPADDR 0.0.0.0
NETMASK 0.0.0.0
GATEWAY 0.0.0.0
NAMESERVER 0.0.0.0
```

**Advantages**:
- Automatic configuration
- Works in most environments
- No manual IP management

**Requirements**:
- DHCP server on network
- May take 5-10 seconds to acquire lease

### Static IP Configuration

**sample.cfg**:
```c
IPADDR 192.168.1.100
NETMASK 255.255.255.0
GATEWAY 192.168.1.1
NAMESERVER 8.8.8.8
```

**Advantages**:
- Faster startup
- Predictable addressing
- No DHCP dependency

**Requirements**:
- Know network configuration
- Avoid IP conflicts
- Update if network changes

### DNS Configuration

**Primary DNS**:
```c
NAMESERVER 8.8.8.8
```

**Multiple DNS Servers**:
```c
NAMESERVER 8.8.8.8
NAMESERVER 8.8.4.4
```

**No DNS** (IP addresses only):
```c
NAMESERVER 0.0.0.0
```

---

## Socket Settings

### TCP_MAX_SOCKETS

**Purpose**: Maximum concurrent TCP sockets

**Values**:
- Minimum: 1
- Typical: 2-4
- Maximum: 8-16 (memory permitting)

**Memory Impact**:
- Each socket: ~6 KB
- 2 sockets: ~52 KB total
- 4 sockets: ~64 KB total
- 8 sockets: ~88 KB total

**Recommendations**:
- Client apps: 2-4 sockets
- Server apps: 4-8 sockets
- Testing: 2 sockets (minimal)

### TCP_SOCKET_RING_SIZE

**Purpose**: Receive buffer size per socket

**Default**: 4096 bytes (4 KB)

**Values**:
- Minimum: 1024 (1 KB)
- Typical: 4096 (4 KB)
- Maximum: 8192 (8 KB)

**Trade-offs**:
- Larger: Better throughput, more memory
- Smaller: Less memory, may need more recv() calls

**Recommendations**:
- HTTP client: 4096
- Large transfers: 8192
- Memory constrained: 2048

### PACKET_BUFFERS

**Purpose**: Number of packet buffers for receive

**Default**: 10

**Values**:
- Minimum: 5
- Typical: 10
- Maximum: 20

**Memory Impact**:
- Each buffer: ~1.5 KB
- 10 buffers: ~15 KB

**Recommendations**:
- Low traffic: 5-10
- High traffic: 15-20
- Multiple sockets: 10-15

### TCP_MAX_XMIT_BUFS

**Purpose**: Number of transmit buffers

**Default**: 4

**Values**:
- Minimum: 2
- Typical: 4
- Maximum: 8

**Memory Impact**:
- Each buffer: ~1.5 KB
- 4 buffers: ~6 KB

**Recommendations**:
- Small sends: 2-4
- Large sends: 4-8
- Bulk transfer: 8

---

## Memory Tuning

### Memory Budget Analysis

**Available Memory**: 640 KB (DOS)

**System Usage**:
- DOS: ~100 KB
- Program code: ~50-100 KB
- Stack: ~10 KB
- **Available**: ~430-480 KB

**Socket Subsystem**:
- mTCP init: ~24 KB
- Per socket: ~6 KB
- Packet buffers: ~15 KB (10 buffers)
- Transmit buffers: ~6 KB (4 buffers)
- **Total (4 sockets)**: ~64 KB

**Remaining for Application**: ~366-416 KB

### Optimization Strategies

#### Strategy 1: Minimize Sockets

**Goal**: Reduce memory usage

**Changes**:
```c
MAX_SOCKETS = 2
TCP_SOCKET_RING_SIZE = 2048
PACKET_BUFFERS = 5
```

**Savings**: ~20 KB

**Use Case**: Simple client, memory-constrained

#### Strategy 2: Maximize Throughput

**Goal**: Increase performance

**Changes**:
```c
TCP_SOCKET_RING_SIZE = 8192
PACKET_BUFFERS = 20
TCP_MAX_XMIT_BUFS = 8
```

**Cost**: ~30 KB additional

**Use Case**: File transfer, high bandwidth

#### Strategy 3: Multiple Connections

**Goal**: Support many concurrent sockets

**Changes**:
```c
MAX_SOCKETS = 8
TCP_SOCKET_RING_SIZE = 2048
PACKET_BUFFERS = 15
```

**Cost**: ~40 KB additional

**Use Case**: Server, proxy, multiple clients

### Memory Monitoring

**Check Available Memory**:

Use test_memory_profile.c:

```c
/* Before init */
unsigned long before = get_free_memory();

/* After init */
socket_init();
unsigned long after = get_free_memory();

printf("Used: %lu bytes\n", before - after);
```

**Expected Usage** (4 sockets):
- mTCP init: 24,576 bytes
- 4 sockets: 24,576 bytes
- **Total**: ~49,152 bytes

---

## Performance Optimization

### Latency Optimization

**Goal**: Minimize response time

**Settings**:
```c
TCP_SOCKET_RING_SIZE = 2048  /* Smaller buffer */
PACKET_BUFFERS = 5           /* Fewer buffers */
TCP_MAX_XMIT_BUFS = 2        /* Minimal transmit */
```

**Benefits**:
- Faster packet processing
- Lower memory overhead
- Quick response

**Trade-offs**:
- Lower throughput
- May need more recv() calls

### Throughput Optimization

**Goal**: Maximize data transfer rate

**Settings**:
```c
TCP_SOCKET_RING_SIZE = 8192  /* Larger buffer */
PACKET_BUFFERS = 20          /* More buffers */
TCP_MAX_XMIT_BUFS = 8        /* More transmit */
```

**Benefits**:
- Higher bandwidth
- Fewer recv() calls
- Better for large transfers

**Trade-offs**:
- More memory usage
- Slightly higher latency

### Balanced Configuration

**Goal**: Good all-around performance

**Settings** (current default):
```c
MAX_SOCKETS = 4
TCP_SOCKET_RING_SIZE = 4096
PACKET_BUFFERS = 10
TCP_MAX_XMIT_BUFS = 4
```

**Benefits**:
- Reasonable memory usage
- Good throughput
- Supports multiple connections

---

## Common Configurations

### Configuration 1: Simple HTTP Client

**Use Case**: Fetch web pages, REST API calls

**Settings**:
```c
/* socket.h */
#define MAX_SOCKETS 2

/* sample.cfg */
#define TCP_MAX_SOCKETS (2)
#define TCP_SOCKET_RING_SIZE (4096)
#define PACKET_BUFFERS (10)
#define TCP_MAX_XMIT_BUFS (4)
```

**Memory**: ~52 KB  
**Sockets**: 2 concurrent connections  
**Performance**: Good for typical HTTP

### Configuration 2: File Transfer Client

**Use Case**: FTP, large downloads

**Settings**:
```c
/* socket.h */
#define MAX_SOCKETS 2

/* sample.cfg */
#define TCP_MAX_SOCKETS (2)
#define TCP_SOCKET_RING_SIZE (8192)
#define PACKET_BUFFERS (20)
#define TCP_MAX_XMIT_BUFS (8)
```

**Memory**: ~70 KB  
**Sockets**: 2 concurrent connections  
**Performance**: Optimized for throughput

### Configuration 3: Multi-Connection Client

**Use Case**: Parallel downloads, connection pooling

**Settings**:
```c
/* socket.h */
#define MAX_SOCKETS 8

/* sample.cfg */
#define TCP_MAX_SOCKETS (8)
#define TCP_SOCKET_RING_SIZE (2048)
#define PACKET_BUFFERS (15)
#define TCP_MAX_XMIT_BUFS (4)
```

**Memory**: ~88 KB  
**Sockets**: 8 concurrent connections  
**Performance**: Many connections, moderate throughput

### Configuration 4: Memory-Constrained

**Use Case**: Limited memory, simple operations

**Settings**:
```c
/* socket.h */
#define MAX_SOCKETS 1

/* sample.cfg */
#define TCP_MAX_SOCKETS (1)
#define TCP_SOCKET_RING_SIZE (1024)
#define PACKET_BUFFERS (5)
#define TCP_MAX_XMIT_BUFS (2)
```

**Memory**: ~40 KB  
**Sockets**: 1 connection  
**Performance**: Minimal, but functional

### Configuration 5: Server Application

**Use Case**: Accept multiple client connections

**Settings**:
```c
/* socket.h */
#define MAX_SOCKETS 8

/* sample.cfg */
#define TCP_MAX_SOCKETS (8)
#define TCP_SOCKET_RING_SIZE (4096)
#define PACKET_BUFFERS (20)
#define TCP_MAX_XMIT_BUFS (8)
```

**Memory**: ~110 KB  
**Sockets**: 1 listening + 7 clients  
**Performance**: Good for small server

---

## Configuration Checklist

### Before Changing Configuration

- [ ] Determine memory budget
- [ ] Count required concurrent sockets
- [ ] Estimate throughput needs
- [ ] Check available DOS memory

### When Changing MAX_SOCKETS

- [ ] Update socket.h: `#define MAX_SOCKETS X`
- [ ] Update socket.cpp: `Utils::initStack(X, ...)`
- [ ] Update sample.cfg: `#define TCP_MAX_SOCKETS (X)`
- [ ] Verify all three match
- [ ] Rebuild: `wmake clean && wmake all`
- [ ] Test with new configuration

### After Configuration Change

- [ ] Test socket creation (all sockets)
- [ ] Test concurrent connections
- [ ] Monitor memory usage
- [ ] Verify performance
- [ ] Document configuration

---

## Troubleshooting Configuration

### Problem: "Failed to initialize mTCP stack"

**Check**:
1. MTCP.CFG environment variable set?
2. sample.cfg file exists and readable?
3. TCP_MAX_SOCKETS matches socket.h?
4. Packet driver loaded?

### Problem: "Failed to create socket" (after N sockets)

**Check**:
1. MAX_SOCKETS in socket.h = N?
2. TCP_MAX_SOCKETS in sample.cfg = N?
3. Utils::initStack() first parameter = N?
4. All three synchronized?

### Problem: Poor performance

**Check**:
1. TCP_SOCKET_RING_SIZE too small?
2. PACKET_BUFFERS too few?
3. Network congestion?
4. Multiple recv() calls needed?

**Try**:
- Increase TCP_SOCKET_RING_SIZE to 8192
- Increase PACKET_BUFFERS to 20
- Increase TCP_MAX_XMIT_BUFS to 8

### Problem: Out of memory

**Check**:
1. Too many sockets?
2. Buffers too large?
3. Other memory usage?

**Try**:
- Reduce MAX_SOCKETS
- Reduce TCP_SOCKET_RING_SIZE to 2048
- Reduce PACKET_BUFFERS to 5

---

## See Also

- [PHASE4_SOCKET_API.md](PHASE4_SOCKET_API.md) - API reference
- [PHASE4_MEMORY_OPTIMIZATION.md](PHASE4_MEMORY_OPTIMIZATION.md) - Memory details
- [sample.cfg](tests/network/sample.cfg) - Configuration file

---

**Document Version**: 1.0  
**Last Updated**: 2026-05-18  
**Phase**: 4.1 Day 13-14