# Memory Manager Debug Guide

## Overview
This guide explains how to debug the memory manager initialization issue in djvm.exe.

## Debug Information Added

### 1. memory.c (Lines 20-42)
Added debug output to `memory_init()`:
- Reports when memory is already initialized
- Shows heap_size parameter value
- Reports malloc success/failure
- Shows heap_start pointer address

### 2. djvm.c (Lines 235-243)
Added debug output around memory initialization:
- Reports before calling memory_init()
- Reports after successful initialization
- Uses fflush(stdout) to ensure output appears immediately

## Testing in DOSBox-X

### Step 1: Compile Test Program
```
C:\DOSJAVA> djc sockinit.jav
```

Expected output:
```
Compiling sockinit.jav...
Code generation complete
Output: sockinit.djc
```

### Step 2: Run with Debug Output
```
C:\DOSJAVA> djvm -d sockinit.djc
```

Expected debug output (if working):
```
[DEBUG] About to initialize memory manager...
[DEBUG] Initializing memory manager with heap_size=30720
[DEBUG] malloc succeeded, heap_start=XXXX:XXXX
[DEBUG] Memory manager initialized successfully
```

Expected debug output (if failing):
```
[DEBUG] About to initialize memory manager...
[DEBUG] Initializing memory manager with heap_size=30720
[DEBUG] malloc failed for heap_size=30720
ERROR: Failed to initialize memory manager
```

### Step 3: Analyze Results

#### If malloc succeeds but VM still fails:
- Check system_init() or native_init() functions
- Problem is after memory initialization

#### If malloc fails:
- heap_size=30720 bytes (~30KB) is too large for DOS environment
- Need to reduce DEFAULT_HEAP_SIZE in memory.h
- Possible values to try:
  - 16384 (16KB)
  - 8192 (8KB)
  - 4096 (4KB)

## Memory Model Information

Current configuration:
- **Memory Model**: Medium (-mm)
- **Code Segment**: Up to 1MB (multiple segments)
- **Data Segment**: 64KB (single segment)
- **Heap Allocation**: Uses malloc() from data segment

### DOS Memory Constraints
- Total conventional memory: 640KB
- Available for programs: ~550-600KB after DOS
- Data segment limit: 64KB
- Stack: 16KB (configured in Makefile)

### Heap Size Calculation
With 64KB data segment:
- Global variables: ~10-15KB
- Stack: 16KB (separate)
- Heap: Should be < 30KB to fit in remaining data segment

## Troubleshooting

### Problem: malloc fails with heap_size=30720
**Solution**: Reduce DEFAULT_HEAP_SIZE in src/vm/memory.h

Edit memory.h line 14:
```c
/* Before */
#define DEFAULT_HEAP_SIZE 30720  /* ~30KB for heap */

/* After - try progressively smaller values */
#define DEFAULT_HEAP_SIZE 16384  /* ~16KB for heap */
```

Then rebuild:
```
C:\Users\dotns\src> cd dosjava
C:\Users\dotns\src\dosjava> .\build_all.bat
```

### Problem: VM crashes without error message
**Possible causes**:
1. Stack overflow - increase stack size in Makefile
2. Heap corruption - run memory_check() before operations
3. Pointer arithmetic error - check memory.c calculations

### Problem: "Already initialized" message appears
**Cause**: memory_init() called multiple times
**Solution**: Check djvm.c for duplicate initialization calls

## Next Steps

1. Run test in DOSBox-X and capture debug output
2. Report results:
   - Did malloc succeed or fail?
   - What was the heap_start address?
   - Did VM proceed past memory initialization?
3. Based on results, adjust heap size or investigate next initialization step

## Files Modified

- `src/vm/memory.c` - Added debug output to memory_init()
- `src/vm/djvm.c` - Added debug output around memory_init() call
- `build/bin/djvm.exe` - Rebuilt with debug information

## Reverting Debug Output

After fixing the issue, remove debug printf() statements:
1. Remove lines from memory.c (lines 22, 30, 33, 38)
2. Remove lines from djvm.c (lines 237, 242)
3. Rebuild with `.\build_all.bat`