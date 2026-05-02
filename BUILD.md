# DOS Java Build Instructions

## Prerequisites

### Required Tools

1. **Open Watcom v2 C Compiler**
   - Download from: https://github.com/open-watcom/open-watcom-v2/releases
   - Install and add to PATH
   - Verify installation: `wcc -?`

2. **Make Utility**
   - Included with Open Watcom (wmake)
   - Or use GNU Make if available

3. **DOS Environment**
   - Real DOS system, or
   - DOSBox emulator, or
   - QEMU with DOS

### Optional Tools

- **Java Development Kit (JDK)** - For compiling test Java programs
- **Git** - For version control
- **Text Editor** - Any editor that supports C

## Build Steps

### 1. Set Up Environment

On Windows:
```batch
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINW;%PATH%
set INCLUDE=%WATCOM%\H
```

On Linux (with Open Watcom cross-compiler):
```bash
export WATCOM=/opt/watcom
export PATH=$WATCOM/binl:$PATH
export INCLUDE=$WATCOM/h
```

### 2. Create Build Directories

```batch
cd dosjava
wmake dirs
```

Or manually:
```batch
mkdir build
mkdir build\obj
mkdir build\bin
```

### 3. Build Test Program

```batch
wmake test_memory
```

This will:
1. Compile all source files to .obj files
2. Link them into test_mem.exe
3. Place executable in build/bin/

### 4. Run Test Program

On DOS or DOSBox:
```batch
cd build\bin
test_mem.exe
```

Expected output:
```
=== DOS Java Memory Manager Test ===

Memory manager initialized
Memory Statistics:
  Total heap:  30720 bytes
  Used:        8 bytes
  Free:        30712 bytes
  Allocations: 0
  Frees:       0

Test: Basic Allocation
  Allocated 100 bytes: OK
  Allocated 200 bytes: OK
  Allocated 50 bytes: OK
  ...
```

## Build Targets

### Available Targets

```batch
wmake all          # Build all targets (default)
wmake test_memory  # Build memory manager test
wmake clean        # Remove build files
wmake dirs         # Create build directories
wmake help         # Show help
```

## Compiler Flags

### Current Flags (for debugging)

- `-ms` - Small memory model (64KB code + 64KB data)
- `-0` - Generate 8086 instructions
- `-w4` - Warning level 4 (all warnings)
- `-zq` - Quiet mode (less verbose output)
- `-od` - Disable optimizations
- `-d2` - Full debugging information

### Release Flags (for production)

To build optimized release version, modify Makefile:

```makefile
CFLAGS = -ms -0 -w4 -zq -ox -s
```

Where:
- `-ox` - Maximum optimizations
- `-s` - Remove stack overflow checks

## Troubleshooting

### "wcc: command not found"

**Problem:** Open Watcom not in PATH

**Solution:**
```batch
set PATH=C:\WATCOM\BINW;%PATH%
```

### "Cannot open include file"

**Problem:** Include path not set

**Solution:**
```batch
set INCLUDE=C:\WATCOM\H
```

### "Out of memory" during compilation

**Problem:** Insufficient DOS memory

**Solution:**
- Close other programs
- Use DOS extender (DOS4GW)
- Reduce heap size in code

### Linker errors

**Problem:** Missing object files or libraries

**Solution:**
- Run `wmake clean` and rebuild
- Check that all source files compiled
- Verify object files exist in build/obj/

### "Segment too large"

**Problem:** Code or data exceeds 64KB

**Solution:**
- Use Compact or Large memory model
- Split code into multiple modules
- Reduce static data

## Memory Models

### Small Model (Current)
- Code: 64KB
- Data: 64KB
- Best for: Small programs

### Compact Model
- Code: 64KB
- Data: Multiple 64KB segments
- Change flag: `-mc`

### Large Model
- Code: Multiple 64KB segments
- Data: Multiple 64KB segments
- Change flag: `-ml`

## Cross-Compilation

### Building on Linux

1. Install Open Watcom for Linux
2. Set environment variables
3. Use same Makefile with `wmake`

### Building on Windows for DOS

1. Install Open Watcom for Windows
2. Use wmake or nmake
3. Test in DOSBox

## Testing in DOSBox

### DOSBox Configuration

Create dosbox.conf:
```ini
[autoexec]
mount c: C:\dosjava
c:
cd build\bin
```

### Run DOSBox

```batch
dosbox -conf dosbox.conf
```

Then in DOSBox:
```
test_mem.exe
```

## Continuous Integration

### Automated Build Script

Create build.bat:
```batch
@echo off
echo Building DOS Java...

wmake clean
wmake dirs
wmake all

if errorlevel 1 (
    echo Build FAILED
    exit /b 1
)

echo Build SUCCESS
exit /b 0
```

Run:
```batch
build.bat
```

## Next Steps

After successful build:

1. **Test the executable** - Run test_mem.exe
2. **Verify output** - Check memory statistics
3. **Build VM components** - Implement interpreter
4. **Create more tests** - Add integration tests
5. **Optimize** - Profile and improve performance

## Build Artifacts

After successful build, you'll have:

```
build/
├── obj/
│   ├── memory.obj
│   ├── object.obj
│   ├── string.obj
│   ├── system.obj
│   └── test_memory.obj
└── bin/
    ├── test_mem.exe
    └── test_mem.map (if debugging enabled)
```

## Size Estimates

Typical sizes for Small model:

- memory.obj: ~2KB
- object.obj: ~1KB
- string.obj: ~3KB
- system.obj: ~1KB
- test_mem.exe: ~15-20KB

## Performance Notes

### Compilation Time
- Single file: <1 second
- Full project: 5-10 seconds
- Clean build: 10-15 seconds

### Executable Size
- Debug build: 20-30KB
- Release build: 10-15KB
- With optimizations: 8-12KB

## Advanced Build Options

### Static Analysis

```batch
wcc -ms -w4 -we -za99 file.c
```

Where:
- `-we` - Treat warnings as errors
- `-za99` - C99 standard compliance

### Profiling Build

```batch
wcc -ms -et -d2 file.c
```

Where:
- `-et` - Enable profiling

### Assembly Output

```batch
wcc -ms -s file.c
```

Generates file.asm with assembly listing

## References

- [Open Watcom Documentation](http://www.openwatcom.org/doc.php)
- [DOS Programming Guide](http://www.ctyme.com/rbrown.htm)
- [C Compiler Options](http://www.openwatcom.org/ftp/manuals/current/cguide.pdf)