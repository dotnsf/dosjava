# Phase 6: Float Type Implementation Plan

## Overview
Implement 32-bit IEEE 754 floating-point support in dosjava using Watcom C's software FPU emulation. This phase adds float type support to the VM, compiler, and runtime, enabling basic floating-point arithmetic operations.

## Date
Start: 2026-05-22

## Goals
1. Implement float type in VM with software FPU emulation
2. Add float arithmetic operations (+, -, *, /, %)
3. Implement type conversions (int ↔ float, long ↔ float)
4. Add compiler support for float literals and operations
5. Create comprehensive test suite

## Technical Approach

### FPU Strategy: Software Emulation
- **Method**: Watcom C software FPU emulation
- **Compiler Flag**: `-fpc` (FPU calls)
- **Rationale**: Maximum compatibility with all 8086 systems
- **Trade-off**: Slower execution (10-100x vs hardware FPU) but universal compatibility

### Float Representation
```
Stack Layout: 2 words (32 bits total)
[high 16 bits] [low 16 bits]

IEEE 754 Single Precision:
- Sign: 1 bit
- Exponent: 8 bits (biased by 127)
- Mantissa: 23 bits
```

---

## Phase 6.1: Basic Float Support (Days 1-8)

### Duration: 8 days

### Objectives
- Implement float type in VM
- Add float arithmetic operations (+, -, *, /, %)
- Implement type conversions
- Add compiler support for float literals

---

### Day 1: VM Float Infrastructure

**Files**: `src/vm/interpreter.h`, `src/vm/stack.h`, `src/vm/stack.c`

#### Tasks

1. **Add float stack operations** (`stack.c`)
```c
/* Float stack operations (32-bit IEEE 754) */
int stack_push_float(InterpreterContext* ctx, float value);
float stack_pop_float(InterpreterContext* ctx);
float stack_peek_float(InterpreterContext* ctx, int offset);
```

2. **Update stack helper functions**
```c
/* Push float as 2 words [high, low] */
int stack_push_float(InterpreterContext* ctx, float value) {
    uint32_t bits;
    uint16_t high, low;
    
    /* Convert float to 32-bit representation */
    memcpy(&bits, &value, sizeof(float));
    
    high = (uint16_t)(bits >> 16);
    low = (uint16_t)(bits & 0xFFFF);
    
    /* Push high word first, then low word */
    if (stack_push_shared(ctx, high) != 0) return -1;
    if (stack_push_shared(ctx, low) != 0) return -1;
    
    return 0;
}

/* Pop float from 2 words [low, high] */
float stack_pop_float(InterpreterContext* ctx) {
    uint16_t low, high;
    uint32_t bits;
    float value;
    
    /* Pop low word first, then high word */
    low = stack_pop_shared(ctx);
    high = stack_pop_shared(ctx);
    
    /* Combine into 32-bit value */
    bits = ((uint32_t)high << 16) | low;
    
    /* Convert to float */
    memcpy(&value, &bits, sizeof(float));
    
    return value;
}
```

3. **Add float type checking**
```c
/* Check if value on stack is float (for debugging) */
int stack_is_float_compatible(InterpreterContext* ctx, int offset);
```

**Verification**:
- Stack operations compile without errors
- Unit tests for push/pop operations
- Verify IEEE 754 bit representation

---

### Day 2: Float Opcodes Definition

**Files**: `src/format/opcodes.h`, `src/format/opcodes.c`

#### Tasks

1. **Add float opcodes** (`opcodes.h`)
```c
/* Float arithmetic operations */
#define OP_FADD     0x62  /* Add two floats */
#define OP_FSUB     0x63  /* Subtract two floats */
#define OP_FMUL     0x64  /* Multiply two floats */
#define OP_FDIV     0x65  /* Divide two floats */
#define OP_FREM     0x66  /* Float remainder (modulo) */
#define OP_FNEG     0x67  /* Negate float */

/* Float comparison */
#define OP_FCMPG    0x68  /* Compare floats (greater) */
#define OP_FCMPL    0x69  /* Compare floats (less) */

/* Type conversions */
#define OP_I2F      0x6A  /* int to float */
#define OP_L2F      0x6B  /* long to float */
#define OP_F2I      0x6C  /* float to int */
#define OP_F2L      0x6D  /* float to long */

/* Float constants */
#define OP_FCONST_0 0x6E  /* Push float 0.0 */
#define OP_FCONST_1 0x6F  /* Push float 1.0 */
#define OP_FCONST_2 0x70  /* Push float 2.0 */
#define OP_FCONST   0x71  /* Push float constant [index:2] */
```

2. **Update opcode name table** (`opcodes.c`)
```c
const char* opcode_names[] = {
    /* ... existing opcodes ... */
    "OP_FADD",
    "OP_FSUB",
    "OP_FMUL",
    "OP_FDIV",
    "OP_FREM",
    "OP_FNEG",
    "OP_FCMPG",
    "OP_FCMPL",
    "OP_I2F",
    "OP_L2F",
    "OP_F2I",
    "OP_F2L",
    "OP_FCONST_0",
    "OP_FCONST_1",
    "OP_FCONST_2",
    "OP_FCONST",
    /* ... */
};
```

**Verification**:
- All opcodes defined with unique values
- No conflicts with existing opcodes
- Opcode names match definitions

---

### Day 3-4: Float Arithmetic Implementation

**Files**: `src/vm/interpreter.c`

#### Tasks

1. **Implement OP_FADD** (Day 3)
```c
case OP_FADD: {
    /* Add two floats: value1 + value2 */
    float value2, value1, result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    result = value1 + value2;
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FADD\n");
        return -1;
    }
    break;
}
```

2. **Implement OP_FSUB** (Day 3)
```c
case OP_FSUB: {
    /* Subtract two floats: value1 - value2 */
    float value2, value1, result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    result = value1 - value2;
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FSUB\n");
        return -1;
    }
    break;
}
```

3. **Implement OP_FMUL** (Day 3)
```c
case OP_FMUL: {
    /* Multiply two floats: value1 * value2 */
    float value2, value1, result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    result = value1 * value2;
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FMUL\n");
        return -1;
    }
    break;
}
```

4. **Implement OP_FDIV** (Day 4)
```c
case OP_FDIV: {
    /* Divide two floats: value1 / value2 */
    float value2, value1, result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    /* Check for division by zero */
    if (value2 == 0.0f) {
        printf("ERROR: Division by zero in OP_FDIV\n");
        return -1;
    }
    
    result = value1 / value2;
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FDIV\n");
        return -1;
    }
    break;
}
```

5. **Implement OP_FREM** (Day 4)
```c
case OP_FREM: {
    /* Float remainder: value1 % value2 */
    float value2, value1, result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    /* Check for division by zero */
    if (value2 == 0.0f) {
        printf("ERROR: Division by zero in OP_FREM\n");
        return -1;
    }
    
    /* Use fmod() from math.h */
    result = fmodf(value1, value2);
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FREM\n");
        return -1;
    }
    break;
}
```

6. **Implement OP_FNEG** (Day 4)
```c
case OP_FNEG: {
    /* Negate float: -value */
    float value, result;
    
    value = stack_pop_float(ctx);
    result = -value;
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow in OP_FNEG\n");
        return -1;
    }
    break;
}
```

**Verification**:
- Each operation compiles without errors
- Manual testing with simple float values
- Check for NaN and Infinity handling

---

### Day 5: Float Comparison and Constants

**Files**: `src/vm/interpreter.c`

#### Tasks

1. **Implement OP_FCMPG and OP_FCMPL**
```c
case OP_FCMPG: {
    /* Compare floats (NaN returns 1) */
    float value2, value1;
    int result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    if (isnan(value1) || isnan(value2)) {
        result = 1;  /* NaN comparison returns 1 for FCMPG */
    } else if (value1 > value2) {
        result = 1;
    } else if (value1 == value2) {
        result = 0;
    } else {
        result = -1;
    }
    
    if (stack_push_shared(ctx, (uint16_t)result) != 0) {
        printf("ERROR: Stack overflow in OP_FCMPG\n");
        return -1;
    }
    break;
}

case OP_FCMPL: {
    /* Compare floats (NaN returns -1) */
    float value2, value1;
    int result;
    
    value2 = stack_pop_float(ctx);
    value1 = stack_pop_float(ctx);
    
    if (isnan(value1) || isnan(value2)) {
        result = -1;  /* NaN comparison returns -1 for FCMPL */
    } else if (value1 > value2) {
        result = 1;
    } else if (value1 == value2) {
        result = 0;
    } else {
        result = -1;
    }
    
    if (stack_push_shared(ctx, (uint16_t)result) != 0) {
        printf("ERROR: Stack overflow in OP_FCMPL\n");
        return -1;
    }
    break;
}
```

2. **Implement float constants**
```c
case OP_FCONST_0: {
    if (stack_push_float(ctx, 0.0f) != 0) {
        printf("ERROR: Stack overflow in OP_FCONST_0\n");
        return -1;
    }
    break;
}

case OP_FCONST_1: {
    if (stack_push_float(ctx, 1.0f) != 0) {
        printf("ERROR: Stack overflow in OP_FCONST_1\n");
        return -1;
    }
    break;
}

case OP_FCONST_2: {
    if (stack_push_float(ctx, 2.0f) != 0) {
        printf("ERROR: Stack overflow in OP_FCONST_2\n");
        return -1;
    }
    break;
}

case OP_FCONST: {
    /* Load float constant from constant pool */
    uint16_t const_idx;
    float value;
    
    const_idx = interpreter_read_u16(ctx);
    
    /* Get float from constant pool */
    if (const_idx >= ctx->djc_file->header.constant_pool_count) {
        printf("ERROR: Invalid constant index: %u\n", const_idx);
        return -1;
    }
    
    /* Assume constant pool stores float as 32-bit value */
    value = ctx->djc_file->float_constants[const_idx];
    
    if (stack_push_float(ctx, value) != 0) {
        printf("ERROR: Stack overflow in OP_FCONST\n");
        return -1;
    }
    break;
}
```

**Verification**:
- Comparison operations return correct results
- NaN handling works correctly
- Constants push correct values

---

### Day 6: Type Conversions

**Files**: `src/vm/interpreter.c`

#### Tasks

1. **Implement OP_I2F (int to float)**
```c
case OP_I2F: {
    /* Convert int to float */
    int16_t int_value;
    float float_value;
    
    int_value = (int16_t)stack_pop_shared(ctx);
    float_value = (float)int_value;
    
    if (stack_push_float(ctx, float_value) != 0) {
        printf("ERROR: Stack overflow in OP_I2F\n");
        return -1;
    }
    break;
}
```

2. **Implement OP_L2F (long to float)**
```c
case OP_L2F: {
    /* Convert long to float */
    uint16_t low, high;
    int32_t long_value;
    float float_value;
    
    low = stack_pop_shared(ctx);
    high = stack_pop_shared(ctx);
    
    long_value = ((int32_t)high << 16) | low;
    float_value = (float)long_value;
    
    if (stack_push_float(ctx, float_value) != 0) {
        printf("ERROR: Stack overflow in OP_L2F\n");
        return -1;
    }
    break;
}
```

3. **Implement OP_F2I (float to int)**
```c
case OP_F2I: {
    /* Convert float to int (truncate towards zero) */
    float float_value;
    int16_t int_value;
    
    float_value = stack_pop_float(ctx);
    
    /* Handle special cases */
    if (isnan(float_value)) {
        int_value = 0;
    } else if (float_value >= 32767.0f) {
        int_value = 32767;
    } else if (float_value <= -32768.0f) {
        int_value = -32768;
    } else {
        int_value = (int16_t)float_value;
    }
    
    if (stack_push_shared(ctx, (uint16_t)int_value) != 0) {
        printf("ERROR: Stack overflow in OP_F2I\n");
        return -1;
    }
    break;
}
```

4. **Implement OP_F2L (float to long)**
```c
case OP_F2L: {
    /* Convert float to long (truncate towards zero) */
    float float_value;
    int32_t long_value;
    uint16_t high, low;
    
    float_value = stack_pop_float(ctx);
    
    /* Handle special cases */
    if (isnan(float_value)) {
        long_value = 0;
    } else if (float_value >= 2147483647.0f) {
        long_value = 2147483647;
    } else if (float_value <= -2147483648.0f) {
        long_value = -2147483648;
    } else {
        long_value = (int32_t)float_value;
    }
    
    /* Push as 2 words [high, low] */
    high = (uint16_t)(long_value >> 16);
    low = (uint16_t)(long_value & 0xFFFF);
    
    if (stack_push_shared(ctx, high) != 0) {
        printf("ERROR: Stack overflow in OP_F2L\n");
        return -1;
    }
    if (stack_push_shared(ctx, low) != 0) {
        printf("ERROR: Stack overflow in OP_F2L\n");
        return -1;
    }
    break;
}
```

**Verification**:
- Conversions produce correct results
- Special cases (NaN, overflow) handled correctly
- Truncation towards zero works as expected

---

### Day 7: Compiler Float Literal Support

**Files**: `tools/compiler/lexer.h`, `tools/compiler/lexer.c`, `tools/compiler/parser.c`

#### Tasks

1. **Add float token type** (`lexer.h`)
```c
typedef enum {
    /* ... existing tokens ... */
    TOKEN_FLOAT_LITERAL,
    /* ... */
} TokenType;
```

2. **Implement float literal lexing** (`lexer.c`)
```c
/* Recognize float literals: 3.14, 1.0f, 2.5F, 1e-5, 1.5e10f */
Token* lex_number(Lexer* lexer) {
    /* ... existing int/long logic ... */
    
    /* Check for decimal point */
    if (lexer->current == '.') {
        is_float = 1;
        advance(lexer);
        
        /* Read fractional part */
        while (isdigit(lexer->current)) {
            /* ... */
        }
    }
    
    /* Check for exponent (e or E) */
    if (lexer->current == 'e' || lexer->current == 'E') {
        is_float = 1;
        /* ... handle exponent ... */
    }
    
    /* Check for float suffix (f or F) */
    if (lexer->current == 'f' || lexer->current == 'F') {
        is_float = 1;
        advance(lexer);
    }
    
    if (is_float) {
        token->type = TOKEN_FLOAT_LITERAL;
        token->float_value = strtof(buffer, NULL);
    }
    /* ... */
}
```

3. **Add float literal parsing** (`parser.c`)
```c
ASTNode* parse_primary_expression(Parser* parser) {
    /* ... existing cases ... */
    
    if (parser->current->type == TOKEN_FLOAT_LITERAL) {
        ASTNode* node = ast_create_float_literal(parser->current->float_value);
        advance(parser);
        return node;
    }
    
    /* ... */
}
```

**Verification**:
- Float literals are correctly tokenized
- Various formats recognized (3.14, 1.0f, 1e-5)
- Parser creates correct AST nodes

---

### Day 8: Compiler Code Generation

**Files**: `tools/compiler/codegen.c`

#### Tasks

1. **Add float literal code generation**
```c
void codegen_float_literal(CodeGenerator* gen, ASTNode* node) {
    float value = node->float_value;
    
    /* Optimize common constants */
    if (value == 0.0f) {
        emit_opcode(gen, OP_FCONST_0);
    } else if (value == 1.0f) {
        emit_opcode(gen, OP_FCONST_1);
    } else if (value == 2.0f) {
        emit_opcode(gen, OP_FCONST_2);
    } else {
        /* Add to constant pool and emit OP_FCONST */
        uint16_t const_idx = add_float_constant(gen, value);
        emit_opcode(gen, OP_FCONST);
        emit_u16(gen, const_idx);
    }
}
```

2. **Add float arithmetic code generation**
```c
void codegen_binary_expression(CodeGenerator* gen, ASTNode* node) {
    /* Generate left operand */
    codegen_expression(gen, node->left);
    
    /* Generate right operand */
    codegen_expression(gen, node->right);
    
    /* Emit operation based on type */
    if (node->expr_type == TYPE_FLOAT) {
        switch (node->op) {
            case OP_ADD: emit_opcode(gen, OP_FADD); break;
            case OP_SUB: emit_opcode(gen, OP_FSUB); break;
            case OP_MUL: emit_opcode(gen, OP_FMUL); break;
            case OP_DIV: emit_opcode(gen, OP_FDIV); break;
            case OP_MOD: emit_opcode(gen, OP_FREM); break;
        }
    } else {
        /* ... existing int/long logic ... */
    }
}
```

3. **Add type conversion code generation**
```c
void codegen_cast_expression(CodeGenerator* gen, ASTNode* node) {
    /* Generate expression to cast */
    codegen_expression(gen, node->expr);
    
    /* Emit conversion opcode */
    if (node->from_type == TYPE_INT && node->to_type == TYPE_FLOAT) {
        emit_opcode(gen, OP_I2F);
    } else if (node->from_type == TYPE_LONG && node->to_type == TYPE_FLOAT) {
        emit_opcode(gen, OP_L2F);
    } else if (node->from_type == TYPE_FLOAT && node->to_type == TYPE_INT) {
        emit_opcode(gen, OP_F2I);
    } else if (node->from_type == TYPE_FLOAT && node->to_type == TYPE_LONG) {
        emit_opcode(gen, OP_F2L);
    }
    /* ... */
}
```

**Verification**:
- Float literals compile to correct opcodes
- Arithmetic operations generate correct bytecode
- Type conversions work correctly

---

### Day 8: Testing and Integration

**Files**: `tests/tfloat.jav`, `tests/tfconv.jav`

#### Tasks

1. **Create basic float test** (`tfloat.jav`)
```java
class tfloat {
    public static void main() {
        float a = 3.14f;
        float b = 2.0f;
        
        float sum = a + b;
        float diff = a - b;
        float prod = a * b;
        float quot = a / b;
        float rem = a % b;
        
        System.out.println("Sum: ");
        System.out.println(sum);
        System.out.println("Diff: ");
        System.out.println(diff);
        System.out.println("Prod: ");
        System.out.println(prod);
        System.out.println("Quot: ");
        System.out.println(quot);
        System.out.println("Rem: ");
        System.out.println(rem);
    }
}
```

2. **Create type conversion test** (`tfconv.jav`)
```java
class tfconv {
    public static void main() {
        int i = 42;
        long l = 1000000L;
        float f = 3.14f;
        
        // int to float
        float f1 = (float)i;
        System.out.println("int to float: ");
        System.out.println(f1);
        
        // long to float
        float f2 = (float)l;
        System.out.println("long to float: ");
        System.out.println(f2);
        
        // float to int
        int i1 = (int)f;
        System.out.println("float to int: ");
        System.out.println(i1);
        
        // float to long
        long l1 = (long)f;
        System.out.println("float to long: ");
        System.out.println(l1);
    }
}
```

3. **Update Makefile**
```makefile
# Add -fpc flag for software FPU emulation
CFLAGS = -ml -0 -w4 -zq -os -s -fpc -i=C:\WATCOM\h
```

4. **Build and test**
```batch
cd dosjava
.\build_all.bat
cd build\bin
djc.exe ..\..\tests\tfloat.jav
djvm.exe tfloat.djc
```

**Verification**:
- All tests compile without errors
- Float arithmetic produces correct results
- Type conversions work as expected
- No memory leaks or crashes

---

## Phase 6.2: Float Arrays (Days 9-12)

### Duration: 4 days

### Objectives
- Implement float[] array type
- Add array element access for floats
- Support multi-dimensional float arrays

---

### Day 9: Float Array Type Support

**Files**: `src/vm/memory.h`, `src/vm/memory.c`

#### Tasks

1. **Add float array allocation**
```c
/* Allocate float array (2 words per element) */
void* memory_alloc_float_array(int length) {
    size_t size = sizeof(ArrayHeader) + (length * sizeof(float));
    ArrayHeader* array = (ArrayHeader*)memory_alloc(size);
    
    if (array) {
        array->length = length;
        array->element_size = sizeof(float);  /* 4 bytes */
    }
    
    return array;
}
```

2. **Add float array access helpers**
```c
/* Get float array element */
float memory_get_float_array_element(void* array, int index) {
    ArrayHeader* header = (ArrayHeader*)array;
    float* data = (float*)((char*)array + sizeof(ArrayHeader));
    
    if (index < 0 || index >= header->length) {
        return 0.0f;  /* Out of bounds */
    }
    
    return data[index];
}

/* Set float array element */
void memory_set_float_array_element(void* array, int index, float value) {
    ArrayHeader* header = (ArrayHeader*)array;
    float* data = (float*)((char*)array + sizeof(ArrayHeader));
    
    if (index >= 0 && index < header->length) {
        data[index] = value;
    }
}
```

**Verification**:
- Float arrays allocate correctly
- Element access works properly
- Bounds checking functions correctly

---

### Day 10: Float Array Opcodes

**Files**: `src/format/opcodes.h`, `src/vm/interpreter.c`

#### Tasks

1. **Add float array opcodes** (`opcodes.h`)
```c
#define OP_NEWARRAY_FLOAT  0x72  /* Create float array [length:2] */
#define OP_FALOAD          0x73  /* Load float from array */
#define OP_FASTORE         0x74  /* Store float to array */
```

2. **Implement OP_NEWARRAY_FLOAT**
```c
case OP_NEWARRAY_FLOAT: {
    /* Create float array */
    uint16_t length;
    void* array;
    uint16_t array_handle;
    
    /* Pop array length */
    length = stack_pop_shared(ctx);
    
    /* Allocate array */
    array = memory_alloc_float_array(length);
    if (array == NULL) {
        printf("ERROR: Failed to allocate float array\n");
        return -1;
    }
    
    /* Allocate array handle */
    array_handle = memory_alloc_array_handle(array);
    if (array_handle == 0) {
        printf("ERROR: Failed to allocate array handle\n");
        return -1;
    }
    
    /* Push array handle */
    if (stack_push_shared(ctx, array_handle) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}
```

3. **Implement OP_FALOAD**
```c
case OP_FALOAD: {
    /* Load float from array */
    uint16_t index;
    uint16_t array_handle;
    void* array;
    float value;
    
    /* Pop index and array handle */
    index = stack_pop_shared(ctx);
    array_handle = stack_pop_shared(ctx);
    
    /* Resolve array handle */
    array = memory_get_array_ptr(array_handle);
    if (array == NULL) {
        printf("ERROR: Invalid array handle\n");
        return -1;
    }
    
    /* Get array element */
    value = memory_get_float_array_element(array, index);
    
    /* Push value */
    if (stack_push_float(ctx, value) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}
```

4. **Implement OP_FASTORE**
```c
case OP_FASTORE: {
    /* Store float to array */
    float value;
    uint16_t index;
    uint16_t array_handle;
    void* array;
    
    /* Pop value, index, and array handle */
    value = stack_pop_float(ctx);
    index = stack_pop_shared(ctx);
    array_handle = stack_pop_shared(ctx);
    
    /* Resolve array handle */
    array = memory_get_array_ptr(array_handle);
    if (array == NULL) {
        printf("ERROR: Invalid array handle\n");
        return -1;
    }
    
    /* Set array element */
    memory_set_float_array_element(array, index, value);
    break;
}
```

**Verification**:
- Array creation works correctly
- Element load/store operations function properly
- Bounds checking prevents crashes

---

### Day 11: Compiler Float Array Support

**Files**: `tools/compiler/parser.c`, `tools/compiler/codegen.c`

#### Tasks

1. **Add float array type parsing**
```c
Type* parse_type(Parser* parser) {
    /* ... existing logic ... */
    
    if (match(parser, TOKEN_FLOAT)) {
        Type* type = type_create(TYPE_FLOAT);
        
        /* Check for array dimensions */
        while (match(parser, TOKEN_LBRACKET)) {
            expect(parser, TOKEN_RBRACKET);
            type = type_create_array(type);
        }
        
        return type;
    }
    
    /* ... */
}
```

2. **Add float array creation codegen**
```c
void codegen_array_creation(CodeGenerator* gen, ASTNode* node) {
    /* Generate array length expression */
    codegen_expression(gen, node->length);
    
    /* Emit array creation opcode based on type */
    if (node->element_type == TYPE_FLOAT) {
        emit_opcode(gen, OP_NEWARRAY_FLOAT);
    } else {
        /* ... existing types ... */
    }
}
```

3. **Add float array access codegen**
```c
void codegen_array_access(CodeGenerator* gen, ASTNode* node) {
    /* Generate array reference */
    codegen_expression(gen, node->array);
    
    /* Generate index */
    codegen_expression(gen, node->index);
    
    /* Emit load/store based on type and context */
    if (node->element_type == TYPE_FLOAT) {
        if (node->is_store) {
            /* Generate value to store */
            codegen_expression(gen, node->value);
            emit_opcode(gen, OP_FASTORE);
        } else {
            emit_opcode(gen, OP_FALOAD);
        }
    }
    /* ... */
}
```

**Verification**:
- Float array declarations parse correctly
- Array creation generates correct bytecode
- Array access operations work properly

---

### Day 12: Float Array Testing

**Files**: `tests/tfarr.jav`

#### Tasks

1. **Create float array test**
```java
class tfarr {
    public static void main() {
        // Create float array
        float[] arr = new float[5];
        
        // Initialize array
        arr[0] = 1.1f;
        arr[1] = 2.2f;
        arr[2] = 3.3f;
        arr[3] = 4.4f;
        arr[4] = 5.5f;
        
        // Print array elements
        System.out.println("Float array:");
        int i = 0;
        while (i < 5) {
            System.out.println(arr[i]);
            i = i + 1;
        }
        
        // Calculate sum
        float sum = 0.0f;
        i = 0;
        while (i < 5) {
            sum = sum + arr[i];
            i = i + 1;
        }
        
        System.out.println("Sum: ");
        System.out.println(sum);
    }
}
```

2. **Test multi-dimensional arrays**
```java
class tfmat {
    public static void main() {
        // Create 2D float array (matrix)
        float[][] matrix = new float[3][3];
        
        // Initialize identity matrix
        matrix[0][0] = 1.0f;
        matrix[1][1] = 1.0f;
        matrix[2][2] = 1.0f;
        
        // Print matrix
        System.out.println("Identity matrix:");
        int i = 0;
        while (i < 3) {
            int j = 0;
            while (j < 3) {
                System.out.println(matrix[i][j]);
                j = j + 1;
            }
            i = i + 1;
        }
    }
}
```

**Verification**:
- Float arrays work correctly
- Multi-dimensional arrays function properly
- No memory leaks or corruption

---

## Phase 6.3: Math Class (Days 13-16)

### Duration: 4 days

### Objectives
- Implement Math class with common functions
- Add sqrt, abs, min, max
- Add trigonometric functions (sin, cos, tan)
- Add exponential functions (pow, exp, log)

---

### Day 13: Math Class Infrastructure

**Files**: `src/runtime/math.h`, `src/runtime/math.c`

#### Tasks

1. **Create Math class header** (`math.h`)
```c
#ifndef MATH_H
#define MATH_H

/* Math constants */
#define MATH_PI    3.14159265358979323846f
#define MATH_E     2.71828182845904523536f

/* Basic functions */
float math_abs(float x);
float math_min(float a, float b);
float math_max(float a, float b);
float math_sqrt(float x);

/* Trigonometric functions */
float math_sin(float x);
float math_cos(float x);
float math_tan(float x);

/* Exponential functions */
float math_pow(float base, float exp);
float math_exp(float x);
float math_log(float x);

#endif /* MATH_H */
```

2. **Implement basic functions** (`math.c`)
```c
#include "math.h"
#include <math.h>

float math_abs(float x) {
    return fabsf(x);
}

float math_min(float a, float b) {
    return (a < b) ? a : b;
}

float math_max(float a, float b) {
    return (a > b) ? a : b;
}

float math_sqrt(float x) {
    if (x < 0.0f) {
        return 0.0f;  /* Or NaN */
    }
    return sqrtf(x);
}
```

**Verification**:
- Functions compile without errors
- Basic math operations work correctly

---

### Day 14: Trigonometric Functions

**Files**: `src/runtime/math.c`

#### Tasks

1. **Implement trigonometric functions**
```c
float math_sin(float x) {
    return sinf(x);
}

float math_cos(float x) {
    return cosf(x);
}

float math_tan(float x) {
    return tanf(x);
}
```

2. **Add to VM native methods** (`interpreter.c`)
```c
else if (strcmp(method_name, "sin") == 0) {
    /* Math.sin(float) */
    float x, result;
    
    x = stack_pop_float(ctx);
    result = math_sin(x);
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}

/* Similar for cos, tan */
```

**Verification**:
- Trigonometric functions return correct values
- Edge cases handled properly

---

### Day 15: Exponential Functions

**Files**: `src/runtime/math.c`, `src/vm/interpreter.c`

#### Tasks

1. **Implement exponential functions**
```c
float math_pow(float base, float exp) {
    return powf(base, exp);
}

float math_exp(float x) {
    return expf(x);
}

float math_log(float x) {
    if (x <= 0.0f) {
        return 0.0f;  /* Or NaN */
    }
    return logf(x);
}
```

2. **Add to VM native methods**
```c
else if (strcmp(method_name, "pow") == 0) {
    /* Math.pow(float, float) */
    float exp, base, result;
    
    exp = stack_pop_float(ctx);
    base = stack_pop_float(ctx);
    result = math_pow(base, exp);
    
    if (stack_push_float(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}

/* Similar for exp, log */
```

**Verification**:
- Exponential functions work correctly
- Special cases handled properly

---

### Day 16: Math Class Testing

**Files**: `tests/tmath.jav`

#### Tasks

1. **Create comprehensive Math test**
```java
class tmath {
    public static void main() {
        // Test basic functions
        float x = -5.5f;
        System.out.println("abs(-5.5): ");
        System.out.println(Math.abs(x));
        
        System.out.println("min(3.0, 7.0): ");
        System.out.println(Math.min(3.0f, 7.0f));
        
        System.out.println("max(3.0, 7.0): ");
        System.out.println(Math.max(3.0f, 7.0f));
        
        System.out.println("sqrt(16.0): ");
        System.out.println(Math.sqrt(16.0f));
        
        // Test trigonometric functions
        float angle = 0.0f;  // 0 radians
        System.out.println("sin(0): ");
        System.out.println(Math.sin(angle));
        
        System.out.println("cos(0): ");
        System.out.println(Math.cos(angle));
        
        // Test exponential functions
        System.out.println("pow(2.0, 3.0): ");
        System.out.println(Math.pow(2.0f, 3.0f));
        
        System.out.println("exp(1.0): ");
        System.out.println(Math.exp(1.0f));
        
        System.out.println("log(2.718): ");
        System.out.println(Math.log(2.718f));
    }
}
```

**Verification**:
- All Math functions work correctly
- Results match expected values
- No crashes or errors

---

## Success Criteria

### Phase 6.1 (Days 1-8)
- ✅ Float arithmetic operations (+, -, *, /, %) work correctly
- ✅ Float comparison operations function properly
- ✅ Type conversions (int ↔ float, long ↔ float) work correctly
- ✅ Float literals compile and execute correctly
- ✅ All tests pass in DOSBox-X

### Phase 6.2 (Days 9-12)
- ✅ Float arrays can be created and accessed
- ✅ Multi-dimensional float arrays work correctly
- ✅ No memory leaks or corruption
- ✅ Array tests pass

### Phase 6.3 (Days 13-16)
- ✅ Math class functions work correctly
- ✅ Trigonometric functions return accurate results
- ✅ Exponential functions handle edge cases properly
- ✅ Math tests pass

## Build Configuration

### Makefile Changes
```makefile
# Add software FPU emulation flag
CFLAGS = -ml -0 -w4 -zq -os -s -fpc -i=C:\WATCOM\h
CXXFLAGS = -ml -0 -w4 -zq -os -s -fpc -i=C:\WATCOM\h

# Add math runtime object
RUNTIME_OBJS = ... build/obj/math.obj

# Math runtime
build/obj/math.obj: src/runtime/math.c src/runtime/math.h
	$(CC) $(CFLAGS) -fo=$@ src/runtime/math.c
```

## Documentation

### Files to Create/Update
1. `PHASE6_FLOAT_IMPLEMENTATION.md` - Implementation details
2. `PHASE6_FLOAT_TESTS.md` - Test results and coverage
3. `README.md` - Update with float type support
4. `TECHNICAL_SPEC.md` - Add float type specification

## Performance Considerations

### Expected Performance Impact
- **Float arithmetic**: 10-100x slower than int (software FPU)
- **Code size**: +20-25KB (FPU library + opcodes)
- **Memory usage**: No significant increase (32KB heap sufficient)

### Optimization Opportunities
1. Cache common float constants (0.0, 1.0, 2.0)
2. Use integer arithmetic where possible
3. Consider fixed-point alternative for performance-critical code

## Risk Mitigation

### Potential Issues
1. **Precision errors**: Document float precision limitations
2. **Performance**: Warn users about software FPU overhead
3. **NaN/Infinity**: Handle special values consistently
4. **Rounding**: Document truncation behavior in conversions

### Contingency Plans
1. If performance is unacceptable, implement fixed-point alternative
2. If code size exceeds limits, remove Math class (Phase 6.3)
3. If bugs are difficult to fix, simplify to basic operations only

## Timeline Summary

```
Phase 6.1: Days 1-8   (Basic float support)
Phase 6.2: Days 9-12  (Float arrays)
Phase 6.3: Days 13-16 (Math class)

Total: 16 days
```

## Next Steps After Phase 6

1. **Performance benchmarking**: Measure float vs int performance
2. **Fixed-point implementation**: Alternative for performance-critical code
3. **Double type**: 64-bit floating-point (if needed)
4. **String formatting**: Support for printing float values with precision
5. **Phase 7**: Network programming or GUI support