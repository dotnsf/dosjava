# Network Tests - mTCP Integration

## Phase 4.1 Day 1-2: mTCP Initialization Test (Current)

### Test Program: tmtcp.exe

Basic mTCP initialization test to verify library integration with dosjava.

### Prerequisites

1. **DOSBox-X Configuration**
   - NE2000 network card emulation enabled
   - Packet driver support

2. **MTCP.CFG Configuration File**
   mTCP searches for configuration via MTCPCFG environment variable.
   Sample content:
   ```
   PACKETINT 0x60
   IPADDR 192.168.0.123
   NETMASK 255.255.255.0
   GATEWAY 192.168.0.1
   NAMESERVER 8.8.8.8
   ```

3. **Network Setup in DOSBox-X**
   ```
   # Load NE2000 packet driver
   ne2000 0x60 3 0x300
   ```

### Test Execution

**Method 1: Using the batch file (Recommended)**
1. Copy these files to the same directory in DOSBox-X:
   - `tmtcp.exe` (from `build/bin/`)
   - `runtmtcp.bat`
   - `MTCP.CFG`
2. Load packet driver: `ne2000 0x60 3 0x300`
3. Run: `runtmtcp.bat`

**Method 2: Manual execution**
1. Copy these files to DOSBox-X:
   - `tmtcp.exe` (from `build/bin/`)
   - `MTCP.CFG`
2. Load packet driver: `ne2000 0x60 3 0x300`
3. Set environment: `SET MTCPCFG=D:\MTCP.CFG`
4. Run: `tmtcp.exe`

### Expected Output

```
=== mTCP Initialization Test for dosjava ===

Test 1: mTCP headers
  PASS: tcp.h included successfully
  INFO: Using mTCP library (Wattcp-compatible API)

Test 2: Check for configuration files
  INFO: MTCPCFG=D:\MTCP.CFG
  FOUND: Configuration file at MTCPCFG location

Test 2b: Display configuration file contents
  --- Contents of D:\MTCP.CFG ---
   1: PACKETINT 0x60
   2: IPADDR 192.168.0.123
   3: NETMASK 255.255.255.0
   4: GATEWAY 192.168.0.1
   5: NAMESERVER 8.8.8.8
  --- End of file ---

Test 3: Check for packet driver
  Packet driver vector (INT 60h): xxxx:xxxx
  INFO: Packet driver vector is set
        Segment: xxxx, Offset: xxxx
        Driver appears to be loaded

Test 4: Initialize mTCP library
  Calling sock_init()...
  (mTCP will display diagnostic messages)
  ----------------------------------------
  [mTCP diagnostic messages]
  ----------------------------------------
  PASS: sock_init() succeeded
  INFO: _watt_do_exit flag is NOT set

Test 5: mTCP library loaded
  PASS: mTCP is initialized and ready
  INFO: dosjava can now use network sockets

=== All Tests Passed ===

mTCP is ready for socket programming!

Next steps:
  - Implement C-level socket wrapper functions
  - Test socket creation and connection
  - Integrate with dosjava VM

Configuration:
  - Library: mTCP (Wattcp-compatible API)
  - Memory model: Small
  - Max sockets: 4
  - Packet buffers: 10
```

### Troubleshooting

**Issue: "Packet driver vector is NULL"**
- **Cause**: Packet driver not loaded
- **Solution**: Load packet driver: `ne2000 0x60 3 0x300`

**Issue: "sock_init() failed" or "_watt_do_exit flag is set"**
- **Cause**: Configuration file missing or network setup incorrect
- **Solution**:
  1. Verify MTCP.CFG exists and MTCPCFG is set correctly
  2. Check packet driver is loaded
  3. Verify DOSBox-X NE2000 emulation is enabled
  4. Check network settings in MTCP.CFG

**If program crashes:**
- Verify MTCPWS.LIB is correct version (Small memory model)
- Check for memory conflicts
- Ensure dosjava.cfg is in the project root

### Build Command

```
cd dosjava
wmake test_mtcp
```

This compiles the test and links with mTCP library (Small model).

### Files

- `test_mtcp_init.c` - Source code
- `tmtcp.exe` - Compiled test program (in build/bin/)
- `runtmtcp.bat` - Test runner batch file
- `MTCP.CFG` - Sample mTCP configuration file
- `README.md` - This file

### Important Notes

- mTCP uses MTCPCFG environment variable to locate configuration
- Configuration file can be anywhere, just set MTCPCFG to point to it
- Packet driver must be loaded before running the test
- mTCP provides Wattcp-compatible API for easier migration
- dosjava uses Small memory model (MTCPWS.LIB)
- Configuration in dosjava.cfg: TCP_MAX_SOCKETS=4, PACKET_BUFFERS=10

### Next Steps

After successful initialization test:
1. Implement C-level socket wrapper functions (Day 3-5)
2. Create socket API: socket_connect, socket_send, socket_recv, etc.
3. Add comprehensive socket tests

---

## Wattcp Test (Deprecated)

The original Wattcp test (twatt.exe) is deprecated. Use mTCP (tmtcp.exe) instead.

### Why mTCP?

1. **Proven**: doscurl project successfully uses mTCP
2. **Compatible**: Wattcp-compatible API
3. **Stable**: Works reliably in DOSBox-X
4. **Documented**: Better documentation and examples

### Migration Notes

- mTCP provides same API as Wattcp (sock_init, tcp_open, etc.)
- Configuration uses MTCPCFG instead of WATTCP.CFG
- Library: MTCPWS.LIB (Small) instead of wattcpws.lib
- Include path: C:\mTCP\src\TCPINC instead of C:\WATCOM\h\wattcp