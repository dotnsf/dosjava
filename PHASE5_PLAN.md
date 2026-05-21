# Phase 5: Long Type Support and Memory Model Migration

## Overview
Phase 5 focuses on implementing 32-bit long type support and migrating to Large Memory Model to enable practical use of arrays and Date class extensions.

## Date
Start: 2026-05-21

## Goals
1. Migrate to Large Memory Model for expanded memory capacity
2. Implement 32-bit long type support in VM and compiler
3. Enable long arrays for practical data processing
4. Extend Date class with proper getTime()/setTime() support

---

## Phase 5.1: Large Memory Model Migration

### Duration: 1-2 days

### Objectives
- Migrate from Medium (-mm) to Large (-ml) memory model
- Expand heap size from 2KB to 32KB
- Verify all existing functionality works correctly
- Establish baseline for long type implementation

### Tasks

#### 5.1.1: Makefile Configuration
**Files**: `Makefile`

**Changes**:
```makefile
# Change memory model flag
CFLAGS = -ml -0 -w4 -zq -os -s -i=C:\WATCOM\h
CXXFLAGS = -ml -0 -w4 -zq -os -s -i=C:\WATCOM\h

# Update comment
# Target: 16-bit DOS executable (Large memory model)
```

**Verification**:
- Build succeeds without errors
- All executables link correctly

#### 5.1.2: Memory Manager Configuration
**Files**: `src/vm/memory.h`, `src/vm/memory.c`

**Changes**:
```c
// memory.h
#define DEFAULT_HEAP_SIZE 32768  // 32KB (was 2KB)

// Update comments to reflect Large model
```

**Verification**:
- Memory manager initializes with 32KB heap
- Allocation/deallocation works correctly

#### 5.1.3: Build and Test
**Actions**:
1. Clean build: `wmake clean`
2. Full build: `.\build_all.bat`
3. Run existing tests in DOSBox-X:
   - arrays.jav
   - exc2.jav, exc3.jav, exc4.jav
   - file I/O tests
   - Date tests

**Success Criteria**:
- ✅ All builds succeed
- ✅ All existing tests pass
- ✅ No memory-related errors
- ✅ Executable sizes increase by ~20-30% (expected)

#### 5.1.4: Documentation
**Files**: `PHASE5_1_LARGE_MODEL_MIGRATION.md`

**Content**:
- Migration rationale
- Changes made
- Test results
- Performance comparison (optional)
- Known issues (if any)

---

## Phase 5.2: Long Type Implementation

### Duration: 8-12 days

### Objectives
- Implement 32-bit long type in VM
- Add long arithmetic operations
- Implement type conversions (int ↔ long)
- Add compiler support for long literals and operations

### Sub-phases

#### 5.2.1: VM Opcodes Definition (Day 1)
**Files**: `src/format/opcodes.h`

**New Opcodes**:
```c
/* Long Operations (0x90-0x9F) */
#define OP_PUSH_LONG     0x90  /* Push 32-bit long [high:2] [low:2] */
#define OP_LADD          0x91  /* Long addition */
#define OP_LSUB          0x92  /* Long subtraction */
#define OP_LMUL          0x93  /* Long multiplication */
#define OP_LDIV          0x94  /* Long division */
#define OP_LMOD          0x95  /* Long modulo */
#define OP_LNEG          0x96  /* Long negation */
#define OP_I2L           0x97  /* int to long conversion */
#define OP_L2I           0x98  /* long to int conversion */
#define OP_LCMP          0x99  /* Long comparison */
#define OP_LOAD_LONG     0x9A  /* Load long from locals [idx:1] */
#define OP_STORE_LONG    0x9B  /* Store long to locals [idx:1] */
```

**Documentation**:
- Add opcode descriptions to opcodes.h
- Document stack behavior for each operation

#### 5.2.2: Stack Operations (Day 2-3)
**Files**: `src/vm/interpreter.c`

**Helper Functions**:
```c
/* Push 32-bit long (high word first, then low word) */
static int stack_push_long(ExecutionContext* ctx, uint32_t value) {
    uint16_t high = (uint16_t)(value >> 16);
    uint16_t low = (uint16_t)(value & 0xFFFF);
    
    if (stack_push_shared(ctx, high) != 0) return -1;
    if (stack_push_shared(ctx, low) != 0) return -1;
    return 0;
}

/* Pop 32-bit long (low word first, then high word) */
static uint32_t stack_pop_long(ExecutionContext* ctx) {
    uint16_t low = stack_pop_shared(ctx);
    uint16_t high = stack_pop_shared(ctx);
    return ((uint32_t)high << 16) | low;
}
```

**Testing**:
- Unit test for push/pop operations
- Verify stack pointer management

#### 5.2.3: Arithmetic Operations (Day 4-6)
**Files**: `src/vm/interpreter.c`

**Implementation Order**:
1. OP_PUSH_LONG - Push long constant
2. OP_LADD - Addition with carry handling
3. OP_LSUB - Subtraction with borrow handling
4. OP_LMUL - 32-bit multiplication (use compiler intrinsics)
5. OP_LDIV - 32-bit division with zero check
6. OP_LMOD - 32-bit modulo with zero check
7. OP_LNEG - Negation (two's complement)

**Example Implementation**:
```c
case OP_LADD: {
    uint32_t value2 = stack_pop_long(ctx);
    uint32_t value1 = stack_pop_long(ctx);
    uint32_t result = value1 + value2;
    if (stack_push_long(ctx, result) != 0) {
        printf("ERROR: Stack overflow\n");
        return -1;
    }
    break;
}
```

**Testing**:
- Test each operation individually
- Test overflow/underflow behavior
- Test with maximum/minimum values

#### 5.2.4: Type Conversions (Day 7)
**Files**: `src/vm/interpreter.c`

**Operations**:
```c
case OP_I2L: {
    /* int to long: sign-extend 16-bit to 32-bit */
    uint16_t int_val = stack_pop_shared(ctx);
    int32_t long_val = (int32_t)(int16_t)int_val;
    if (stack_push_long(ctx, (uint32_t)long_val) != 0) {
        return -1;
    }
    break;
}

case OP_L2I: {
    /* long to int: truncate to 16-bit */
    uint32_t long_val = stack_pop_long(ctx);
    uint16_t int_val = (uint16_t)(long_val & 0xFFFF);
    if (stack_push_shared(ctx, int_val) != 0) {
        return -1;
    }
    break;
}
```

**Testing**:
- Test positive/negative conversions
- Test truncation behavior
- Test round-trip conversions

#### 5.2.5: Comparison Operations (Day 8)
**Files**: `src/vm/interpreter.c`

**Implementation**:
```c
case OP_LCMP: {
    /* Compare two longs, push -1, 0, or 1 */
    int32_t value2 = (int32_t)stack_pop_long(ctx);
    int32_t value1 = (int32_t)stack_pop_long(ctx);
    uint16_t result;
    
    if (value1 < value2) result = (uint16_t)-1;
    else if (value1 > value2) result = 1;
    else result = 0;
    
    if (stack_push_shared(ctx, result) != 0) {
        return -1;
    }
    break;
}
```

**Testing**:
- Test all comparison cases
- Test with equal values
- Test with extreme values

#### 5.2.6: Local Variable Operations (Day 9)
**Files**: `src/vm/interpreter.c`

**Implementation**:
```c
case OP_LOAD_LONG: {
    /* Load long from locals (2 consecutive slots) */
    uint8_t index = interpreter_read_u8(ctx);
    uint16_t high = load_local(ctx, index);
    uint16_t low = load_local(ctx, index + 1);
    uint32_t value = ((uint32_t)high << 16) | low;
    if (stack_push_long(ctx, value) != 0) {
        return -1;
    }
    break;
}

case OP_STORE_LONG: {
    /* Store long to locals (2 consecutive slots) */
    uint8_t index = interpreter_read_u8(ctx);
    uint32_t value = stack_pop_long(ctx);
    store_local(ctx, index, (uint16_t)(value >> 16));
    store_local(ctx, index + 1, (uint16_t)(value & 0xFFFF));
    break;
}
```

**Note**: Long variables occupy 2 local variable slots

**Testing**:
- Test load/store operations
- Verify slot allocation
- Test with multiple long variables

#### 5.2.7: Compiler Integration (Day 10-12)
**Files**: 
- `tools/compiler/lexer.c` - Add `long` keyword
- `tools/compiler/parser.c` - Parse long literals and declarations
- `tools/compiler/semantic.c` - Type checking for long
- `tools/compiler/codegen.c` - Generate long opcodes

**Lexer Changes**:
```c
/* Add to keyword table */
{"long", TOKEN_LONG},

/* Recognize long literals */
123L, 456l  // Suffix 'L' or 'l'
```

**Parser Changes**:
```c
/* Parse long type */
TYPE_LONG

/* Parse long literals */
long_literal: INTEGER_LITERAL 'L'
```

**Semantic Analysis**:
```c
/* Type checking */
- long + long → long
- long + int → long (implicit conversion)
- int + long → long (implicit conversion)
- long = int → OK (implicit conversion)
- int = long → Error (requires explicit cast)
```

**Code Generation**:
```c
/* Generate appropriate opcodes */
- long literal → OP_PUSH_LONG
- long arithmetic → OP_LADD, OP_LSUB, etc.
- Type conversion → OP_I2L, OP_L2I
- Local variables → OP_LOAD_LONG, OP_STORE_LONG (allocate 2 slots)
```

**Testing**:
- Compile simple long programs
- Test arithmetic expressions
- Test type conversions
- Test local variables

#### 5.2.8: Integration Testing (Day 12)
**Test Files**: `tests/long1.jav`, `tests/long2.jav`, etc.

**Test Cases**:
```java
// long1.jav - Basic operations
class LongTest1 {
    static void main() {
        long a = 100000L;
        long b = 200000L;
        long c = a + b;
        System.out.println(c);  // Should print 300000
    }
}

// long2.jav - Type conversions
class LongTest2 {
    static void main() {
        int i = 1000;
        long l = i;  // Implicit conversion
        int j = (int)l;  // Explicit cast
        System.out.println(j);
    }
}

// long3.jav - Arithmetic
class LongTest3 {
    static void main() {
        long a = 1000000L;
        long b = 2000000L;
        System.out.println(a + b);
        System.out.println(a - b);
        System.out.println(a * 2L);
        System.out.println(b / 2L);
    }
}
```

**Success Criteria**:
- ✅ All test programs compile
- ✅ All test programs execute correctly in DOSBox-X
- ✅ Results match expected output

#### 5.2.9: Documentation (Day 12)
**Files**: `PHASE5_2_LONG_TYPE_IMPLEMENTATION.md`

**Content**:
- Implementation details
- Opcode reference
- Stack layout for long operations
- Compiler integration notes
- Test results
- Known limitations

---

## Phase 5.3: Long Array Implementation

### Duration: 3-5 days

### Objectives
- Implement long[] array type
- Support array creation, access, and modification
- Enable practical data processing with large datasets

### Tasks

#### 5.3.1: Array Type Extension (Day 1)
**Files**: `src/runtime/object.h`, `src/runtime/array.c`

**Array Structure**:
```c
/* Array element types */
#define ARRAY_TYPE_INT   1
#define ARRAY_TYPE_LONG  2  /* New */

/* Array structure remains same, but element size varies */
typedef struct Array {
    Object base;
    uint16_t length;
    uint8_t element_type;
    void* elements;  /* Points to int[] or long[] */
} Array;
```

**New Functions**:
```c
/* Create long array */
Array* array_new_long(uint16_t length);

/* Get long element */
uint32_t array_get_long(Array* arr, uint16_t index);

/* Set long element */
void array_set_long(Array* arr, uint16_t index, uint32_t value);
```

#### 5.3.2: VM Opcode Support (Day 2)
**Files**: `src/format/opcodes.h`, `src/vm/interpreter.c`

**New Opcodes**:
```c
#define OP_NEW_LONG_ARRAY   0x9C  /* Create long array */
#define OP_LALOAD           0x9D  /* Load from long array */
#define OP_LASTORE          0x9E  /* Store to long array */
```

**Implementation**:
```c
case OP_NEW_LONG_ARRAY: {
    uint16_t length = stack_pop_shared(ctx);
    Array* arr = array_new_long(length);
    if (!arr) {
        printf("ERROR: Failed to create long array\n");
        return -1;
    }
    /* Push array reference */
    if (stack_push_shared(ctx, (uint16_t)arr) != 0) {
        return -1;
    }
    break;
}

case OP_LALOAD: {
    uint16_t index = stack_pop_shared(ctx);
    Array* arr = (Array*)stack_pop_shared(ctx);
    uint32_t value = array_get_long(arr, index);
    if (stack_push_long(ctx, value) != 0) {
        return -1;
    }
    break;
}

case OP_LASTORE: {
    uint32_t value = stack_pop_long(ctx);
    uint16_t index = stack_pop_shared(ctx);
    Array* arr = (Array*)stack_pop_shared(ctx);
    array_set_long(arr, index, value);
    break;
}
```

#### 5.3.3: Compiler Support (Day 3)
**Files**: `tools/compiler/parser.c`, `tools/compiler/codegen.c`

**Parser Changes**:
```c
/* Parse long array declaration */
long[] array = new long[100];
```

**Code Generation**:
```c
/* Generate opcodes for long array operations */
- new long[n] → OP_PUSH_INT n, OP_NEW_LONG_ARRAY
- array[i] → OP_LOAD_LOCAL, OP_PUSH_INT i, OP_LALOAD
- array[i] = value → OP_LOAD_LOCAL, OP_PUSH_INT i, value, OP_LASTORE
```

#### 5.3.4: Testing (Day 4-5)
**Test Files**: `tests/longarray1.jav`, `tests/longarray2.jav`

**Test Cases**:
```java
// longarray1.jav - Basic operations
class LongArrayTest1 {
    static void main() {
        long[] arr = new long[10];
        arr[0] = 1000000L;
        arr[1] = 2000000L;
        System.out.println(arr[0] + arr[1]);
    }
}

// longarray2.jav - Timestamp array
class LongArrayTest2 {
    static void main() {
        long[] timestamps = new long[100];
        for (int i = 0; i < 100; i++) {
            timestamps[i] = 1000000L + i;
        }
        System.out.println(timestamps[50]);
    }
}
```

**Success Criteria**:
- ✅ Long arrays can be created
- ✅ Elements can be read and written
- ✅ Large arrays (100+ elements) work correctly
- ✅ No memory leaks

#### 5.3.5: Documentation (Day 5)
**Files**: `PHASE5_3_LONG_ARRAY_IMPLEMENTATION.md`

---

## Phase 5.4: Date Class Extension

### Duration: 2-3 days

### Objectives
- Implement Date.getTime() returning long
- Implement Date.setTime(long) accepting long
- Enable practical date/time manipulation

### Tasks

#### 5.4.1: Native Method Implementation (Day 1)
**Files**: `src/vm/native.c`

**New Native Methods**:
```c
/* Date.getTime() - Returns long timestamp */
static int native_date_getTime(ExecutionContext* ctx, uint16_t* args, 
                                uint8_t arg_count, uint16_t* result) {
    Date* date = (Date*)args[0];
    uint32_t time = date_get_time(date);
    
    /* Push 32-bit long to stack */
    if (stack_push_long(ctx, time) != 0) {
        return -1;
    }
    return 0;
}

/* Date.setTime(long) - Sets timestamp */
static int native_date_setTime(ExecutionContext* ctx, uint16_t* args,
                                uint8_t arg_count, uint16_t* result) {
    Date* date = (Date*)args[0];
    /* Pop 32-bit long from stack */
    uint32_t time = stack_pop_long(ctx);
    
    date_set_time(date, time);
    return 0;
}
```

**Registration**:
```c
/* Register in native_register_builtins() */
native_register("Date", "getTime", "()J", 
                native_date_getTime, 0, NULL, NATIVE_RETURN_LONG);
native_register("Date", "setTime", "(J)V",
                native_date_setTime, 1, param_long, NATIVE_RETURN_VOID);
```

#### 5.4.2: Compiler Support (Day 2)
**Files**: `tools/compiler/semantic.c`, `tools/compiler/codegen.c`

**Type Checking**:
```java
Date d = new Date();
long time = d.getTime();  // Returns long
d.setTime(time + 1000L);  // Accepts long
```

**Code Generation**:
- Recognize long return type from getTime()
- Handle long parameter for setTime()

#### 5.4.3: Testing (Day 3)
**Test Files**: `tests/date_long.jav`

**Test Cases**:
```java
class DateLongTest {
    static void main() {
        Date d = new Date();
        
        // Get current time
        long time1 = d.getTime();
        System.out.println(time1);
        
        // Set time to specific value
        d.setTime(1000000000L);
        long time2 = d.getTime();
        System.out.println(time2);  // Should print 1000000000
        
        // Add 1 day (86400 seconds)
        d.setTime(time2 + 86400L);
        long time3 = d.getTime();
        System.out.println(time3);  // Should print 1000086400
    }
}
```

**Success Criteria**:
- ✅ getTime() returns correct long value
- ✅ setTime() accepts long parameter
- ✅ Date calculations work correctly
- ✅ No overflow issues

#### 5.4.4: Documentation (Day 3)
**Files**: `PHASE5_4_DATE_EXTENSION.md`

---

## Testing Strategy

### Unit Tests
- Each opcode tested individually
- Stack operations verified
- Type conversions validated

### Integration Tests
- Complete programs using long type
- Array operations with long elements
- Date manipulation with timestamps

### Regression Tests
- All existing tests must pass
- No performance degradation (acceptable)
- No memory leaks

### DOSBox-X Testing
- All tests run in actual DOS environment
- Verify 16-bit compatibility
- Check memory usage

---

## Success Criteria

### Phase 5.1: Large Memory Model
- ✅ Build succeeds with -ml flag
- ✅ Heap expanded to 32KB
- ✅ All existing tests pass
- ✅ No memory errors

### Phase 5.2: Long Type
- ✅ All long opcodes implemented
- ✅ Arithmetic operations work correctly
- ✅ Type conversions function properly
- ✅ Compiler generates correct code
- ✅ Test programs execute successfully

### Phase 5.3: Long Arrays
- ✅ Long arrays can be created
- ✅ Array access works correctly
- ✅ Large arrays (100+ elements) supported
- ✅ No memory leaks

### Phase 5.4: Date Extension
- ✅ getTime() returns long
- ✅ setTime() accepts long
- ✅ Date calculations work correctly
- ✅ Practical date manipulation possible

---

## Risk Management

### Identified Risks

**1. Large Model Compatibility**
- Risk: Existing code may have issues with Far pointers
- Mitigation: Thorough testing of all existing functionality
- Contingency: Keep Medium model as fallback

**2. 32-bit Arithmetic on 16-bit CPU**
- Risk: Performance issues or bugs in arithmetic
- Mitigation: Use compiler intrinsics, extensive testing
- Contingency: Simplify operations if needed

**3. Memory Consumption**
- Risk: Long arrays consume significant memory
- Mitigation: Monitor heap usage, implement limits
- Contingency: Reduce default heap size if needed

**4. Compiler Complexity**
- Risk: Type system becomes more complex
- Mitigation: Incremental implementation, thorough testing
- Contingency: Simplify type rules if needed

### Mitigation Strategies

1. **Incremental Development**
   - Implement one feature at a time
   - Test thoroughly before proceeding
   - Keep working versions

2. **Comprehensive Testing**
   - Unit tests for each component
   - Integration tests for complete features
   - Regression tests for existing functionality

3. **Documentation**
   - Document all changes
   - Maintain implementation notes
   - Record known issues

4. **Rollback Plan**
   - Keep git commits small and focused
   - Tag stable versions
   - Maintain ability to revert changes

---

## Timeline Summary

| Phase | Duration | Description |
|-------|----------|-------------|
| 5.1 | 1-2 days | Large Memory Model Migration |
| 5.2 | 8-12 days | Long Type Implementation |
| 5.3 | 3-5 days | Long Array Implementation |
| 5.4 | 2-3 days | Date Class Extension |
| **Total** | **14-22 days** | **Complete Phase 5** |

---

## Deliverables

### Code
- Modified Makefile with Large model
- Long type opcodes and VM implementation
- Compiler support for long type
- Long array implementation
- Extended Date class

### Documentation
- PHASE5_1_LARGE_MODEL_MIGRATION.md
- PHASE5_2_LONG_TYPE_IMPLEMENTATION.md
- PHASE5_3_LONG_ARRAY_IMPLEMENTATION.md
- PHASE5_4_DATE_EXTENSION.md
- PHASE5_SUMMARY.md (final summary)

### Tests
- Unit tests for long operations
- Integration tests for complete features
- Example programs demonstrating usage
- Performance benchmarks (optional)

---

## Next Steps After Phase 5

With long type support complete, future enhancements could include:

1. **Float/Double Support** - Floating-point arithmetic
2. **String Enhancements** - StringBuilder, formatting
3. **Collections** - ArrayList, HashMap (using long for capacity)
4. **File I/O Extensions** - Large file support (>64KB)
5. **Advanced Date/Time** - Calendar, TimeZone classes

---

## Conclusion

Phase 5 represents a significant enhancement to the dosjava project, enabling practical data processing and date/time manipulation. The migration to Large Memory Model provides the foundation for future growth, while long type support brings the VM closer to standard Java compatibility.

The incremental approach ensures stability and allows for thorough testing at each stage. Upon completion, dosjava will be capable of handling real-world applications with complex data structures and time-based calculations.