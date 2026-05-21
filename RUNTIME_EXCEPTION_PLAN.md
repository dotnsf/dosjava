# Runtime Exception Handling Implementation Plan

## Overview
Implement runtime exception handling to allow try-catch blocks to catch runtime errors (like division by zero) instead of terminating the program immediately.

## Current Behavior
```java
try {
    int a = 1 / 0;  // Causes "ERROR: Division by zero" and program terminates
    status = 2;
} catch (Exception e) {
    status = 3;  // Never executed
}
```

**Result**: Program terminates with error, catch block never executes.

## Expected Behavior
```java
try {
    int a = 1 / 0;  // Should throw exception
    status = 2;     // Should not execute
} catch (Exception e) {
    status = 3;     // Should execute
}
```

**Result**: Exception caught, catch block executes, program continues normally.

## Implementation Strategy

### Phase 1: Add Exception Helper Function

Create a helper function to handle runtime exceptions:

```c
/**
 * Throw a runtime exception
 * If in try block, jump to catch block
 * Otherwise, print error and terminate
 * 
 * @param ctx Execution context
 * @param message Error message
 * @return 0 if exception handled (jumped to catch), -1 if should terminate
 */
static int throw_runtime_exception(ExecutionContext* ctx, const char* message) {
    if (ctx->in_try_block && ctx->catch_pc) {
        /* Jump to catch block */
        ctx->pc = ctx->catch_pc;
        ctx->in_try_block = 0;
        return 0;  /* Exception handled */
    } else {
        /* No try block - print error and terminate */
        printf("ERROR: %s\n", message);
        return -1;  /* Should terminate */
    }
}
```

**Location**: `src/vm/interpreter.c` (after stack helper functions, before `interpreter_execute()`)

### Phase 2: Modify OP_DIV

**Current code** (line 458-471):
```c
case OP_DIV:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        printf("ERROR: Division by zero\n");
        return -1;  /* Always terminates */
    }
    result = (uint16_t)((int16_t)value1 / (int16_t)value2);
    if (stack_push_shared(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
```

**New code**:
```c
case OP_DIV:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        /* Try to throw exception */
        if (throw_runtime_exception(ctx, "Division by zero") != 0) {
            return -1;  /* Not in try block - terminate */
        }
        break;  /* Exception handled - jumped to catch block */
    }
    result = (uint16_t)((int16_t)value1 / (int16_t)value2);
    if (stack_push_shared(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
```

### Phase 3: Modify OP_MOD

**Current code** (line 473-486):
```c
case OP_MOD:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        printf("ERROR: Modulo by zero\n");
        return -1;
    }
    result = (uint16_t)((int16_t)value1 % (int16_t)value2);
    if (stack_push_shared(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
```

**New code**:
```c
case OP_MOD:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        /* Try to throw exception */
        if (throw_runtime_exception(ctx, "Modulo by zero") != 0) {
            return -1;  /* Not in try block - terminate */
        }
        break;  /* Exception handled - jumped to catch block */
    }
    result = (uint16_t)((int16_t)value1 % (int16_t)value2);
    if (stack_push_shared(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
```

## Test Cases

### Test 1: Division by zero in try block (exc3.jav)
```java
try {
    int a = 1 / 0;
    status = 2;  // Should not execute
} catch (Exception e) {
    status = 3;  // Should execute
}
```

**Expected output**:
```
=== Throw Statement Tests ===

Test 1: Basic throw
  Before throw
  Exception caught
  Status: 3
  Expected: 3
=== All Throw Tests Completed ===
```

### Test 2: Explicit throw (exc2.jav)
```java
try {
    throw new Exception();
    status = 2;  // Should not execute
} catch (Exception e) {
    status = 3;  // Should execute
}
```

**Expected**: Should continue to work as before (already implemented).

### Test 3: Division by zero outside try block
```java
int a = 1 / 0;  // No try block
```

**Expected output**:
```
ERROR: Division by zero
```
Program should terminate (same as current behavior).

## Implementation Steps

1. ✅ Create implementation plan document
2. Add `throw_runtime_exception()` helper function
3. Modify `OP_DIV` to use exception handling
4. Modify `OP_MOD` to use exception handling
5. Build and test exc3.jav
6. Test exc2.jav (regression test)
7. Test division by zero outside try block
8. Document changes

## Files to Modify

- `src/vm/interpreter.c` - Add helper function and modify OP_DIV/OP_MOD

## Potential Future Enhancements

Once this basic mechanism works, we can extend it to handle:
- Array index out of bounds
- Null pointer exceptions
- Stack overflow exceptions
- Other runtime errors

## Notes

- The exception mechanism is already implemented for explicit `throw` statements
- We're just extending it to handle runtime errors
- The `ctx->in_try_block` and `ctx->catch_pc` fields are already in place
- This is a minimal change with maximum compatibility