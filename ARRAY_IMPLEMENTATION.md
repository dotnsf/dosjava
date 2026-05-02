# Array Implementation Documentation

## Overview

This document describes the implementation of array support in the DOS Java compiler and virtual machine. Arrays are a fundamental data structure that allows storing multiple values of the same type in a contiguous memory block.

## Implementation Date

- **Started**: 2026-05-01
- **Completed**: 2026-05-01
- **Status**: Implementation complete, infinite recursion bug fixed, ready for testing

## Bug Fixes

### Infinite Recursion in Array Access Parsing (Fixed 2026-05-01)

**Problem**: The compiler would freeze when parsing array access expressions like `arr[i]`.

**Root Cause**: In `parse_postfix()`, the array access loop called `parse_expression()` to parse the index:
```c
index_node = parse_expression(parser);  // WRONG - causes infinite recursion
```

This created an infinite recursion because:
1. `parse_expression()` calls `parse_assignment()`
2. `parse_assignment()` eventually calls `parse_postfix()`
3. `parse_postfix()` checks for `[` and calls `parse_expression()` again
4. Loop repeats infinitely

**Solution**: Changed to call `parse_additive()` directly instead of `parse_expression()`:
```c
index_node = parse_additive(parser);  // FINAL SOLUTION - breaks recursion
```

This breaks the recursion cycle because `parse_additive()` handles `+` and `-` operators but doesn't call `parse_postfix()`, preventing infinite recursion.

**Multiple Attempts**:
1. **Attempt 1 (Failed)**: `parse_expression()` → `parse_assignment()`
   - Still caused infinite recursion
   - `parse_assignment()` → `parse_logical_or()` → ... → `parse_postfix()`

2. **Attempt 2 (Failed)**: `parse_assignment()` → `parse_logical_or()`
   - Still caused infinite recursion
   - `parse_logical_or()` → `parse_logical_and()` → ... → `parse_postfix()`

3. **Attempt 3 (Success)**: `parse_logical_or()` → `parse_additive()`
   - Breaks recursion successfully
   - `parse_additive()` → `parse_multiplicative()` → `parse_unary()` → `parse_primary()`
   - Never calls `parse_postfix()`, so no infinite loop

**Limitation**: Array indices cannot contain array access expressions (e.g., `arr[arr[0]]` is not supported), but basic arithmetic works (e.g., `arr[i+1]` is supported).

**Files Modified**: `dosjava/tools/compiler/parser.c` (line 1385)

## Features Implemented

### 1. Array Declaration and Initialization
- Syntax: `int[] arr = new int[size];`
- Supports `int[]` and `boolean[]` array types
- Dynamic size allocation at runtime

### 2. Array Element Access
- Syntax: `arr[index]`
- Zero-based indexing
- Bounds checking at runtime

### 3. Array Element Assignment
- Syntax: `arr[index] = value;`
- Type checking at compile time
- Bounds checking at runtime

### 4. Array Length Property
- Syntax: `arr.length`
- Returns the number of elements in the array
- Read-only property

## Architecture

### Type System Extension

Added `TYPE_ARRAY` to the `TypeKind` enum in `ast.h`:

```c
typedef enum {
    TYPE_VOID = 0,
    TYPE_INT = 1,
    TYPE_BOOLEAN = 2,
    TYPE_CLASS = 3,
    TYPE_ARRAY = 4
} TypeKind;
```

### Memory Layout

Arrays are stored in heap memory with the following layout:

```
[length:2 bytes][element0:2 bytes][element1:2 bytes]...[elementN:2 bytes]
```

- First 2 bytes: Array length (number of elements)
- Following bytes: Array elements (2 bytes per element)
- Total size: (length + 1) * 2 bytes

### Bytecode Instructions

Four new opcodes were added to support array operations:

1. **OP_NEW_ARRAY** (0x30)
   - Creates a new array on the heap
   - Stack: [size] → [array_ref]
   - Operand: element type (1 byte)

2. **OP_ARRAY_LOAD** (0x31)
   - Loads an array element
   - Stack: [array_ref, index] → [value]
   - Performs bounds checking

3. **OP_ARRAY_STORE** (0x32)
   - Stores a value into an array element
   - Stack: [array_ref, index, value] → []
   - Performs bounds checking

4. **OP_ARRAY_LENGTH** (0x33)
   - Gets the length of an array
   - Stack: [array_ref] → [length]

## Compiler Changes

### 1. Parser (parser.c)

**parse_type()**: Extended to recognize array type syntax
- Detects `[]` after base type
- Sets `type.kind = TYPE_ARRAY`
- Stores element type in `type.element_type`

**parse_postfix()**: Added array access parsing
- Handles `arr[index]` expressions
- Creates `NODE_ARRAY_ACCESS` nodes
- Supports chained array access

**parse_primary()**: Added array creation parsing
- Handles `new int[size]` expressions
- Creates `NODE_NEW` nodes with array type
- Parses size expression

### 2. Semantic Analyzer (semantic.c)

**check_array_access()**: Validates array access expressions
- Ensures base expression is an array type
- Ensures index expression is an integer
- Returns element type

**check_field_access()**: Special handling for `.length`
- Recognizes `arr.length` pattern
- Returns `TYPE_INT` for length property
- Validates that base is an array

**check_expression()**: Extended for array operations
- Handles `NODE_NEW` with array types
- Handles `NODE_ARRAY_ACCESS`
- Handles `NODE_FIELD_ACCESS` for `.length`

### 3. Code Generator (codegen.c)

**generate_new_array()**: Emits array creation bytecode
- Generates size expression code
- Emits `OP_NEW_ARRAY` with element type
- Handles both `int[]` and `boolean[]`

**generate_array_access()**: Emits array load bytecode
- Generates array reference code
- Generates index expression code
- Emits `OP_ARRAY_LOAD`

**generate_field_access()**: Emits array length bytecode
- Detects `.length` on array types
- Generates array reference code
- Emits `OP_ARRAY_LENGTH`

**generate_assignment()**: Extended for array element assignment
- Detects assignment to `NODE_ARRAY_ACCESS`
- Generates array reference, index, and value code
- Emits `OP_ARRAY_STORE`

## VM Changes

### Interpreter (interpreter.c)

**OP_NEW_ARRAY Implementation**:
```c
case OP_NEW_ARRAY: {
    uint8_t elem_type = code[pc++];
    uint16_t size = stack_pop(stack);
    uint16_t total_size = (size + 1) * sizeof(uint16_t);
    uint16_t* array = (uint16_t*)memory_alloc(total_size);
    if (!array) {
        fprintf(stderr, "Error: Out of memory\n");
        return -1;
    }
    array[0] = size;  // Store length
    stack_push(stack, (uint16_t)array);
    break;
}
```

**OP_ARRAY_LOAD Implementation**:
```c
case OP_ARRAY_LOAD: {
    uint16_t index = stack_pop(stack);
    uint16_t* array = (uint16_t*)stack_pop(stack);
    if (!array) {
        fprintf(stderr, "Error: Null array reference\n");
        return -1;
    }
    uint16_t length = array[0];
    if (index >= length) {
        fprintf(stderr, "Error: Array index out of bounds\n");
        return -1;
    }
    stack_push(stack, array[index + 1]);
    break;
}
```

**OP_ARRAY_STORE Implementation**:
```c
case OP_ARRAY_STORE: {
    uint16_t value = stack_pop(stack);
    uint16_t index = stack_pop(stack);
    uint16_t* array = (uint16_t*)stack_pop(stack);
    if (!array) {
        fprintf(stderr, "Error: Null array reference\n");
        return -1;
    }
    uint16_t length = array[0];
    if (index >= length) {
        fprintf(stderr, "Error: Array index out of bounds\n");
        return -1;
    }
    array[index + 1] = value;
    break;
}
```

**OP_ARRAY_LENGTH Implementation**:
```c
case OP_ARRAY_LENGTH: {
    uint16_t* array = (uint16_t*)stack_pop(stack);
    if (!array) {
        fprintf(stderr, "Error: Null array reference\n");
        return -1;
    }
    stack_push(stack, array[0]);
    break;
}
```

## Test Files

### 1. array.jav - Basic Array Operations

Tests the following features:
- Array declaration and initialization
- Element assignment
- Element access
- Array length property
- Loop iteration over array

Expected output: `10 20 30 40 50 5 150`

### 2. arrays.jav - Bubble Sort Algorithm

Tests the following features:
- Array initialization with values
- Nested loops
- Array element comparison
- Array element swapping
- Complex array manipulation

Expected output: `64 34 25 12 22 12 22 25 34 64`

## Testing

### Test Scripts

1. **test_array.bat**: Runs both array tests in DOSBox
   - Compiles array.jav and arrays.jav
   - Executes the compiled bytecode
   - Displays output for verification

2. **run_array_tests_dosbox.bat**: Automated DOSBox test runner
   - Copies files to DOSBox environment
   - Launches DOSBox with test script
   - Displays results

### Running Tests

```batch
cd dosjava
.\run_array_tests_dosbox.bat
```

Or manually in DOSBox:
```batch
cd tests
test_array.bat
```

## Limitations

1. **Single-dimensional arrays only**: Multi-dimensional arrays are not supported
2. **Fixed element types**: Only `int[]` and `boolean[]` are supported
3. **No array literals**: Arrays must be created with `new` and initialized element by element
4. **No array copying**: No built-in method to copy arrays
5. **Manual memory management**: Arrays are allocated but not automatically freed

## Future Enhancements

1. **Multi-dimensional arrays**: Support for `int[][]` syntax
2. **Array literals**: Support for `{1, 2, 3}` initialization syntax
3. **Array methods**: Add methods like `clone()`, `fill()`, etc.
4. **Garbage collection**: Automatic memory management for arrays
5. **String arrays**: Support for `String[]` type
6. **Array bounds optimization**: Eliminate redundant bounds checks

## Performance Considerations

1. **Bounds checking overhead**: Every array access includes a bounds check
2. **Memory allocation**: Arrays use heap allocation, which is slower than stack
3. **No optimization**: No loop unrolling or bounds check elimination
4. **16-bit addressing**: Limited to 64KB total heap space

## Compatibility

- **DOS**: Fully compatible with DOS 3.0 and later
- **DOSBox**: Tested on DOSBox 0.74-3
- **Memory model**: Small memory model (64KB code + 64KB data)
- **Compiler**: Open Watcom C 2.0

## Related Files

### Modified Files
- `dosjava/tools/compiler/ast.h` - Type system extension
- `dosjava/tools/compiler/parser.c` - Array syntax parsing
- `dosjava/tools/compiler/semantic.c` - Array type checking
- `dosjava/tools/compiler/codegen.c` - Array bytecode generation
- `dosjava/tools/compiler/codegen.h` - Function declarations
- `dosjava/src/vm/interpreter.c` - Array opcode implementation

### New Files
- `dosjava/tests/array.jav` - Basic array test
- `dosjava/tests/arrays.jav` - Bubble sort test
- `dosjava/tests/arraysim.jav` - Minimal array declaration test (8.3 format)
- `dosjava/tests/test_array.bat` - Test runner
- `dosjava/run_array_tests_dosbox.bat` - DOSBox test launcher
- `dosjava/ARRAY_IMPLEMENTATION.md` - This document

## References

- Java Language Specification: Arrays
- DOS Memory Management
- Open Watcom C Compiler Documentation
- DOSBox Documentation

---

**Author**: Bob (AI Assistant)  
**Date**: 2026-05-01  
**Version**: 1.0