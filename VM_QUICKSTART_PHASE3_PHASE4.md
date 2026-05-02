# DOS Java VM - Phase 3 & Phase 4 Quick Start Guide

## Overview

This guide provides a quick reference for implementing Phase 3 (Remaining Opcodes) and Phase 4 (Runtime Library Integration) of the DOS Java Virtual Machine.

## Quick Reference

### Implementation Order (Recommended)

1. **Phase 3.1: Method Invocation** (Week 1) - HIGH PRIORITY
   - Call stack management
   - INVOKE_STATIC opcode
   - Enhanced RETURN opcodes
   - **Why first:** Enables modular code, required for runtime library

2. **Phase 4.1: System.out.println** (Week 3, Days 1-3) - HIGH PRIORITY
   - Native method infrastructure
   - System.out integration
   - **Why second:** Provides immediate user value, enables debugging

3. **Phase 3.2: Object Operations** (Week 2) - MEDIUM PRIORITY
   - Object heap manager
   - NEW, GET_FIELD, PUT_FIELD opcodes
   - **Why third:** Foundation for OOP features

4. **Phase 4.2: String Operations** (Week 3, Days 4-5) - MEDIUM PRIORITY
   - String pool integration
   - String native methods
   - **Why fourth:** Complements System.out.println

5. **Phase 3.3: Array Operations** (Week 4, Days 1-2) - LOW PRIORITY
   - Array allocation and access
   - NEW_ARRAY, ARRAY_LOAD, ARRAY_STORE opcodes

6. **Phase 3.4: Type Operations** (Week 4, Days 3-4) - LOW PRIORITY
   - INSTANCEOF, CHECKCAST opcodes

## Phase 3.1: Method Invocation - Step by Step

### Step 1: Add Call Frame Structure (30 min)

**File:** `dosjava/src/vm/interpreter.h`

```c
/* Add after ExecutionContext forward declaration */
#define MAX_CALL_DEPTH 8

typedef struct CallFrame {
    uint8_t* return_pc;           /* Return address */
    uint16_t* return_locals;      /* Caller's locals */
    uint16_t return_local_count;  /* Number of caller's locals */
    Stack* return_stack;          /* Caller's stack */
} CallFrame;

/* Add to ExecutionContext structure */
typedef struct ExecutionContext {
    /* ... existing fields ... */
    CallFrame call_frames[MAX_CALL_DEPTH];
    uint8_t call_depth;
} ExecutionContext;
```

### Step 2: Initialize Call Stack (15 min)

**File:** `dosjava/src/vm/interpreter.c`

```c
/* In interpreter_init_context(), after existing initialization */
ctx->call_depth = 0;
memset(ctx->call_frames, 0, sizeof(ctx->call_frames));
```

### Step 3: Implement INVOKE_STATIC (2-3 hours)

**File:** `dosjava/src/vm/interpreter.c`

```c
case OP_INVOKE_STATIC: {
    uint16_t method_index;
    DJCMethod* method;
    CallFrame* frame;
    
    /* Check call depth */
    if (ctx->call_depth >= MAX_CALL_DEPTH) {
        fprintf(stderr, "Error: Call stack overflow\n");
        return -1;
    }
    
    /* Read method index (2 bytes) */
    method_index = (ctx->pc[0] << 8) | ctx->pc[1];
    ctx->pc += 2;
    
    /* Look up method */
    method = djc_find_method(ctx->djc_file, method_index);
    if (method == NULL) {
        fprintf(stderr, "Error: Method not found: %d\n", method_index);
        return -1;
    }
    
    /* Save current state to call frame */
    frame = &ctx->call_frames[ctx->call_depth];
    frame->return_pc = ctx->pc;
    frame->return_locals = ctx->locals;
    frame->return_local_count = ctx->local_count;
    frame->return_stack = ctx->stack;
    
    /* Increment call depth */
    ctx->call_depth++;
    
    /* Allocate new locals for called method */
    ctx->local_count = method->max_locals;
    if (ctx->local_count > 0) {
        ctx->locals = (uint16_t*)memory_alloc(sizeof(uint16_t) * ctx->local_count);
        if (ctx->locals == NULL) {
            return -1;
        }
        memset(ctx->locals, 0, sizeof(uint16_t) * ctx->local_count);
    }
    
    /* Allocate new stack for called method */
    ctx->stack = (Stack*)memory_alloc(sizeof(Stack));
    if (ctx->stack == NULL) {
        memory_free(ctx->locals);
        return -1;
    }
    if (stack_init(ctx->stack, method->max_stack) != 0) {
        memory_free(ctx->stack);
        memory_free(ctx->locals);
        return -1;
    }
    
    /* Set PC to method code */
    ctx->pc = djc_get_method_code(ctx->djc_file, method);
    ctx->code_start = ctx->pc;
    ctx->code_length = method->code_length;
    
    break;
}
```

### Step 4: Enhance RETURN Opcodes (1-2 hours)

**File:** `dosjava/src/vm/interpreter.c`

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
    
    /* Free current locals and stack */
    if (ctx->locals != NULL) {
        memory_free(ctx->locals);
    }
    if (ctx->stack != NULL) {
        stack_free(ctx->stack);
        memory_free(ctx->stack);
    }
    
    /* Restore caller's state */
    ctx->pc = frame->return_pc;
    ctx->locals = frame->return_locals;
    ctx->local_count = frame->return_local_count;
    ctx->stack = frame->return_stack;
    
    /* Recalculate code boundaries */
    /* Note: This is simplified - may need adjustment */
    ctx->code_start = ctx->pc - 100;  /* Approximate */
    ctx->code_length = 1000;          /* Approximate */
    
    break;
}

case OP_RETURN_VALUE: {
    CallFrame* frame;
    uint16_t return_value;
    
    /* Pop return value */
    return_value = stack_pop(ctx->stack);
    
    /* Check if this is main method return */
    if (ctx->call_depth == 0) {
        ctx->running = 0;
        break;
    }
    
    /* Get call frame */
    ctx->call_depth--;
    frame = &ctx->call_frames[ctx->call_depth];
    
    /* Free current locals and stack */
    if (ctx->locals != NULL) {
        memory_free(ctx->locals);
    }
    if (ctx->stack != NULL) {
        stack_free(ctx->stack);
        memory_free(ctx->stack);
    }
    
    /* Restore caller's state */
    ctx->pc = frame->return_pc;
    ctx->locals = frame->return_locals;
    ctx->local_count = frame->return_local_count;
    ctx->stack = frame->return_stack;
    
    /* Push return value onto caller's stack */
    stack_push(ctx->stack, return_value);
    
    /* Recalculate code boundaries */
    ctx->code_start = ctx->pc - 100;
    ctx->code_length = 1000;
    
    break;
}
```

### Step 5: Add Method Lookup Helper (30 min)

**File:** `dosjava/src/format/djc.h`

```c
/* Add function declaration */
DJCMethod* djc_find_method(DJCFile* djc, uint16_t method_index);
```

**File:** `dosjava/src/format/djc.c`

```c
DJCMethod* djc_find_method(DJCFile* djc, uint16_t method_index) {
    if (djc == NULL || method_index >= djc->method_count) {
        return NULL;
    }
    return &djc->methods[method_index];
}
```

### Step 6: Test Method Calls (2-3 hours)

**Create test file:** `dosjava/tests/vm/test_method_calls.java`

```java
class TestMethods {
    public static void main() {
        int result = add(5, 3);
        return;
    }
    
    public static int add(int a, int b) {
        return a + b;
    }
}
```

**Compile and test:**
```batch
cd dosjava
wmake djc
build\bin\djc.exe tests\vm\test_method_calls.java
build\bin\djvm.exe -d test_method_calls.djc
```

## Phase 4.1: System.out.println - Step by Step

### Step 1: Create Native Method Infrastructure (1-2 hours)

**File:** `dosjava/src/vm/native.h` (new)

```c
#ifndef NATIVE_H
#define NATIVE_H

#include "interpreter.h"

/* Native method handler function type */
typedef int (*NativeMethod)(ExecutionContext* ctx);

/* Native method registry entry */
typedef struct NativeMethodEntry {
    const char* class_name;
    const char* method_name;
    const char* signature;
    NativeMethod handler;
} NativeMethodEntry;

/* Initialize native method registry */
int native_init(void);

/* Register a native method */
int native_register(const char* class_name, const char* method_name, 
                    const char* signature, NativeMethod handler);

/* Find a native method */
NativeMethod native_find(const char* class_name, const char* method_name);

/* Shutdown native method registry */
void native_shutdown(void);

#endif /* NATIVE_H */
```

**File:** `dosjava/src/vm/native.c` (new)

```c
#include "native.h"
#include "memory.h"
#include <string.h>
#include <stdio.h>

#define MAX_NATIVE_METHODS 32

static NativeMethodEntry native_methods[MAX_NATIVE_METHODS];
static int native_method_count = 0;

int native_init(void) {
    native_method_count = 0;
    memset(native_methods, 0, sizeof(native_methods));
    return 0;
}

int native_register(const char* class_name, const char* method_name,
                    const char* signature, NativeMethod handler) {
    NativeMethodEntry* entry;
    
    if (native_method_count >= MAX_NATIVE_METHODS) {
        return -1;
    }
    
    entry = &native_methods[native_method_count];
    entry->class_name = class_name;
    entry->method_name = method_name;
    entry->signature = signature;
    entry->handler = handler;
    
    native_method_count++;
    return 0;
}

NativeMethod native_find(const char* class_name, const char* method_name) {
    int i;
    
    for (i = 0; i < native_method_count; i++) {
        if (strcmp(native_methods[i].class_name, class_name) == 0 &&
            strcmp(native_methods[i].method_name, method_name) == 0) {
            return native_methods[i].handler;
        }
    }
    
    return NULL;
}

void native_shutdown(void) {
    native_method_count = 0;
}
```

### Step 2: Implement System.out.println Native Methods (1 hour)

**File:** `dosjava/src/vm/native.c` (add to end)

```c
#include "../runtime/system.h"

/* Native: System.out.println() */
static int native_system_out_println(ExecutionContext* ctx) {
    system_println_empty();
    return 0;
}

/* Native: System.out.println(int) */
static int native_system_out_println_int(ExecutionContext* ctx) {
    int16_t value = (int16_t)stack_pop(ctx->stack);
    system_println_int(value);
    return 0;
}

/* Register System.out methods */
int native_register_system_methods(void) {
    native_register("java/lang/System", "println", "()V", 
                    native_system_out_println);
    native_register("java/lang/System", "println", "(I)V", 
                    native_system_out_println_int);
    return 0;
}
```

### Step 3: Initialize Native Methods in VM (30 min)

**File:** `dosjava/src/vm/djvm.c`

```c
#include "native.h"

/* In main(), before loading DJC file */
if (native_init() != 0) {
    fprintf(stderr, "Error: Failed to initialize native methods\n");
    return 1;
}

if (native_register_system_methods() != 0) {
    fprintf(stderr, "Error: Failed to register System methods\n");
    return 1;
}

/* Before return, add cleanup */
native_shutdown();
```

### Step 4: Handle Native Methods in INVOKE_STATIC (1 hour)

**File:** `dosjava/src/vm/interpreter.c`

```c
case OP_INVOKE_STATIC: {
    uint16_t method_index;
    DJCMethod* method;
    NativeMethod native_handler;
    
    /* Read method index */
    method_index = (ctx->pc[0] << 8) | ctx->pc[1];
    ctx->pc += 2;
    
    /* Look up method */
    method = djc_find_method(ctx->djc_file, method_index);
    if (method == NULL) {
        return -1;
    }
    
    /* Check if method is native */
    if (method->flags & METHOD_FLAG_NATIVE) {
        /* Look up native handler */
        native_handler = native_find(method->class_name, method->name);
        if (native_handler == NULL) {
            fprintf(stderr, "Error: Native method not found: %s.%s\n",
                    method->class_name, method->name);
            return -1;
        }
        
        /* Call native method */
        if (native_handler(ctx) != 0) {
            return -1;
        }
        
        /* Continue execution (no call frame needed) */
        break;
    }
    
    /* ... rest of INVOKE_STATIC implementation for non-native methods ... */
}
```

### Step 5: Update Makefile (15 min)

**File:** `dosjava/Makefile`

```makefile
# Add to object files
VM_OBJS = $(BUILD_OBJ)\interpreter.obj $(BUILD_OBJ)\memory.obj \
          $(BUILD_OBJ)\stack.obj $(BUILD_OBJ)\djc.obj \
          $(BUILD_OBJ)\opcodes.obj $(BUILD_OBJ)\system.obj \
          $(BUILD_OBJ)\string.obj $(BUILD_OBJ)\integer.obj \
          $(BUILD_OBJ)\object.obj $(BUILD_OBJ)\native.obj

# Add compilation rule
$(BUILD_OBJ)\native.obj: src\vm\native.c src\vm\native.h
	$(CC) $(CFLAGS) -fo=$@ src\vm\native.c
```

### Step 6: Test System.out.println (1 hour)

**Create test file:** `dosjava/tests/vm/test_println.java`

```java
class TestPrintln {
    public static void main() {
        System.out.println(42);
        System.out.println(100);
        System.out.println();
        return;
    }
}
```

**Expected output:**
```
42
100

```

## Common Issues and Solutions

### Issue 1: Call Stack Overflow
**Symptom:** "Call stack overflow" error
**Solution:** Check recursion depth, ensure RETURN opcodes properly decrement call_depth

### Issue 2: Memory Allocation Failure
**Symptom:** NULL pointer after memory_alloc
**Solution:** Reduce allocation sizes, check available memory in DOS

### Issue 3: Method Not Found
**Symptom:** "Method not found" error
**Solution:** Verify method index in bytecode, check DJC file method table

### Issue 4: Native Method Not Found
**Symptom:** "Native method not found" error
**Solution:** Ensure native_register_system_methods() is called, check method signatures

### Issue 5: Stack Corruption
**Symptom:** Incorrect values on stack, crashes
**Solution:** Verify stack push/pop balance, check parameter passing

## Testing Checklist

### Phase 3.1 Tests
- [ ] Simple method call (no parameters, no return)
- [ ] Method with parameters
- [ ] Method with return value
- [ ] Nested method calls (2-3 levels)
- [ ] Recursive method (factorial)
- [ ] Multiple methods in same class

### Phase 4.1 Tests
- [ ] System.out.println() with no arguments
- [ ] System.out.println(int) with positive number
- [ ] System.out.println(int) with negative number
- [ ] System.out.println(int) with zero
- [ ] Multiple println calls in sequence
- [ ] println inside method call

## Build Commands Quick Reference

```batch
# Clean build
wmake clean

# Build compiler
wmake djc

# Build VM
wmake djvm

# Build all
wmake all

# Compile Java to DJC
build\bin\djc.exe examples\01_hello.java

# Run DJC file
build\bin\djvm.exe examples\01_hello.djc

# Run with debug mode
build\bin\djvm.exe -d examples\01_hello.djc

# Run with verbose mode
build\bin\djvm.exe -v examples\01_hello.djc
```

## Next Steps After Phase 3.1 & 4.1

1. **Verify basic functionality:**
   - Method calls work correctly
   - System.out.println produces output
   - No memory leaks or crashes

2. **Create more test cases:**
   - Complex method call patterns
   - Edge cases (max call depth, etc.)

3. **Move to Phase 3.2 (Objects):**
   - Implement object heap
   - Add NEW, GET_FIELD, PUT_FIELD opcodes

4. **Document lessons learned:**
   - Update this guide with any issues encountered
   - Add troubleshooting tips

---

*Quick Start Guide created: 2026-04-20*
*For detailed information, see: VM_PHASE3_PHASE4_PLAN.md*