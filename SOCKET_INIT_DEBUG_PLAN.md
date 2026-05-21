# Socket Initialization Debug Plan
## Phase 4.2 Day 22-24 - "Init: Could not setup TCP buffers" Error Investigation

## Problem Summary

### Current Status
- **Error**: "Init: Could not setup TCP buffers" from mTCP library
- **Location**: `Utils::initStack()` call in `socket_init()` (socket.cpp line 82)
- **Environment**: DOSBox-X, Medium Memory Model (-mm)
- **Configuration**: 2 TCP sockets, 2 transmit buffers

### Observations
1. Error occurs with both Medium (-mm) and Large (-ml) memory models
2. Error occurs with transmit buffers = 2, 3, and 4
3. "End: heap is corrupted!" message disappeared after reverting to Medium model
4. doscurl works correctly with same mTCP library (Large model, 4 buffers)

## Root Cause Hypotheses

### Hypothesis 1: Memory Allocation Failure
**Theory**: djvm.exe has insufficient free memory for mTCP buffers
- VM already uses significant memory (heap, stack, code)
- mTCP needs contiguous memory blocks for buffers
- DOS 640KB limit is being reached

**Evidence**:
- Error message "Could not setup TCP buffers" suggests malloc/allocation failure
- Works in doscurl (standalone program with less memory usage)
- Fails in djvm (VM with existing memory allocations)

### Hypothesis 2: Initialization Order Issue
**Theory**: mTCP initialization happens too late, after VM memory is fragmented
- VM heap initialization fragments available memory
- mTCP cannot find contiguous blocks for buffers

**Evidence**:
- Socket initialization happens after VM startup
- VM has already allocated heap, stack, and loaded .djc file

### Hypothesis 3: Configuration File Issue
**Theory**: sample.cfg has incompatible settings for VM environment
- PACKET_BUFFERS, TCP_SOCKET_RING_SIZE may be too large
- Settings optimized for standalone programs, not VMs

**Evidence**:
- Using same sample.cfg as doscurl
- No VM-specific configuration tuning

### Hypothesis 4: Stack Size Limitation
**Theory**: 16KB stack is insufficient for mTCP initialization
- mTCP initialization may use significant stack space
- Stack overflow during `Utils::initStack()`

**Evidence**:
- djvm stack = 16KB
- doscurl stack = 8KB (but works)
- Unlikely but possible

## Debug Plan

### Phase 1: Add Detailed Debug Output (Day 1)

#### Step 1.1: Instrument socket_init()
Add debug output before and after each mTCP call:

```cpp
// In socket.cpp socket_init()
printf("DEBUG: Starting socket_init()\n");
printf("DEBUG: Available memory: %lu bytes\n", _memavl());
printf("DEBUG: Largest free block: %lu bytes\n", _memmax());

printf("DEBUG: Calling Utils::parseEnv()\n");
if (Utils::parseEnv() != 0) {
    printf("DEBUG: Utils::parseEnv() FAILED\n");
    return SOCKET_ERR_INIT;
}
printf("DEBUG: Utils::parseEnv() OK\n");

printf("DEBUG: Calling Utils::initStack(2, 2, NULL, NULL)\n");
printf("DEBUG: Memory before initStack: %lu bytes\n", _memavl());
int rc = Utils::initStack(2, 2, NULL, NULL);
printf("DEBUG: Utils::initStack() returned: %d\n", rc);
printf("DEBUG: Memory after initStack: %lu bytes\n", _memavl());
```

#### Step 1.2: Check mTCP Source Code
Examine `Utils::initStack()` in mTCP library to understand:
- What "Could not setup TCP buffers" means exactly
- Which allocation is failing
- Memory requirements for different buffer counts

#### Step 1.3: Test Memory Availability
Create minimal test program to check available memory:

```c
// test_memory_avail.c
#include <stdio.h>
#include <malloc.h>

int main() {
    printf("Available memory: %lu bytes\n", _memavl());
    printf("Largest free block: %lu bytes\n", _memmax());
    
    // Try allocating various sizes
    void* p1 = malloc(1024);
    printf("After 1KB alloc: %lu bytes\n", _memavl());
    free(p1);
    
    void* p2 = malloc(8192);
    printf("After 8KB alloc: %lu bytes\n", _memavl());
    free(p2);
    
    return 0;
}
```

### Phase 2: Reduce Memory Usage (Day 2)

#### Step 2.1: Minimize VM Heap Size
Reduce initial heap allocation in djvm:

```c
// In djvm.c or memory.c
#define HEAP_SIZE 8192  // Reduce from current size
```

#### Step 2.2: Reduce Constant Pool Size
Check if constant pool can be optimized:
- Use smaller initial allocation
- Grow dynamically if needed

#### Step 2.3: Test with Minimal .djc File
Create smallest possible test:

```java
// minimal.java
public class minimal {
    public static void main(String[] args) {
        System.out.println("Start");
        // Socket init here
    }
}
```

### Phase 3: Alternative Initialization Strategies (Day 3)

#### Step 3.1: Early Initialization
Initialize mTCP before VM heap allocation:

```c
// In djvm.c main()
// Move socket_subsystem_init() to very beginning
socket_subsystem_init();  // Before memory_init()
memory_init();
// ... rest of VM initialization
```

#### Step 3.2: Reduce Buffer Requirements
Try absolute minimum configuration:

```cpp
Utils::initStack(1, 1, NULL, NULL);  // 1 socket, 1 buffer
```

#### Step 3.3: Use Different mTCP Configuration
Create vm-specific sample.cfg:

```
PACKETINT 0x60
PACKET_BUFFERS 4      # Reduce from 8
TCP_SOCKET_RING_SIZE 1024  # Reduce from 2048
TCP_MAX_SOCKETS 1     # Reduce from 2
```

### Phase 4: Compare with Working doscurl (Day 4)

#### Step 4.1: Memory Usage Comparison
Add same debug output to doscurl:
- Memory before/after mTCP init
- Compare with djvm values

#### Step 4.2: Binary Size Comparison
Check executable sizes:
```
dir doscurl.exe
dir djvm.exe
```

#### Step 4.3: Memory Map Analysis
Use Watcom tools to analyze memory layout:
```
wdump djvm.exe
wdump doscurl.exe
```

### Phase 5: Workaround Solutions (Day 5)

#### Option A: External Socket Helper
Create separate socket helper program:
- djvm.exe launches sockhelp.exe
- sockhelp.exe handles all network I/O
- Communication via files or shared memory

#### Option B: Stub Initialization
Initialize mTCP in stub loader:
- Create djvm_net.exe wrapper
- Wrapper initializes mTCP
- Wrapper loads and runs djvm.exe

#### Option C: Delayed Loading
Load mTCP library dynamically:
- Don't link mTCP statically
- Load mTCP.DLL or similar at runtime
- Initialize only when needed

## Success Criteria

### Minimum Success
- Identify exact cause of "Could not setup TCP buffers" error
- Document memory requirements for mTCP in VM environment

### Ideal Success
- Socket initialization succeeds in djvm
- Can create TCP connection from Java code
- No memory corruption or crashes

## Testing Protocol

### Test 1: Memory Availability
```
cd build\bin
test_memory_avail.exe
```
Expected: Show available memory in DOS environment

### Test 2: Minimal Socket Init
```
cd build\bin
djvm minimal.djc
```
Expected: Socket init succeeds with minimal program

### Test 3: Full Socket Test
```
cd build\bin
djvm sockconn.djc
```
Expected: Socket init succeeds, connection attempt works

## Rollback Plan

If socket initialization cannot be fixed:
1. Document limitation in README
2. Implement file-based I/O workaround
3. Consider external helper program approach
4. Plan for future 32-bit DOS extender version

## Timeline

- **Day 1**: Debug output and investigation (4 hours)
- **Day 2**: Memory optimization attempts (4 hours)
- **Day 3**: Alternative strategies (4 hours)
- **Day 4**: Comparison analysis (2 hours)
- **Day 5**: Workaround implementation (4 hours)

**Total**: 18 hours over 5 days

## Next Steps

1. Implement Phase 1 Step 1.1 (add debug output)
2. Rebuild and test in DOSBox-X
3. Analyze debug output to identify exact failure point
4. Proceed to Phase 2 or 3 based on findings