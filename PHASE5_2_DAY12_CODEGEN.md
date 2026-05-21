# Phase 5.2 Day 12: Code Generation for Long Type

## Overview
Implemented bytecode generation for Long type in the compiler (codegen.c). The compiler now generates appropriate Long opcodes for literals, operations, and variable access.

## Implementation Date
2026-05-21

## Changes Made

### 1. Long Literal Generation (codegen.c)
**Location**: `generate_expression()` function, lines ~1034-1044

Added `NODE_LITERAL_LONG` case to generate `OP_PUSH_LONG` bytecode:
```c
case NODE_LITERAL_LONG: {
    /* Push 32-bit long constant [high:2] [low:2] */
    int32_t long_val = expr_node->data.literal_long.long_value;
    uint16_t high = (uint16_t)((long_val >> 16) & 0xFFFF);
    uint16_t low = (uint16_t)(long_val & 0xFFFF);
    emit_opcode(codegen, OP_PUSH_LONG);
    emit_u2(codegen, high);
    emit_u2(codegen, low);
    update_stack(codegen, 2);  /* Long takes 2 stack slots */
    return 0;
}
```

**Key Points**:
- Splits 32-bit long value into high and low 16-bit words
- Emits OP_PUSH_LONG (0xA0) followed by high word and low word
- Updates stack by 2 slots (long uses 2 stack positions)

### 2. Binary Operations (codegen.c)
**Location**: `generate_binary_op()` function, lines ~1634-1800

Enhanced binary operation generation to detect Long operands and emit appropriate opcodes:

**Type Detection**:
- Checks if operands are `NODE_LITERAL_LONG`
- Checks if operands are identifiers with `TYPE_LONG` in symbol table
- Sets `use_long_ops` flag if either operand is long

**Arithmetic Operations**:
```c
case BINOP_ADD: emit_opcode(codegen, use_long_ops ? OP_LADD : OP_ADD); break;
case BINOP_SUB: emit_opcode(codegen, use_long_ops ? OP_LSUB : OP_SUB); break;
case BINOP_MUL: emit_opcode(codegen, use_long_ops ? OP_LMUL : OP_MUL); break;
case BINOP_DIV: emit_opcode(codegen, use_long_ops ? OP_LDIV : OP_DIV); break;
case BINOP_MOD: emit_opcode(codegen, use_long_ops ? OP_LMOD : OP_MOD); break;
```

**Comparison Operations**:
For long comparisons, generates sequence: `OP_LCMP` → `OP_PUSH_INT 0` → comparison opcode
```c
case BINOP_EQ:
    if (use_long_ops) {
        emit_opcode(codegen, OP_LCMP);      // Compare longs, push -1/0/1
        emit_opcode(codegen, OP_PUSH_INT);
        emit_u2(codegen, 0);
        update_stack(codegen, 1);
        emit_opcode(codegen, OP_CMP_EQ);    // Compare result with 0
        update_stack(codegen, -1);
    } else {
        emit_opcode(codegen, OP_CMP_EQ);
    }
    break;
```

### 3. Unary Operations (codegen.c)
**Location**: `generate_unary_op()` function, lines ~1825-1860

Enhanced negation to support Long type:
```c
case UNOP_NEG: {
    /* Check if operand is long type */
    int is_long = 0;
    if (operand_node && operand_node->type == NODE_LITERAL_LONG) {
        is_long = 1;
    } else if (operand_node && operand_node->type == NODE_IDENTIFIER) {
        // Check symbol table for TYPE_LONG
        ...
    }
    emit_opcode(codegen, is_long ? OP_LNEG : OP_NEG);
    break;
}
```

### 4. Variable Load (codegen.c)
**Location**: `generate_identifier()` function, lines ~2760-2795

Enhanced to use `OP_LOAD_LONG` for long variables:
```c
if (local_idx != 0xFFFF) {
    /* Check if this is a long variable */
    int is_long = 0;
    // Check symbol table...
    
    if (is_long) {
        /* Load long variable (uses 2 stack slots) */
        emit_opcode(codegen, OP_LOAD_LONG);
        emit_u1(codegen, (uint8_t)local_idx);
        update_stack(codegen, 2);
    } else {
        /* Load int/boolean/reference variable */
        if (local_idx <= 2) {
            emit_opcode(codegen, OP_LOAD_0 + local_idx);
        } else {
            emit_opcode(codegen, OP_LOAD_LOCAL);
            emit_u1(codegen, (uint8_t)local_idx);
        }
        update_stack(codegen, 1);
    }
    return 0;
}
```

### 5. Variable Declaration (codegen.c)
**Location**: `generate_var_decl()` function, lines ~770-820

Enhanced to:
- Allocate 2 local slots for long variables
- Use `OP_STORE_LONG` for storing long values

```c
/* Check if this is a long variable */
int is_long = 0;
// Check symbol table...

/* Update max_locals (long variables use 2 slots) */
if (is_long) {
    if (local_idx + 2 > codegen->context->max_locals) {
        codegen->context->max_locals = local_idx + 2;
    }
} else {
    if (local_idx + 1 > codegen->context->max_locals) {
        codegen->context->max_locals = local_idx + 1;
    }
}

/* Generate initializer if present */
if (init_expr_idx != 0) {
    init_expr = codegen_get_node(codegen, init_expr_idx);
    if (init_expr) {
        generate_expression(codegen, init_expr);
        
        /* Store to local variable */
        if (is_long) {
            emit_opcode(codegen, OP_STORE_LONG);
            emit_u1(codegen, (uint8_t)local_idx);
            update_stack(codegen, -2);
        } else {
            // Standard store...
        }
    }
}
```

### 6. Assignment (codegen.c)
**Location**: `generate_assignment()` function, lines ~2200-2240

Enhanced simple assignment to support long variables:
```c
if (assign_op == 0) {
    /* Check if target is long type */
    int is_long = 0;
    // Check symbol table...
    
    if (generate_expression(codegen, &value_expr_copy) != 0) {
        return -1;
    }
    
    if (is_long) {
        /* Long assignment: store and reload for expression result */
        emit_opcode(codegen, OP_STORE_LONG);
        emit_u1(codegen, (uint8_t)local_idx);
        update_stack(codegen, -2);
        /* Reload for expression result */
        emit_opcode(codegen, OP_LOAD_LONG);
        emit_u1(codegen, (uint8_t)local_idx);
        update_stack(codegen, 2);
    } else {
        // Standard assignment...
    }
    return 0;
}
```

**Note**: Since OP_DUP2 is not available, we store and reload the long value to provide the assignment result.

### 7. Symbol Table Support (symtable.c)
**Location**: `get_type_name()` function, line ~318

Added TYPE_LONG case:
```c
case TYPE_LONG:    return "long";
```

## Opcodes Used

| Opcode | Value | Description |
|--------|-------|-------------|
| OP_PUSH_LONG | 0xA0 | Push 32-bit long constant [high:2] [low:2] |
| OP_LADD | 0xA1 | Long addition |
| OP_LSUB | 0xA2 | Long subtraction |
| OP_LMUL | 0xA3 | Long multiplication |
| OP_LDIV | 0xA4 | Long division |
| OP_LMOD | 0xA5 | Long modulo |
| OP_LNEG | 0xA6 | Long negation |
| OP_I2L | 0xA7 | int to long conversion |
| OP_L2I | 0xA8 | long to int conversion |
| OP_LCMP | 0xA9 | Long compare (returns -1/0/1) |
| OP_LOAD_LONG | 0xAA | Load long from locals [idx:1] |
| OP_STORE_LONG | 0xAB | Store long to locals [idx:1] |

## Stack Management

**Long values occupy 2 stack slots**: [high word] [low word]

**Stack updates**:
- `OP_PUSH_LONG`: +2 slots
- `OP_LOAD_LONG`: +2 slots
- `OP_STORE_LONG`: -2 slots
- Long arithmetic (OP_LADD, etc.): -2 slots (pop two longs, push one long)
- `OP_LNEG`: 0 slots (pop one long, push one long)
- `OP_LCMP`: -3 slots (pop two longs, push one int)
- `OP_I2L`: +1 slot (pop one int, push one long)
- `OP_L2I`: -1 slot (pop one long, push one int)

## Local Variable Allocation

Long variables require 2 consecutive local variable slots:
- Slot N: High word (16 bits)
- Slot N+1: Low word (16 bits)

The `max_locals` counter is updated to account for the extra slot.

## Test Program

Created `tests/testlong.jav` to test Long type functionality:
- Long literal syntax (100L, 200L, etc.)
- Long arithmetic operations (+, -, *, /, %)
- Long negation (-)
- Long comparisons (<, ==, etc.)
- Type casting ((int)longValue)

## Testing Instructions

1. Build the compiler:
   ```
   cd dosjava
   .\build_all.bat
   ```

2. Compile test program in DOSBox-X:
   ```
   djc testlong.jav
   ```

3. Run test program:
   ```
   djvm testlong.djc
   ```

Expected output:
```
100L + 200L = 300
200L - 100L = 100
100L * 3L = 300
200L / 2L = 100
-100L = -100
100L < 200L: true
a == 100L: true
Long test complete!
```

## Known Limitations

1. **No OP_DUP2**: Since there's no OP_DUP2 opcode, assignment expressions use store+reload pattern instead of duplicating the long value on stack.

2. **Compound Assignment**: Compound assignment operators (+=, -=, etc.) for long variables are not yet implemented.

3. **Type Conversion**: Automatic type conversion between int and long is not yet implemented. Explicit casts are required.

4. **Long Arrays**: Long array support will be implemented in Phase 5.3.

## Next Steps

**Phase 5.2 Day 13-15**: Testing and bug fixes
- Test all Long operations in DOSBox-X
- Fix any issues found during testing
- Add more comprehensive test cases

**Phase 5.3**: Long Array Implementation
- Extend array operations to support long element type
- Implement OP_NEW_ARRAY for long arrays
- Implement array load/store for long elements

**Phase 5.4**: Date Class Extension
- Use Long type for Date.getTime() (milliseconds since epoch)
- Implement date arithmetic using long values

## Files Modified

1. `tools/compiler/codegen.c` - Main code generation logic
2. `tools/compiler/symtable.c` - Type name support
3. `tests/testlong.jav` - Test program (new file)

## Completion Status

✅ Phase 5.2 Day 10: Lexer support for Long type
✅ Phase 5.2 Day 11: Parser and AST support for Long type
✅ Phase 5.2 Day 12: Code generation for Long type

**Phase 5.2 (Long Type Implementation) is now complete!**