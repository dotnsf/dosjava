# Network Tests - Wattcp Integration

## Phase 4.1 Day 1-2: Wattcp Initialization Test

### Test Program: twatt.exe

Basic Wattcp initialization test to verify library integration.

### Prerequisites

1. **DOSBox-X Configuration**
   - NE2000 network card emulation enabled
   - Packet driver support

2. **wattcp.cfg Configuration File**
   Wattcp searches for `wattcp.cfg` in the current directory.
   Sample content:
   ```
   my_ip = 192.168.1.100
   netmask = 255.255.255.0
   gateway = 192.168.1.1
   nameserver = 8.8.8.8
   ```

3. **Network Setup in DOSBox-X**
   ```
   # Load NE2000 packet driver
   ne2000 0x60 3 0x300
   ```

### Test Execution

**Method 1: Using the batch file (Recommended)**
1. Copy these files to the same directory in DOSBox-X:
   - `twatt.exe` (from `build/bin/`)
   - `runtwatt.bat`
   - `WATTCP.CFG` → rename to `wattcp.cfg` (lowercase, 8.3 format)
2. Load packet driver: `ne2000 0x60 3 0x300`
3. Run: `runtwatt.bat`

**Method 2: Manual execution**
1. Copy these files to the same directory in DOSBox-X:
   - `twatt.exe` (from `build/bin/`)
   - `WATTCP.CFG` → rename to `wattcp.cfg` (lowercase)
2. Load packet driver: `ne2000 0x60 3 0x300`
3. Run: `twatt.exe`

**Important:** Wattcp searches for `wattcp.cfg` in the current directory, so all files must be in the same directory.

### Expected Output

```
=== Wattcp Initialization Test ===

Test 1: Wattcp headers
  PASS: tcp.h included successfully

Test 2: Configure Wattcp
  INFO: Setting WATTCP.CFG=D:\WATTCP.CFG

Test 3: Initialize Wattcp library
  Calling sock_init()...
  PASS: sock_init() succeeded (returned 1)

Test 4: Wattcp library loaded
  PASS: Wattcp is initialized and ready

=== All Tests Passed ===

Wattcp is ready for socket programming!

Next steps:
  - Test socket creation
  - Test TCP connection
  - Implement socket wrapper API
```

### Troubleshooting

**Issue: "wattcp.cfg not found" or "All attempt to get IP address failed"**
- **Cause**: wattcp.cfg file not in current directory
- **Solution**:
  1. Copy WATTCP.CFG to the same directory as twatt.exe
  2. Rename it to `wattcp.cfg` (lowercase, 8.3 format)
  3. Ensure you're running twatt.exe from that directory

**If sock_init() returns 0:**
- Check packet driver is loaded: `ne2000 0x60 3 0x300`
- Verify wattcp.cfg exists in current directory (lowercase filename)
- Check DOSBox-X network configuration
- Verify wattcp.cfg has correct network settings

**If program crashes:**
- Verify wattcpws.lib is correct version (small memory model)
- Check for memory conflicts

### Next Steps

After successful initialization test:
1. Implement C-level socket wrapper functions (Day 3-5)
2. Create socket API: socket_create, socket_bind, socket_connect, etc.
3. Add comprehensive socket tests

### Files

- `test_wattcp_init.c` - Source code
- `twatt.exe` - Compiled test program (in build/bin/)
- `runtwatt.bat` - Test runner batch file (8.3 format, sets environment and runs test)
- `WATTCP.CFG` - Sample Wattcp configuration file
- `build_test_wattcp.bat` - Build script (deprecated, use wmake)
- `README.md` - This file

### Build Command

```
cd dosjava
wmake test_wattcp
```

This compiles the test and links with Wattcp library.

### Important Notes

- Wattcp searches for `wattcp.cfg` (lowercase) in the current directory
- The configuration file must be in the same directory as the executable
- No environment variables are needed when using current directory approach
- File must be named `wattcp.cfg` (8.3 format, lowercase)
- For production use, include wattcp.cfg with your application