# DOSBox-X Setup Guide for Socket Testing

## Overview
This guide explains how to set up DOSBox-X environment for testing dosjava socket functionality.

## Prerequisites
- DOSBox-X installed and configured
- NE2000 network emulation enabled in DOSBox-X
- dosjava files copied to DOSBox-X drive

## Step-by-Step Setup

### 1. Copy Files to DOSBox-X

Copy the following files from Windows to DOSBox-X drive (e.g., D:\ drive):

```
From: C:\Users\dotns\src\dosjava\build\bin\
To: D:\DOSJAVA\

Files:
- djc.exe
- djvm.exe
```

```
From: C:\Users\dotns\src\dosjava\tests\network\
To: D:\DOSJAVA\

Files:
- MTCP.CFG
```

```
From: C:\Users\dotns\src\dosjava\tests\
To: D:\DOSJAVA\

Files:
- sockinit.jav
- sockconn.jav
- socksend.jav
```

### 2. Load Packet Driver

In DOSBox-X, load the NE2000 packet driver:

```
D:\> ne2000 0x60 3 0x300
```

Expected output:
```
Packet driver for NE2000, version X.XX
Packet driver loaded at interrupt 0x60
```

**Note**: The packet driver is usually located in DOSBox-X's system directory or can be downloaded separately.

### 3. Set Environment Variable

Set the MTCPCFG environment variable to point to the configuration file:

```
D:\> SET MTCPCFG=D:\DOSJAVA\MTCP.CFG
```

Verify it's set:
```
D:\> ECHO %MTCPCFG%
D:\DOSJAVA\MTCP.CFG
```

### 4. Verify Configuration File

Check that the configuration file exists and is readable:

```
D:\> TYPE D:\DOSJAVA\MTCP.CFG
```

Expected output:
```
PACKETINT 0x60
IPADDR 192.168.0.123
NETMASK 255.255.255.0
GATEWAY 192.168.0.1
NAMESERVER 8.8.8.8
```

### 5. Test Socket Initialization

Compile and run the socket initialization test:

```
D:\> CD DOSJAVA
D:\DOSJAVA> djc sockinit.jav
D:\DOSJAVA> djvm -d sockinit.djc
```

Expected output:
```
[DEBUG] About to initialize memory manager...
[DEBUG] Initializing memory manager with heap_size=16384
[DEBUG] malloc succeeded, heap_start=XXXX
[DEBUG] Memory manager initialized successfully
Executing method: main
  Code offset: 0
  Code length: 19
  Max stack:   1
  Max locals:  0

=== Debug Mode ===
[PC=0000] Opcode=0x01
DEBUG VM: Executing opcode 0x01 at PC offset 0
[PC=0003] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 3
[DEBUG] Method: println, Descriptor: (Ljava/lang/String;)V, Expected args: 1, Actual args: 1
Socket Init Test
[PC=0007] Opcode=0x40
DEBUG VM: Executing opcode 0x40 at PC offset 7
[DEBUG] Method: init, Descriptor: ()V, Expected args: 0, Actual args: 0
Socket initialized
[PC=0011] Opcode=0xb1
DEBUG VM: Executing opcode 0xb1 at PC offset 11
```

## Configuration File Details

### MTCP.CFG Format

```
PACKETINT 0x60          # Packet driver interrupt (must match loaded driver)
IPADDR 192.168.0.123    # IP address for this machine
NETMASK 255.255.255.0   # Network mask
GATEWAY 192.168.0.1     # Default gateway
NAMESERVER 8.8.8.8      # DNS server (optional, for hostname resolution)
```

### Adjusting Network Settings

If your DOSBox-X network uses different settings, modify MTCP.CFG:

1. **PACKETINT**: Must match the interrupt used when loading packet driver
2. **IPADDR**: Choose an unused IP in your network range
3. **NETMASK**: Typically 255.255.255.0 for home networks
4. **GATEWAY**: Your router's IP address
5. **NAMESERVER**: Your DNS server or public DNS (8.8.8.8, 1.1.1.1)

## Troubleshooting

### Error: "Failed to initialize socket subsystem"

**Cause**: MTCPCFG environment variable not set or configuration file not found

**Solution**:
```
D:\> SET MTCPCFG=D:\DOSJAVA\MTCP.CFG
D:\> TYPE %MTCPCFG%
```

### Error: "Failed to initialize mTCP stack"

**Possible causes**:
1. Packet driver not loaded
2. Wrong packet interrupt in MTCP.CFG
3. Network configuration mismatch

**Solutions**:

1. **Check packet driver**:
   ```
   D:\> ne2000 0x60 3 0x300
   ```

2. **Verify PACKETINT matches**:
   - Packet driver loaded at 0x60
   - MTCP.CFG has `PACKETINT 0x60`

3. **Check DOSBox-X network settings**:
   - In DOSBox-X config, ensure NE2000 emulation is enabled
   - Verify network backend is configured (slirp, pcap, etc.)

### Error: "Packet driver not found"

**Cause**: NE2000 packet driver not available

**Solution**:
1. Download packet driver (ne2000.com or ne2000.exe)
2. Copy to DOSBox-X drive
3. Load before running tests

### Network Not Working

**Check DOSBox-X Configuration**:

In `dosbox-x.conf`:
```ini
[ne2000]
ne2000=true
nicbase=300
nicirq=3
```

## Batch File for Easy Setup

Create `setup.bat` in D:\DOSJAVA\:

```batch
@ECHO OFF
ECHO DOSBox-X Socket Test Setup
ECHO ===========================
ECHO.

ECHO Step 1: Loading packet driver...
ne2000 0x60 3 0x300
IF ERRORLEVEL 1 GOTO ERROR_DRIVER
ECHO OK: Packet driver loaded
ECHO.

ECHO Step 2: Setting environment variable...
SET MTCPCFG=D:\DOSJAVA\MTCP.CFG
ECHO OK: MTCPCFG=%MTCPCFG%
ECHO.

ECHO Step 3: Verifying configuration file...
IF NOT EXIST %MTCPCFG% GOTO ERROR_CONFIG
TYPE %MTCPCFG%
ECHO.
ECHO OK: Configuration file found
ECHO.

ECHO Setup complete! You can now run socket tests.
ECHO.
ECHO Example:
ECHO   djc sockinit.jav
ECHO   djvm -d sockinit.djc
GOTO END

:ERROR_DRIVER
ECHO ERROR: Failed to load packet driver
ECHO Make sure ne2000.com is available
GOTO END

:ERROR_CONFIG
ECHO ERROR: Configuration file not found: %MTCPCFG%
ECHO Make sure MTCP.CFG exists in D:\DOSJAVA\
GOTO END

:END
```

Run setup:
```
D:\DOSJAVA> setup.bat
```

## Quick Reference

### Essential Commands

```batch
REM Load packet driver
ne2000 0x60 3 0x300

REM Set configuration
SET MTCPCFG=D:\DOSJAVA\MTCP.CFG

REM Compile Java program
djc program.jav

REM Run with debug output
djvm -d program.djc

REM Run without debug output
djvm program.djc
```

### File Locations

```
D:\DOSJAVA\
├── djc.exe           # Compiler
├── djvm.exe          # Virtual Machine
├── MTCP.CFG          # mTCP configuration
├── sockinit.jav      # Socket init test
├── sockconn.jav      # Socket connection test
├── socksend.jav      # Socket send/recv test
└── setup.bat         # Setup script (optional)
```

## Next Steps

After successful setup:

1. **Test Socket Initialization**: `djvm -d sockinit.djc`
2. **Test Socket Connection**: `djvm -d sockconn.djc`
3. **Test Socket Communication**: `djvm -d socksend.djc`

## Additional Resources

- **mTCP Documentation**: http://www.brutman.com/mTCP/
- **DOSBox-X Wiki**: https://dosbox-x.com/wiki/
- **Packet Driver Collection**: http://www.crynwr.com/drivers/

## Notes

- The packet driver must be loaded **before** running any socket programs
- The MTCPCFG environment variable must be set **in the same session**
- Configuration changes require restarting the program (not DOSBox-X)
- Network settings in MTCP.CFG should match your DOSBox-X network configuration