# Long Type Test Suite

## Overview

This test suite validates all Long type operations implemented in Phase 5.2.

**Important**: This test uses **bytecode format** (.djc), not Java source code, because the compiler does not yet support long type (Phase 5.2 Day 10-12 pending).

## Files

- **longtest.djc** - Bytecode test program (97 lines) - VM instructions, not Java source
- **tlong.bat** - Batch script to run the test (8.3 filename)

## How It Works

1. **Direct Execution**: `longtest.djc` contains VM bytecode instructions
2. **VM Execution**: `djvm.exe` directly interprets the bytecode
3. No compilation step needed (compiler doesn't support long yet)

## Bytecode Format

The `.djc` file contains VM instructions like:
```
PUSH_LONG 0x0000 0x0064  # Push 100L (high word, low word)
LADD                      # Long addition
PRINT_INT                 # Print result
```

## Test Coverage

### Test 1-2: Long Constants and Type Conversions
- Push 100L, 200L (PUSH_LONG)
- I2L: int 50 → long 50L
- L2I: long 1000L → int 1000

### Test 3-8: Arithmetic Operations
- **LADD**: 100L + 50L = 150L
- **LSUB**: 100L - 50L = 50L
- **LMUL**: 100L * 50L = 5000L
- **LDIV**: 100L / 50L = 2L
- **LMOD**: 100L % 50L = 0L
- **LNEG**: -(100L) = -100L

### Test 9-11: Comparison (LCMP)
- 100L < 200L → -1
- 200L > 100L → 1
- 100L == 100L → 0

### Test 12: Local Variables
- STORE_LONG to local[0-1]: 300L
- LOAD_LONG from local[0-1]
- STORE_LONG to local[2-3]: 500L
- Combined: 300L + 500L = 800L

## Expected Output

The test prints 24 lines of numbers (long values as high/low word pairs):

```
0          # Test 1: 100L high word
100        # Test 1: 100L low word
0          # Test 1: 200L high word
200        # Test 1: 200L low word
0          # Test 2: I2L(50) high word
50         # Test 2: I2L(50) low word
1000       # Test 2: L2I(1000L)
0          # Test 3: 150L high word
150        # Test 3: 150L low word (100L + 50L)
0          # Test 4: 50L high word
50         # Test 4: 50L low word (100L - 50L)
0          # Test 5: 5000L high word
5000       # Test 5: 5000L low word (100L * 50L)
0          # Test 6: 2L high word
2          # Test 6: 2L low word (100L / 50L)
0          # Test 7: 0L high word
0          # Test 7: 0L low word (100L % 50L)
-1         # Test 8: -100L high word (0xFFFF)
-100       # Test 8: -100L low word (-(100L))
-1         # Test 9: LCMP(100L, 200L) = -1
1          # Test 10: LCMP(200L, 100L) = 1
0          # Test 11: LCMP(100L, 100L) = 0
0          # Test 12: 300L high word (local var)
300        # Test 12: 300L low word
0          # Test 12: 800L high word (300L + 500L)
800        # Test 12: 800L low word
```

Total: 26 lines of output

## Running the Test

### Windows (Native)
```batch
cd dosjava\tests
tlong.bat
```

### DOSBox-X
```batch
cd \DOSJAVA\TESTS
tlong.bat
```

### Direct VM Execution
```batch
djvm.exe longtest.djc
```

## Interpreting Results

### Success Criteria
- All arithmetic operations produce correct results
- Type conversions work properly
- Local variable storage/retrieval works
- Comparison returns correct values (-1, 0, 1)
- No crashes or hangs

### Common Issues
1. **Incorrect high/low word order**: Check stack_push_long/pop_long
2. **Sign extension errors**: Verify I2L implementation
3. **Overflow**: Check 32-bit arithmetic operations
4. **Local variable corruption**: Verify STORE_LONG/LOAD_LONG indices

## Implementation Details

### Stack Layout
Long values are stored as two 16-bit words:
```
[high word]  <- stack top - 1
[low word]   <- stack top
```

### Local Variable Layout
Long values occupy two consecutive slots:
```
local[idx]     = high word
local[idx + 1] = low word
```

### Opcode Summary
| Opcode | Name | Description |
|--------|------|-------------|
| 0xA0 | PUSH_LONG | Push 32-bit constant |
| 0xA1 | LADD | Long addition |
| 0xA2 | LSUB | Long subtraction |
| 0xA3 | LMUL | Long multiplication |
| 0xA4 | LDIV | Long division |
| 0xA5 | LMOD | Long modulo |
| 0xA6 | LNEG | Long negation |
| 0xA7 | I2L | int to long |
| 0xA8 | L2I | long to int |
| 0xA9 | LCMP | Long comparison |
| 0xAA | LOAD_LONG | Load from locals |
| 0xAB | STORE_LONG | Store to locals |

## Next Steps

After successful testing:
1. **Phase 5.3**: Implement long arrays
2. **Phase 5.4**: Extend Date class with long timestamps
3. **Phase 6**: Implement remaining Java features

## Notes

- All operations are 32-bit signed integers
- Division by zero throws ArithmeticException
- Overflow wraps around (no exception)
- Compatible with 16-bit DOS memory model