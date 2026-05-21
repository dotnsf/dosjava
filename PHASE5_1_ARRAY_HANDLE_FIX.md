# Phase 5.1: Array Handle Fix for Large Memory Model

## Problem

When migrating from Medium (-mm) to Large (-ml) memory model, arrays.jav failed with:
```
ERROR: Array index out of bounds (idx=0 len=0 val=64 raw=552)
```

The array length was being read as 0 instead of 5.

## Root Cause

In Large memory model:
- **Pointers are 32-bit** (Far pointers: segment:offset)
- **Stack values are 16-bit** (uint16_t)

The original code cast array pointers to uint16_t:
```c
raw_ref = (uint16_t)array_data;  // WRONG: Truncates 32-bit pointer to 16-bit
```

This truncated the pointer, causing incorrect memory access.

## Solution: Array Handle Table

Instead of storing pointers directly on the stack, we use an **array handle table**:

1. **Allocate array** → Get pointer from heap
2. **Store pointer** in handle table → Get 16-bit handle (1-255)
3. **Push handle** to stack (not pointer)
4. **Pop handle** from stack → Look up pointer in table
5. **Access array** using pointer from table

### Implementation

#### memory.h
```c
/* Array handle table for Large memory model compatibility */
#define MAX_ARRAY_HANDLES 256

typedef struct {
    void* array_ptr;         /* Pointer to array data */
    uint8_t in_use;          /* 1 if handle is active, 0 if free */
} ArrayHandle;

uint16_t memory_alloc_array_handle(void* array_ptr);
void* memory_get_array_ptr(uint16_t handle);
void memory_free_array_handle(uint16_t handle);
```

#### memory.c
- Global handle table: `ArrayHandle g_array_handles[256]`
- `memory_alloc_array_handle()`: Find free slot, store pointer, return handle
- `memory_get_array_ptr()`: Look up pointer by handle
- `memory_free_array_handle()`: Mark slot as free

#### interpreter.c

**OP_NEW_ARRAY:**
```c
array_data = (uint16_t*)memory_alloc(total_size);
array_data[0] = size;  // Store length

// Get handle for pointer
array_handle = memory_alloc_array_handle(array_data);
stack_push_shared(ctx, array_handle);  // Push handle, not pointer
```

**OP_ARRAY_LOAD/STORE/LENGTH:**
```c
array_handle = stack_pop_shared(ctx);
array_data = (uint16_t*)memory_get_array_ptr(array_handle);  // Get pointer from handle
// Now access array_data normally
```

## Benefits

1. **Memory Model Independent**: Works with both Medium and Large models
2. **Simple Stack Management**: Always 16-bit values on stack
3. **Type Safe**: No pointer truncation
4. **Scalable**: Supports up to 255 concurrent arrays

## Testing

After fix, arrays.jav should work correctly in DOSBox-X:
```
djc arrays.jav
djvm arrays.jav
```

Expected output:
```
Array test: 64
Array length: 5
```

## Files Modified

1. `src/vm/memory.h` - Added ArrayHandle struct and functions
2. `src/vm/memory.c` - Implemented handle table management
3. `src/vm/interpreter.c` - Modified OP_NEW_ARRAY, OP_ARRAY_LOAD, OP_ARRAY_STORE, OP_ARRAY_LENGTH

## Build Status

✅ Build successful with Large memory model (-ml)
✅ 32KB heap allocated
⏳ Testing in DOSBox-X pending

## Next Steps

1. Test arrays.jav in DOSBox-X
2. Test other array-using programs
3. If successful, proceed with Phase 5.2 (Long Type Implementation)