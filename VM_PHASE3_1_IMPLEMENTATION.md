# Phase 3.1: Method Invocation Implementation Plan

## Overview

This document provides a detailed, step-by-step implementation plan for Phase 3.1: Method Invocation using the shared stack approach. This design was chosen to minimize memory usage in the DOS environment.

## Design: Shared Stack Approach

### Key Principles

1. **Single Shared Stack**: All method calls share one operand stack (4KB = 2048 slots)
2. **Single Shared Locals**: All local variables share one array (256 bytes = 128 slots)
3. **Frame Pointers**: Each call frame stores offsets into shared arrays
4. **Fixed Memory**: No dynamic allocation during method calls
5. **Limited Depth**: Maximum 4 nested method calls

### Memory Layout

```
ExecutionContext:
├── shared_stack[2048]      // 4KB shared operand stack
├── stack_pointer           // Current position in shared_stack
├── shared_locals[128]      // 256 bytes shared local variables
├── local_pointer           // Current position in shared_locals
├── call_frames[4]          // Call frame stack (4 levels max)
└── call_depth              // Current call depth (0-3)

CallFrame:
├── return_pc               // Return address (pointer to bytecode)
├── frame_pointer           // Stack frame base offset
├── local_base              // Local variables base offset
└── local_count             // Number of locals in this frame
```

### Advantages

- **Predictable Memory**: ~8KB total (vs. 32KB+ with separate stacks)
- **No Allocation**: All memory pre-allocated at initialization
- **Simple Management**: Just pointer arithmetic
- **Fast**: No malloc/free overhead

### Limitations

- **Max Call Depth**: 4 levels (sufficient for most programs)
- **Max Stack Size**: 2048 slots total (shared across all frames)
- **Max Locals**: 128 slots total (shared across all frames)

## Implementation Steps

### Step 1: Update interpreter.h (30 minutes)

**File**: `dosjava/src/vm/interpreter.h`

**Changes**:

```c
/* Maximum call depth */
#define MAX_CALL_DEPTH 4

/* Shared stack size (2048 slots = 4KB) */
#define SHARED_STACK_SIZE 2048

/* Shared locals size (128 slots = 256 bytes) */
#define SHARED_LOCALS_SIZE 128

/* Call frame structure */
typedef struct CallFrame {
    uint8_t* return_pc;         /* Return address */
    uint16_t frame_pointer;     /* Stack frame base offset */
    uint16_t local_base;        /* Local variables base offset */
    uint8_t local_count;        /* Number of locals in this frame */
} CallFrame;

/* Execution context structure */
typedef struct ExecutionContext {
    /* Program counter */
    uint8_t* pc;
    uint8_t* code_start;
    uint16_t code_length;
    
    /* DJC file reference */
    DJCFile* djc_file;
    
    /* Shared operand stack */
    uint16_t shared_stack[SHARED_STACK_SIZE];
    uint16_t stack_pointer;     /* Current stack position */
    
    /* Shared local variables */
    uint16_t shared_locals[SHARED_LOCALS_SIZE];
    uint16_t local_pointer;     /* Current locals position */
    
    /* Call frames */
    CallFrame call_frames[MAX_CALL_DEPTH];
    uint8_t call_depth;         /* Current call depth (0-3) */
    
    /* Execution state */
    int running;
} ExecutionContext;
```

**Remove**:
- Old `Stack* stack` field
- Old `uint16_t* locals` field
- Old `uint16_t local_count` field

### Step 2: Update Stack Operations (1 hour)

**File**: `dosjava/src/vm/interpreter.c`

**Add new stack helper functions**:

```c
/* Push value onto shared stack */
static inline int stack_push_shared(ExecutionContext* ctx, uint16_t value) {
    if (ctx->stack_pointer >= SHARED_STACK_SIZE) {
        return -1;  /* Stack overflow */
    }
    ctx->shared_stack[ctx->stack_pointer++] = value;
    return 0;
}

/* Pop value from shared stack */
static inline uint16_t stack_pop_shared(ExecutionContext* ctx) {
    if (ctx->stack_pointer == 0) {
        return 0;  /* Stack underflow */
    }
    return ctx->shared_stack[--ctx->stack_pointer];
}

/* Peek at top of shared stack */
static inline uint16_t stack_peek_shared(ExecutionContext* ctx) {
    if (ctx->stack_pointer == 0) {
        return 0;
    }
    return ctx->shared_stack[ctx->stack_pointer - 1];
}

/* Get current frame pointer */
static inline uint16_t get_frame_pointer(ExecutionContext* ctx) {
    if (ctx->call_depth == 0) {
        return 0;
    }
    return ctx->call_frames[ctx->call_depth - 1].frame_pointer;
}

/* Get current local base */
static inline uint16_t get_local_base(ExecutionContext* ctx) {
    if (ctx->call_depth == 0) {
        return 0;
    }
    return ctx->call_frames[ctx->call_depth - 1].local_base;
}

/* Load local variable */
static inline uint16_t load_local(ExecutionContext* ctx, uint8_t index) {
    uint16_t base = get_local_base(ctx);
    if (base + index >= SHARED_LOCALS_SIZE) {
        return 0;  /* Out of bounds */
    }
    return ctx->shared_locals[base + index];
}

/* Store local variable */
static inline void store_local(ExecutionContext* ctx, uint8_t index, uint16_t value) {
    uint16_t base = get_local_base(ctx);
    if (base + index < SHARED_LOCALS_SIZE) {
        ctx->shared_locals[base + index] = value;
    }
}
```

### Step 3: Update interpreter_init_context (30 minutes)

**File**: `dosjava/src/vm/interpreter.c`

**Replace existing initialization**:

```c
int interpreter_init_context(ExecutionContext* ctx, DJCFile* djc_file, DJCMethod* method) {
    uint8_t* code;
    
    if (ctx == NULL || djc_file == NULL || method == NULL) {
        return -1;
    }
    
    /* Get method code */
    code = djc_get_method_code(djc_file, method);
    if (code == NULL) {
        return -1;
    }
    
    /* Initialize context */
    memset(ctx, 0, sizeof(ExecutionContext));
    
    ctx->pc = code;
    ctx->code_start = code;
    ctx->code_length = method->code_length;
    ctx->djc_file = djc_file;
    ctx->running = 1;
    
    /* Initialize shared stack */
    ctx->stack_pointer = 0;
    
    /* Initialize shared locals */
    ctx->local_pointer = 0;
    
    /* Allocate space for main method locals */
    if (method->max_locals > 0) {
        if (method->max_locals > SHARED_LOCALS_SIZE) {
            return -1;  /* Too many locals */
        }
        ctx->local_pointer = method->max_locals;
        /* Initialize locals to 0 */
        memset(ctx->shared_locals, 0, method->max_locals * sizeof(uint16_t));
    }
    
    /* Initialize call frames */
    ctx->call_depth = 0;
    
    return 0;
}
```

### Step 4: Update interpreter_free_context (15 minutes)

**File**: `dosjava/src/vm/interpreter.c`

**Simplify cleanup** (no dynamic memory to free):

```c
void interpreter_free_context(ExecutionContext* ctx) {
    if (ctx == NULL) {
        return;
    }
    
    /* Nothing to free - all memory is static */
    ctx->running = 0;
}
```

### Step 5: Update All Opcode Handlers (1-2 hours)

**File**: `dosjava/src/vm/interpreter.c`

**Replace all stack operations**:

```c
/* Example: OP_PUSH_INT */
case OP_PUSH_INT: {
    uint16_t value = (ctx->pc[0] << 8) | ctx->pc[1];
    ctx->pc += 2;
    if (stack_push_shared(ctx, value) != 0) {
        return -1;  /* Stack overflow */
    }
    break;
}

/* Example: OP_ADD */
case OP_ADD: {
    uint16_t b = stack_pop_shared(ctx);
    uint16_t a = stack_pop_shared(ctx);
    if (stack_push_shared(ctx, a + b) != 0) {
        return -1;
    }
    break;
}

/* Example: OP_LOAD_LOCAL */
case OP_LOAD_LOCAL: {
    uint8_t index = *ctx->pc++;
    uint16_t value = load_local(ctx, index);
    if (stack_push_shared(ctx, value) != 0) {
        return -1;
    }
    break;
}

/* Example: OP_STORE_LOCAL */
case OP_STORE_LOCAL: {
    uint8_t index = *ctx->pc++;
    uint16_t value = stack_pop_shared(ctx);
    store_local(ctx, index, value);
    break;
}
```

**Note**: Update ALL opcodes that use stack or locals (approximately 40 opcodes).

### Step 6: Implement INVOKE_STATIC (2-3 hours)

**File**: `dosjava/src/vm/interpreter.c`

```c
case OP_INVOKE_STATIC: {
    uint16_t method_index;
    DJCMethod* method;
    CallFrame* frame;
    uint8_t* method_code;
    
    /* Read method index (2 bytes, big-endian) */
    method_index = (ctx->pc[0] << 8) | ctx->pc[1];
    ctx->pc += 2;
    
    /* Check call depth */
    if (ctx->call_depth >= MAX_CALL_DEPTH) {
        fprintf(stderr, "Error: Call stack overflow (max depth: %d)\n", MAX_CALL_DEPTH);
        return -1;
    }
    
    /* Look up method */
    method = djc_find_method(ctx->djc_file, method_index);
    if (method == NULL) {
        fprintf(stderr, "Error: Method not found (index: %d)\n", method_index);
        return -1;
    }
    
    /* Check if method is native */
    if (method->flags & METHOD_NATIVE) {
        /* TODO: Handle native methods in Phase 4.1 */
        fprintf(stderr, "Error: Native methods not yet supported\n");
        return -1;
    }
    
    /* Get method code */
    method_code = djc_get_method_code(ctx->djc_file, method);
    if (method_code == NULL) {
        fprintf(stderr, "Error: Failed to get method code\n");
        return -1;
    }
    
    /* Check if we have enough space for locals */
    if (ctx->local_pointer + method->max_locals > SHARED_LOCALS_SIZE) {
        fprintf(stderr, "Error: Not enough space for local variables\n");
        return -1;
    }
    
    /* Save current state to call frame */
    frame = &ctx->call_frames[ctx->call_depth];
    frame->return_pc = ctx->pc;
    frame->frame_pointer = ctx->stack_pointer;
    frame->local_base = ctx->local_pointer;
    frame->local_count = method->max_locals;
    
    /* Increment call depth */
    ctx->call_depth++;
    
    /* Allocate space for new method's locals */
    ctx->local_pointer += method->max_locals;
    
    /* Initialize new locals to 0 */
    if (method->max_locals > 0) {
        memset(&ctx->shared_locals[frame->local_base], 0, 
               method->max_locals * sizeof(uint16_t));
    }
    
    /* TODO: Handle method parameters (pop from stack, store to locals) */
    /* For now, we assume no parameters */
    
    /* Set PC to method code */
    ctx->pc = method_code;
    ctx->code_start = method_code;
    ctx->code_length = method->code_length;
    
    break;
}
```

### Step 7: Enhance RETURN Opcodes (1-2 hours)

**File**: `dosjava/src/vm/interpreter.c`

```c
case OP_RETURN: {
    CallFrame* frame;
    
    /* Check if this is main method return */
    if (ctx->call_depth == 0) {
        ctx->running = 0;
        break;
    }
    
    /* Get call frame */
    ctx->call_depth--;
    frame = &ctx->call_frames[ctx->call_depth];
    
    /* Restore stack pointer (discard current frame's stack) */
    ctx->stack_pointer = frame->frame_pointer;
    
    /* Restore local pointer (free current frame's locals) */
    ctx->local_pointer = frame->local_base;
    
    /* Restore PC */
    ctx->pc = frame->return_pc;
    
    /* Note: code_start and code_length are not restored */
    /* This is OK because PC is absolute */
    
    break;
}

case OP_RETURN_VALUE: {
    CallFrame* frame;
    uint16_t return_value;
    
    /* Pop return value */
    return_value = stack_pop_shared(ctx);
    
    /* Check if this is main method return */
    if (ctx->call_depth == 0) {
        ctx->running = 0;
        break;
    }
    
    /* Get call frame */
    ctx->call_depth--;
    frame = &ctx->call_frames[ctx->call_depth];
    
    /* Restore stack pointer */
    ctx->stack_pointer = frame->frame_pointer;
    
    /* Push return value onto caller's stack */
    if (stack_push_shared(ctx, return_value) != 0) {
        return -1;
    }
    
    /* Restore local pointer */
    ctx->local_pointer = frame->local_base;
    
    /* Restore PC */
    ctx->pc = frame->return_pc;
    
    break;
}
```

### Step 8: Add djc_find_method Helper (30 minutes)

**File**: `dosjava/src/format/djc.h`

```c
/**
 * Find method by index
 * @param djc DJC file
 * @param method_index Method index
 * @return Pointer to method, or NULL if not found
 */
DJCMethod* djc_find_method(DJCFile* djc, uint16_t method_index);
```

**File**: `dosjava/src/format/djc.c`

```c
DJCMethod* djc_find_method(DJCFile* djc, uint16_t method_index) {
    if (djc == NULL || method_index >= djc->method_count) {
        return NULL;
    }
    return &djc->methods[method_index];
}
```

### Step 9: Update Makefile (5 minutes)

No changes needed - djc.c is already linked.

### Step 10: Create Test Program (30 minutes)

**File**: `dosjava/tests/vm/test_method_call.java`

```java
/*
 * Test: Simple Method Call
 * 
 * Tests basic method invocation with no parameters and no return value.
 */

class TestMethodCall {
    public static void main() {
        int x = 10;
        helper();
        int y = 20;
        return;
    }
    
    public static void helper() {
        int z = 30;
        return;
    }
}
```

**File**: `dosjava/tests/vm/test_method_return.java`

```java
/*
 * Test: Method with Return Value
 * 
 * Tests method invocation with return value.
 */

class TestMethodReturn {
    public static void main() {
        int result = getValue();
        return;
    }
    
    public static int getValue() {
        int value = 42;
        return value;
    }
}
```

## Testing Strategy

### Test 1: Compile Test Programs

```batch
cd dosjava
build\bin\djc.exe tests\vm\test_method_call.java
build\bin\djc.exe tests\vm\test_method_return.java
```

**Expected**: No compilation errors, .djc files created.

### Test 2: Run with Debug Mode

```batch
build\bin\djvm.exe -d test_method_call.djc
```

**Expected Output**:
```
=== Debug Mode ===
[PC=0000] Opcode=0x02  ; PUSH_INT 10
[PC=0003] Opcode=0x15  ; STORE_0
[PC=0004] Opcode=0x40  ; INVOKE_STATIC
[PC=0007] Opcode=0x02  ; PUSH_INT 20
[PC=0010] Opcode=0x15  ; STORE_1
[PC=0011] Opcode=0x42  ; RETURN

Execution completed successfully
```

### Test 3: Verify Stack and Locals

Add debug output in INVOKE_STATIC and RETURN to verify:
- Stack pointer is saved/restored correctly
- Local pointer is saved/restored correctly
- Call depth increments/decrements correctly

### Test 4: Test Nested Calls

Create a test with 3-4 levels of nesting to verify call depth limit.

## Success Criteria

- ✅ All test programs compile without errors
- ✅ Simple method call executes correctly
- ✅ Method with return value works
- ✅ Stack and locals are properly managed
- ✅ Call depth limit is enforced
- ✅ No memory leaks or corruption
- ✅ Debug output shows correct execution flow

## Estimated Time

- Step 1-4: 2 hours (data structure updates)
- Step 5: 2 hours (update all opcodes)
- Step 6-7: 4 hours (INVOKE_STATIC and RETURN)
- Step 8-10: 1 hour (helpers and tests)

**Total: 9 hours (1-2 days)**

## Next Steps After Completion

1. Test with more complex programs
2. Add parameter passing support
3. Move to Phase 4.1 (Native methods)
4. Implement System.out.println

---

*Implementation plan created: 2026-04-20*
*Status: Ready for implementation*