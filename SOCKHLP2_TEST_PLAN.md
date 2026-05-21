# sockhlp2.exe Test Plan

## Overview
sockhlp2.exe is a simplified socket helper based directly on doscurl.cpp's connection logic.
This version aims to identify why the original sockhelp.exe fails to connect while doscurl.exe succeeds.

## Files Created
- `tools/sockhelp_v2.c` - Source code (doscurl-based implementation)
- `build_sockhelp_v2.bat` - Build script
- `build/bin/sockhlp2.exe` - Executable (8.3 filename)
- `build/bin/tsockh2.bat` - Test script (8.3 filename)

## Key Differences from Original sockhelp.exe
1. **Direct code copy**: Uses doscurl's connectToServer() logic exactly
2. **Same configuration**: Uses doscurl.cfg (not sample.cfg)
3. **Same memory model**: Large model (-ml)
4. **Same mTCP objects**: Links with *_l.obj files
5. **Minimal modifications**: Only changed for command-line interface

## Test Procedure

### Step 1: Verify doscurl.exe works
```batch
cd C:\DOSJAVA\BUILD\BIN
doscurl.exe http://192.168.0.2:8080/
```
Expected: Successfully connects and retrieves HTTP response

### Step 2: Test sockhlp2.exe
```batch
cd C:\DOSJAVA\BUILD\BIN
tsockh2.bat
```
Or manually:
```batch
del SOCK.IN
del SOCK.OUT
sockhlp2.exe connect 192.168.0.2 8080
type SOCK.OUT
```

### Step 3: Compare Results

#### If sockhlp2.exe succeeds:
- Connection established successfully
- SOCK.OUT contains: `OK:Connected`
- **Conclusion**: The issue was in the original sockhelp.c implementation
- **Next step**: Identify specific differences between sockhelp.c and sockhelp_v2.c

#### If sockhlp2.exe fails:
- Connection times out
- SOCK.OUT contains: `ERROR:Connection timeout`
- **Conclusion**: The issue is NOT in the connection logic itself
- **Next step**: Investigate environmental differences (command-line parsing, file I/O, etc.)

## Debug Output
sockhlp2.exe includes extensive debug output:
- mTCP initialization status
- DNS resolution results
- Connection attempt details
- Timer tick values
- Socket state information

All debug output goes to stdout (not stderr) for DOSBox-X compatibility.

## Expected Behavior

### Success Case
```
sockhlp2.exe connect 192.168.0.2 8080

Debug: Command: connect
Debug: Host: 192.168.0.2
Debug: Port: 8080
Debug: Initializing mTCP...
mTCP initialization successful
Debug: Connecting to 192.168.0.2:8080...
Debug: Local port: 12345
Debug: Starting connection...
Debug: Connection loop started, timeout=1092000 ticks
Debug: Tick 0: Waiting for connection...
Debug: Tick 18: Waiting for connection...
...
Debug: Connection established!
Connection successful
```

SOCK.OUT:
```
OK:Connected
```

### Failure Case
```
sockhlp2.exe connect 192.168.0.2 8080

Debug: Command: connect
Debug: Host: 192.168.0.2
Debug: Port: 8080
Debug: Initializing mTCP...
mTCP initialization successful
Debug: Connecting to 192.168.0.2:8080...
Debug: Local port: 12345
Debug: Starting connection...
Debug: Connection loop started, timeout=1092000 ticks
Debug: Tick 0: Waiting for connection...
Debug: Tick 18: Waiting for connection...
...
Debug: Tick 1091: Waiting for connection...
Debug: Connection timeout after 1091 ticks
Connection timeout
```

SOCK.OUT:
```
ERROR:Connection timeout
```

## Comparison with doscurl.exe

### Similarities
- Same mTCP configuration (doscurl.cfg)
- Same memory model (Large)
- Same connection logic (connectToServer)
- Same packet processing loop
- Same timeout calculation
- Same Ctrl-Break handler

### Differences
- Command-line interface (HTTP URL vs host:port)
- File I/O (SOCK.OUT vs stdout)
- HTTP protocol handling (none in sockhlp2)
- Error reporting format

## Next Steps Based on Results

### If sockhlp2.exe works:
1. Compare sockhelp.c and sockhelp_v2.c line by line
2. Identify the specific bug in sockhelp.c
3. Fix sockhelp.c
4. Rebuild and test
5. Remove debug output from both versions

### If sockhlp2.exe fails:
1. Test with even simpler program (minimal mTCP test)
2. Check if issue is related to:
   - Command-line argument parsing
   - File I/O operations
   - Memory allocation
   - Stack size
   - Startup code differences
3. Consider using doscurl.exe directly with wrapper script

## Build Commands

### Rebuild sockhlp2.exe
```batch
cd C:\Users\dotns\src\dosjava
build_sockhelp_v2.bat
```

### Rebuild all
```batch
cd C:\Users\dotns\src\dosjava
build_all.bat
```

## Notes
- sockhlp2.exe requires WATTCP.CFG in the same directory
- Packet driver must be loaded before running
- mTCP buffers are configured in doscurl.cfg (compile-time)
- Network settings are in WATTCP.CFG (runtime)

@REM Made with Bob