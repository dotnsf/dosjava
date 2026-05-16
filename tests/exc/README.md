# Exception Handling Tests (tests/exc/)

This directory contains comprehensive tests for the dosjava exception handling implementation (Phase 3.5).

## Test Files

### exc1.jav (test_basic)
Basic exception handling functionality tests:
- **Test 1**: Try-catch with exception - Verifies basic exception catching
- **Test 2**: Try-finally without exception - Verifies finally execution without exceptions
- **Test 3**: Try-catch-finally with exception - Verifies complete exception flow

**Expected Output:**
```
=== Basic Exception Tests ===

Test 1: Try-catch with exception
  Before exception
  Exception caught
  Status: 2
  Expected: 2

Test 2: Try-finally without exception
  Try block
  Finally block
  Status: 1
  Expected: 1

Test 3: Try-catch-finally with exception
  Try block
  Catch block
  Finally block
  Status: 3
  Expected: 3

=== All Basic Tests Completed ===
```

### exc2.jav (test_throw)
Throw statement behavior tests:
- **Test 1**: Basic throw operation - Verifies exception is thrown and caught
- **Test 2**: Code after throw not executed - Verifies control flow after throw
- **Test 3**: Catch handles thrown exception - Verifies catch block receives exception

**Expected Output:**
```
=== Throw Statement Tests ===

Test 1: Basic throw
  Before throw
  Exception caught
  Status: 3
  Expected: 3

Test 2: Code after throw not executed
  Counter: 0
  Counter: 1
  Exception caught, counter: 1
  Final counter: 1
  Expected: 1

Test 3: Catch handles exception
  Throwing exception
  Handling exception
  Result: 20
  Expected: 20

=== All Throw Tests Completed ===
```

### exc3.jav (test_nested)
Nested try-catch block tests:
- **Test 1**: Basic nested try-catch - Verifies independent operation of nested blocks
- **Test 2**: Inner exception caught - Verifies inner catch handles inner exceptions
- **Test 3**: Outer exception caught - Verifies outer catch handles outer exceptions
- **Test 4**: Both levels with finally - Verifies both finally blocks execute

**Expected Output:**
```
=== Nested Try-Catch Tests ===

Test 1: Basic nested try-catch
  Outer try
  Inner try
  After inner block
  Outer status: 2
  Inner status: 1
  Expected: outer=2, inner=1

Test 2: Inner exception caught
  Outer try
  Inner try
  Inner catch
  After inner block
  Result: 50
  Expected: 50

Test 3: Outer exception caught
  Outer try
  Inner try
  After inner block
  Outer catch
  Result: 60
  Expected: 60

Test 4: Nested with finally
  Outer try
  Inner try
  Inner catch
  Inner finally
  After inner block
  Outer finally
  Outer finally: 1
  Inner finally: 1
  Expected: both=1

=== All Nested Tests Completed ===
```

### exc4.jav (test_finally)
Finally block execution guarantee tests:
- **Test 1**: Finally without exception - Verifies finally executes without exceptions
- **Test 2**: Finally with exception - Verifies finally executes with exceptions
- **Test 3**: Finally always executes - Verifies finally always executes
- **Test 4**: Finally after catch - Verifies execution order: try → catch → finally

**Expected Output:**
```
=== Finally Block Tests ===

Test 1: Finally without exception
  Try block
  Finally block
  Try executed: 1
  Finally executed: 1
  Expected: both=1

Test 2: Finally with exception
  Try block
  Catch block
  Finally block
  Try executed: 1
  Catch executed: 1
  Finally executed: 1
  Expected: all=1

Test 3: Finally always executes
  Without exception: 2
  With exception: 5
  Expected: 2, then 5

Test 4: Finally after catch
  Try: setting value to 10
  Catch: setting value to 20
  Finally: setting value to 30
  Final value: 30
  Expected: 30

=== All Finally Tests Completed ===
```

### exc5.jav (test_edge)
Edge cases and error condition tests:
- **Test 1**: Empty try block - Verifies empty try blocks work
- **Test 2**: Empty catch block - Verifies empty catch blocks work
- **Test 3**: Empty finally block - Verifies empty finally blocks work
- **Test 4**: Multiple statements after throw - Verifies none execute after throw
- **Test 5**: Exception in catch block - Verifies exception propagation from catch
- **Test 6**: Exception in finally block - Verifies exception propagation from finally

**Expected Output:**
```
=== Edge Case Tests ===

Test 1: Empty try block
  Finally executed
  Status: 1
  Expected: 1

Test 2: Empty catch block
  Before throw
  After catch
  Status: 2
  Expected: 2

Test 3: Empty finally block
  Try block
  After finally
  Status: 2
  Expected: 2

Test 4: Multiple statements after throw
  Counter: 0
  Exception caught
  Final counter: 0
  Expected: 0

Test 5: Exception in catch block
  Inner try
  Inner catch
  Outer catch
  Status: 4
  Expected: 4

Test 6: Exception in finally block
  Inner try
  Inner finally
  Outer catch
  Status: 4
  Expected: 4

=== All Edge Case Tests Completed ===
```

## Running Tests

### On Windows (Cross-compile)

1. Compile the test files:
```batch
cd dosjava
djc tests\exc\exc1.jav
djc tests\exc\exc2.jav
djc tests\exc\exc3.jav
djc tests\exc\exc4.jav
djc tests\exc\exc5.jav
```

2. Copy the compiled `.class` files to DOSBox-X

### In DOSBox-X

Run each test:
```
djvm exc1
djvm exc2
djvm exc3
djvm exc4
djvm exc5
```

## Test Coverage

These tests verify:
- ✓ Basic try-catch-finally syntax
- ✓ Exception throwing with `throw` statement
- ✓ Exception catching and handling
- ✓ Finally block execution guarantees
- ✓ Nested exception handling
- ✓ Control flow after exceptions
- ✓ Edge cases (empty blocks, exceptions in catch/finally)

## Implementation Details

### Bytecode Operations
- `OP_TRY_BEGIN (0x90)`: Marks start of try block, contains catch offset
- `OP_TRY_END (0x91)`: Marks end of try block
- `OP_CATCH_BEGIN (0x92)`: Marks start of catch block
- `OP_CATCH_END (0x93)`: Marks end of catch block
- `OP_FINALLY_BEGIN (0x94)`: Marks start of finally block
- `OP_FINALLY_END (0x95)`: Marks end of finally block
- `OP_THROW (0x96)`: Throws an exception

### Exception Flow
1. `throw` statement executed → VM jumps to catch_pc
2. Catch block executes (if present)
3. Finally block executes (if present)
4. Execution continues after try-catch-finally

## Known Limitations

1. **Single Exception Type**: Currently only supports generic `Exception` class
2. **No Exception Messages**: Exception objects don't carry messages yet
3. **No Stack Traces**: Stack trace information not implemented
4. **Simple Propagation**: Unhandled exceptions terminate the program

## Future Enhancements

- Multiple exception types (IOException, RuntimeException, etc.)
- Exception messages and constructors
- Stack trace generation
- Exception chaining (cause)
- Multi-catch blocks
- Try-with-resources

## Related Files

- `dosjava/tools/compiler/parser.c` - Exception syntax parsing
- `dosjava/tools/compiler/codegen.c` - Exception bytecode generation
- `dosjava/src/vm/interpreter.c` - Exception runtime handling
- `dosjava/PHASE3_5_TASKS.md` - Implementation plan and progress