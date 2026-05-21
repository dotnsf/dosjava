# Runtime Exception Handling Implementation

## Date
2026-05-21

## Overview
Implemented runtime exception handling to allow try-catch blocks to catch runtime errors (like division by zero) instead of terminating the program immediately.

## Problem Statement

### Before Implementation
```java
try {
    int a = 1 / 0;  // Causes "ERROR: Division by zero" and program terminates
    status = 2;
} catch (Exception e) {
    status = 3;  // Never executed
}
```

**Result**: Program terminated with error, catch block never executed.

### After Implementation
```java
try {
    int a = 1 / 0;  // Throws exception
    status = 2;     // Not executed
} catch (Exception e) {
    status = 3;     // Executed successfully
}
```

**Result**: Exception caught, catch block executes, program continues normally.

## Implementation Details

### 1. Exception Helper Function

Added `throw_runtime_exception()` to `src/vm/interpreter.c` (after line 158):

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

**Key Features**:
- Checks if currently in try block (`ctx->in_try_block`)
- If yes, jumps to catch block (`ctx->catch_pc`)
- If no, prints error and returns -1 to terminate

### 2. OP_DIV Modification

Modified division operation (line 481-497):

**Before**:
```c
case OP_DIV:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        printf("ERROR: Division by zero\n");
        return -1;  // Always terminates
    }
    // ... normal division
```

**After**:
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
    // ... normal division
```

### 3. OP_MOD Modification

Modified modulo operation (line 499-514):

**Before**:
```c
case OP_MOD:
    value2 = stack_pop_shared(ctx);
    value1 = stack_pop_shared(ctx);
    if (value2 == 0) {
        printf("ERROR: Modulo by zero\n");
        return -1;  // Always terminates
    }
    // ... normal modulo
```

**After**:
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
    // ... normal modulo
```

## Test Results

### Test 1: exc3.jav - Division by Zero in Try Block ✅

**Code**:
```java
try {
    System.out.println("  Before throw");
    a = 1 / 0;
    status = 2;  // Should not execute
} catch (Exception e) {
    System.out.println("  Exception caught");
    status = 3;
}
System.out.println("  Status: " + status);
```

**Output**:
```
=== Throw Statement Tests ===

Test 1: Basic throw
  Before throw
  Exception caught
  Status: 3
  Expected: 3
=== All Throw Tests Completed ===
```

**Result**: ✅ PASS - Exception caught and handled correctly

### Test 2: exc2.jav - Explicit Throw (Regression Test) ✅

**Code**:
```java
try {
    throw new Exception();
    status = 2;  // Should not execute
} catch (Exception e) {
    status = 3;
}
```

**Result**: ✅ PASS - Continues to work as before

### Test 3: Division by Zero Outside Try Block ✅

**Code**:
```java
int a = 1 / 0;  // No try block
```

**Expected Output**:
```
ERROR: Division by zero
```

**Result**: ✅ PASS - Program terminates with error message (same as before)

## Technical Design

### Exception Mechanism Flow

```
Runtime Error Detected (e.g., division by zero)
    |
    v
throw_runtime_exception() called
    |
    +-- in_try_block == 1 && catch_pc != NULL?
    |       |
    |       +-- YES: Jump to catch block
    |       |       - Set pc = catch_pc
    |       |       - Clear in_try_block flag
    |       |       - Return 0 (continue execution)
    |       |
    |       +-- NO: Terminate with error
    |               - Print error message
    |               - Return -1 (terminate)
    |
    v
Execution continues from catch block or terminates
```

### Integration with Existing Exception Handling

The implementation leverages existing exception handling infrastructure:

1. **OP_TRY_BEGIN**: Sets `ctx->in_try_block = 1` and `ctx->catch_pc`
2. **OP_TRY_END**: Clears `ctx->in_try_block = 0`
3. **OP_CATCH_BEGIN**: Marks catch block start
4. **OP_CATCH_END**: Clears `ctx->catch_pc = NULL`

Runtime errors now use the same mechanism as explicit `throw` statements.

## Benefits

1. **Consistent Exception Handling**: Runtime errors and explicit throws use the same mechanism
2. **Backward Compatible**: Code without try-catch continues to work as before
3. **Extensible**: Easy to add exception handling to other runtime errors
4. **Minimal Code Changes**: Only ~25 lines of new code
5. **No Performance Impact**: Only checked when error occurs

## Future Enhancements

This mechanism can be extended to handle:
- Array index out of bounds
- Null pointer exceptions
- Stack overflow exceptions
- Integer overflow/underflow
- Other runtime errors

## Files Modified

- `src/vm/interpreter.c` - Added exception helper and modified OP_DIV/OP_MOD

## Related Documents

- `RUNTIME_EXCEPTION_PLAN.md` - Implementation plan
- `tests/exc3.jav` - Test case for runtime exceptions
- `tests/exc2.jav` - Regression test for explicit throw

## Conclusion

The runtime exception handling implementation successfully allows try-catch blocks to catch runtime errors like division by zero. All tests pass, and the implementation is backward compatible with existing code.