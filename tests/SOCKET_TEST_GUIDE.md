# Socket Test Guide

Phase 4.2 Day 22-24: Java Socket Testing

## Overview

This guide describes how to test the Socket functionality in DOSBox-X environment.

## Prerequisites

1. **Network Configuration**
   - mTCP configuration file: `tests\network\sample.cfg`
   - Set `MTCPCFG` environment variable in DOSBox-X:
     ```
     SET MTCPCFG=C:\DOSJAVA\TESTS\NETWORK\SAMPLE.CFG
     ```

2. **Packet Driver**
   - Load packet driver in DOSBox-X before running tests
   - Example: `ne2000.com 0x60 10 0x300`

3. **Test Server**
   - For connection tests, ensure a test HTTP server is running
   - Default test target: `127.0.0.1:80` (localhost)

## Test Files

### 1. sockinit.jav - Socket Initialization Test
**Purpose**: Tests Socket.init() native method

**Expected Output**:
```
Socket Init Test
Socket initialized
```

**Run**:
```
cd \dosjava\tests
runsock sockinit
```

### 2. sockconn.jav - Socket Connection Test
**Purpose**: Tests Socket.new() and Socket.isConnected() native methods

**Expected Output** (success):
```
Socket Connection Test
Creating socket...
Socket created
Socket connected
Socket closed
```

**Expected Output** (failure):
```
Socket Connection Test
Creating socket...
Failed to create socket
```

**Run**:
```
cd \dosjava\tests
runsock sockconn
```

### 3. socksend.jav - Socket Send/Recv Test
**Purpose**: Tests Socket.send() and Socket.recv() native methods

**Expected Output** (success):
```
Socket Send/Recv Test
Creating socket...
Socket created
Socket connected
Sent bytes:
18
Received:
HTTP/1.0 200 OK
...
Socket closed
```

**Run**:
```
cd \dosjava\tests
runsock socksend
```

## Test Procedure

### Step 1: Build All Components
In Windows 11 environment:
```
cd C:\Users\dotns\src\dosjava
.\build_all.bat
```

### Step 2: Copy Files to DOSBox-X
Copy the following to DOSBox-X C:\DOSJAVA:
- `build\bin\djc.exe`
- `build\bin\djvm.exe`
- `tests\sockinit.jav`
- `tests\sockconn.jav`
- `tests\socksend.jav`
- `tests\runsock.bat`
- `tests\network\sample.cfg`

### Step 3: Configure Network in DOSBox-X
```
SET MTCPCFG=C:\DOSJAVA\TESTS\NETWORK\SAMPLE.CFG
```

### Step 4: Load Packet Driver
```
ne2000.com 0x60 10 0x300
```

### Step 5: Run Tests
```
cd \dosjava\tests
runsock sockinit
runsock sockconn
runsock socksend
```

## Troubleshooting

### Error: "Socket initialization failed"
- Check MTCPCFG environment variable
- Verify sample.cfg file exists and is readable
- Ensure packet driver is loaded

### Error: "Failed to create socket"
- Check network connectivity
- Verify target host:port is reachable
- Check mTCP configuration (IP address, gateway, etc.)

### Error: "Socket not connected"
- Target server may not be running
- Firewall may be blocking connection
- Check IP address and port number

### Error: "No response"
- Connection established but no data received
- Server may have closed connection
- Check recv buffer size (default: 256 bytes)

## Test Results Documentation

Record test results in the following format:

```
Test: sockinit.jav
Date: YYYY-MM-DD
Environment: DOSBox-X version X.XX.X
Result: PASS/FAIL
Output:
[paste output here]
Notes:
[any observations]
```

## Next Steps

After successful testing:
1. Document any issues found
2. Fix bugs if necessary
3. Proceed to Phase 4.2 Day 25-26: Integration Testing