# Phase 5.3: Long Array Implementation Plan

## Overview

Extend the existing array implementation to support `long[]` arrays. Long arrays store 32-bit long values (2 words per element) instead of 16-bit int values (1 word per element).

## Current Array Implementation

### Memory Layout (int[])
```
[length:2 bytes][elem0:2 bytes][elem1:2 bytes]...[elemN:2 bytes]
Total size: (length + 1) * 2 bytes
```

### Existing Opcodes
- `OP_NEW_ARRAY` (0x60): Creates int[] or boolean[] array
- `OP_ARRAY_LENGTH` (0x61): Gets array length
- `OP_ARRAY_LOAD` (0x62): Loads 16-bit element
- `OP_ARRAY_STORE` (0x63): Stores 16-bit element

## Long Array Design

### Memory Layout (long[])
```
[length:2 bytes][elem0_high:2][elem0_low:2][elem1_high:2][elem1_low:2]...
Total size: (length * 2 + 1) * 2 bytes = (length * 4 + 2) bytes
```

Each long element occupies 2 consecutive 16-bit words: [high, low]

### New Opcodes (0xB0-0xB2)

1. **OP_NEW_LONG_ARRAY** (0xB0)
   - Creates a new long[] array
   - Stack: [size] → [array_ref]
   - Memory allocation: (size * 2 + 1) * 2 bytes
   - Initializes all elements to 0L

2. **OP_LARRAY_LOAD** (0xB1)
   - Loads a long element from array
   - Stack: [array_ref, index] → [high, low]
   - Pushes 2 words onto stack (high word first, then low word)
   - Performs bounds checking

3. **OP_LARRAY_STORE** (0xB2)
   - Stores a long element into array
   - Stack: [array_ref, index, high, low] → []
   - Pops 4 values: array_ref, index, high word, low word
   - Performs bounds checking

## Implementation Plan

### Day 1: Define Opcodes

**File**: `dosjava/src/format/opcodes.h`

Add new opcodes in the 0xB0-0xBF range:
```c
/* Long Array Operations (0xB0-0xBF) */
#define OP_NEW_LONG_ARRAY  0xB0  /* Create long array [size] -> [array_ref] */
#define OP_LARRAY_LOAD     0xB1  /* Load long from array [arr,idx] -> [high,low] */
#define OP_LARRAY_STORE    0xB2  /* Store long to array [arr,idx,high,low] -> [] */
```

Update `opcode_name()` and `opcode_length()` functions in `opcodes.c`.

### Day 2: VM Implementation

**File**: `dosjava/src/vm/interpreter.c`

#### OP_NEW_LONG_ARRAY Implementation
```c
case OP_NEW_LONG_ARRAY: {
    uint16_t size = stack_pop_shared(ctx);
    uint16_t total_size = (uint16_t)((size * 2 + 1) * sizeof(uint16_t));
    uint16_t* array_data = (uint16_t*)memory_alloc(total_size);
    
    if (array_data == NULL) {
        printf("ERROR: Out of memory allocating long array\n");
        return -1;
    }
    
    memset(array_data, 0, total_size);
    array_data[0] = size;  // Store element count (not word count)
    
    uint16_t array_handle = memory_alloc_array_handle(array_data);
    if (array_handle == 0) {
        printf("ERROR: Out of array handles\n");
        memory_free(array_data);
        return -1;
    }
    
    if (stack_push_shared(ctx, array_handle) != 0) {
        printf("ERROR: Stack overflow\n");
        memory_free_array_handle(array_handle);
        memory_free(array_data);
        return -1;
    }
    break;
}
```

#### OP_LARRAY_LOAD Implementation
```c
case OP_LARRAY_LOAD: {
    uint16_t index = stack_pop_shared(ctx);
    uint16_t array_handle = stack_pop_shared(ctx);
    uint16_t* array_data = (uint16_t*)memory_get_array_ptr(array_handle);
    
    if (array_data == NULL) {
        printf("ERROR: Null array reference (LARRAY_LOAD)\n");
        return -1;
    }
    
    uint16_t length = array_data[0];
    if (index >= length) {
        printf("ERROR: Array index out of bounds\n");
        return -1;
    }
    
    // Each long element uses 2 words: [high, low]
    uint16_t offset = 1 + (index * 2);
    uint16_t high = array_data[offset];
    uint16_t low = array_data[offset + 1];
    
    // Push high word first, then low word (stack grows upward)
    if (stack_push_shared(ctx, high) != 0 || 
        stack_push_shared(ctx, low) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}
```

#### OP_LARRAY_STORE Implementation
```c
case OP_LARRAY_STORE: {
    uint16_t low = stack_pop_shared(ctx);
    uint16_t high = stack_pop_shared(ctx);
    uint16_t index = stack_pop_shared(ctx);
    uint16_t array_handle = stack_pop_shared(ctx);
    uint16_t* array_data = (uint16_t*)memory_get_array_ptr(array_handle);
    
    if (array_data == NULL) {
        printf("ERROR: Null array reference (LARRAY_STORE)\n");
        return -1;
    }
    
    uint16_t length = array_data[0];
    if (index >= length) {
        printf("ERROR: Array index out of bounds\n");
        return -1;
    }
    
    // Each long element uses 2 words: [high, low]
    uint16_t offset = 1 + (index * 2);
    array_data[offset] = high;
    array_data[offset + 1] = low;
    
    // Push value back for assignment expression result
    if (stack_push_shared(ctx, high) != 0 || 
        stack_push_shared(ctx, low) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}
```

### Day 3: Compiler Type System

**File**: `dosjava/tools/compiler/ast.h`

The type system already supports arrays with element types. Need to ensure `long[]` is properly recognized:

```c
typedef struct {
    uint16_t kind;          // TYPE_ARRAY
    uint16_t element_type;  // TYPE_LONG for long[]
} TypeInfo;
```

**File**: `dosjava/tools/compiler/parser.c`

Update `parse_type()` to handle `long[]` syntax:
```c
if (parser->current.type == TOK_LONG) {
    type.kind = TYPE_LONG;
    parser_advance(parser);
    
    // Check for array syntax
    if (parser->current.type == TOK_LBRACKET) {
        parser_advance(parser);
        if (parser->current.type != TOK_RBRACKET) {
            parser_error(parser, "Expected ']'");
            return type;
        }
        parser_advance(parser);
        type.kind = TYPE_ARRAY;
        type.element_type = TYPE_LONG;
    }
}
```

### Day 4: Code Generation

**File**: `dosjava/tools/compiler/codegen.c`

#### Array Creation
Update `generate_new_array()` to detect long[] type:
```c
static void generate_new_array(CodeGenerator* codegen, ASTNode* node) {
    TypeInfo type = node->data.new_expr.type;
    
    // Generate size expression
    generate_expression(codegen, node->data.new_expr.size);
    
    // Emit appropriate opcode based on element type
    if (type.kind == TYPE_ARRAY) {
        if (type.element_type == TYPE_LONG) {
            emit_opcode(codegen, OP_NEW_LONG_ARRAY);
        } else {
            emit_opcode(codegen, OP_NEW_ARRAY);
            emit_u1(codegen, (uint8_t)type.element_type);
        }
    }
    
    update_stack(codegen, 1);  // Array reference on stack
}
```

#### Array Load
Update `generate_array_access()` to handle long arrays:
```c
static void generate_array_access(CodeGenerator* codegen, ASTNode* node) {
    ASTNode* array = node->data.array_access.array;
    ASTNode* index = node->data.array_access.index;
    TypeInfo array_type = get_expression_type(codegen, array);
    
    // Generate array reference
    generate_expression(codegen, array);
    
    // Generate index
    generate_expression(codegen, index);
    
    // Emit appropriate load opcode
    if (array_type.kind == TYPE_ARRAY && 
        array_type.element_type == TYPE_LONG) {
        emit_opcode(codegen, OP_LARRAY_LOAD);
        update_stack(codegen, 2);  // Pushes 2 words (high, low)
    } else {
        emit_opcode(codegen, OP_ARRAY_LOAD);
        update_stack(codegen, 1);  // Pushes 1 word
    }
}
```

#### Array Store
Update `generate_assignment()` for long array elements:
```c
if (target->type == NODE_ARRAY_ACCESS) {
    ASTNode* array = target->data.array_access.array;
    ASTNode* index = target->data.array_access.index;
    TypeInfo array_type = get_expression_type(codegen, array);
    
    // Generate: array, index, value
    generate_expression(codegen, array);
    generate_expression(codegen, index);
    generate_expression(codegen, value);
    
    // Emit appropriate store opcode
    if (array_type.kind == TYPE_ARRAY && 
        array_type.element_type == TYPE_LONG) {
        emit_opcode(codegen, OP_LARRAY_STORE);
        update_stack(codegen, -2);  // Consumes 4 words, pushes 2
    } else {
        emit_opcode(codegen, OP_ARRAY_STORE);
        update_stack(codegen, 0);  // Consumes 3 words, pushes 1
    }
}
```

### Day 5: Testing

**File**: `dosjava/tests/testlongarr.jav`

Create comprehensive test program:
```java
class TestLongArray {
    public static void main() {
        // Test 1: Create long array
        long[] arr = new long[5];
        System.println("Created long array of size 5");
        
        // Test 2: Store long values
        arr[0] = 1000000L;
        arr[1] = 2000000L;
        arr[2] = 3000000L;
        arr[3] = 4000000L;
        arr[4] = 5000000L;
        System.println("Stored 5 long values");
        
        // Test 3: Load and print long values
        long sum = 0L;
        int i = 0;
        while (i < 5) {
            long val = arr[i];
            System.print("arr[");
            System.printInt(i);
            System.print("] = ");
            System.printLong(val);
            System.println("");
            sum = sum + val;
            i = i + 1;
        }
        
        // Test 4: Print sum
        System.print("Sum = ");
        System.printLong(sum);
        System.println("");
        
        // Test 5: Array length
        int len = arr.length;
        System.print("Array length: ");
        System.printInt(len);
        System.println("");
        
        System.println("Long array test complete!");
    }
}
```

Expected output:
```
Created long array of size 5
Stored 5 long values
arr[0] = 1000000
arr[1] = 2000000
arr[2] = 3000000
arr[3] = 4000000
arr[4] = 5000000
Sum = 15000000
Array length: 5
Long array test complete!
```

## Technical Considerations

### Memory Usage
- Long arrays use 2x memory compared to int arrays
- A long[100] array uses: (100 * 2 + 1) * 2 = 402 bytes
- An int[100] array uses: (100 + 1) * 2 = 202 bytes

### Stack Usage
- Loading a long array element pushes 2 words
- Storing a long array element pops 4 words (array, index, high, low)
- Need to ensure stack has sufficient space

### Type Safety
- Compiler must prevent mixing int[] and long[] operations
- Type checking in semantic analyzer
- Runtime type information in array handle

### Compatibility
- Existing int[] arrays continue to work unchanged
- OP_ARRAY_LENGTH works for both int[] and long[]
- Different opcodes prevent accidental mixing

## Success Criteria

1. ✅ Compile `long[] arr = new long[10];` without errors
2. ✅ Store long values: `arr[0] = 1000000L;`
3. ✅ Load long values: `long x = arr[0];`
4. ✅ Get array length: `int len = arr.length;`
5. ✅ Perform arithmetic with array elements
6. ✅ Handle bounds checking correctly
7. ✅ Run successfully in DOSBox-X

## Files to Modify

1. `dosjava/src/format/opcodes.h` - Add new opcodes
2. `dosjava/src/format/opcodes.c` - Update opcode metadata
3. `dosjava/src/vm/interpreter.c` - Implement VM operations
4. `dosjava/tools/compiler/parser.c` - Parse long[] syntax
5. `dosjava/tools/compiler/semantic.c` - Type checking
6. `dosjava/tools/compiler/codegen.c` - Generate bytecode
7. `dosjava/tests/testlongarr.jav` - Test program

## Timeline

- **Day 1**: Opcode definitions (30 minutes)
- **Day 2**: VM implementation (2 hours)
- **Day 3**: Compiler type system (1 hour)
- **Day 4**: Code generation (2 hours)
- **Day 5**: Testing and debugging (2 hours)

**Total estimated time**: 7.5 hours

---

**Status**: Planning Complete - Ready to implement
**Next Step**: Day 1 - Define opcodes in opcodes.h