# Phase 11: Runtime Exception Detection and Auto-Throw

## Overview

Phase 11 implements automatic detection and throwing of 5 critical runtime exceptions in the DOSJava VM. When these conditions are detected during execution, the VM will automatically throw the appropriate exception, allowing user code to catch and handle them.

## Target Exceptions

### High Priority (3 exceptions)
1. **NullPointerException** - Most common runtime error
2. **ArrayIndexOutOfBoundsException** - Array safety
3. **NumberFormatException** - String to number conversion errors

### Medium Priority (2 exceptions)
4. **IllegalArgumentException** - Parameter validation
5. **StringIndexOutOfBoundsException** - String safety

## Implementation Strategy

### Phase 11.1: Exception Type System Enhancement

**Goal**: Extend the exception system to support multiple exception types

**Tasks**:
1. Add exception type constants to VM
   - `EXCEPTION_TYPE_GENERIC` (0) - existing
   - `EXCEPTION_TYPE_NULL_POINTER` (1)
   - `EXCEPTION_TYPE_ARRAY_INDEX_OUT_OF_BOUNDS` (2)
   - `EXCEPTION_TYPE_NUMBER_FORMAT` (3)
   - `EXCEPTION_TYPE_ILLEGAL_ARGUMENT` (4)
   - `EXCEPTION_TYPE_STRING_INDEX_OUT_OF_BOUNDS` (5)

2. Extend Exception object structure
   - Add `type` field to store exception type
   - Keep existing `message` field for error details

3. Update exception creation in VM
   - Modify `vm_throw_exception()` to accept type parameter
   - Store type in Exception object

**Files to modify**:
- `vm/vm.h` - Add exception type constants
- `vm/vm.c` - Update `vm_throw_exception()` function
- `vm/exception.c` - Update exception object creation

### Phase 11.2: NullPointerException Detection

**Goal**: Automatically detect and throw NullPointerException

**Detection Points**:
1. **Method calls on null objects** (`INVOKEVIRTUAL`, `INVOKESPECIAL`)
   - Check if object reference is null before method call
   - Throw NullPointerException with message: "Cannot invoke method on null object"

2. **Field access on null objects** (`GETFIELD`, `PUTFIELD`)
   - Check if object reference is null before field access
   - Throw NullPointerException with message: "Cannot access field on null object"

3. **Array access on null arrays** (`IALOAD`, `IASTORE`, `FALOAD`, `FASTORE`, etc.)
   - Check if array reference is null before access
   - Throw NullPointerException with message: "Cannot access null array"

**Implementation**:
```c
// Example for INVOKEVIRTUAL
case OP_INVOKEVIRTUAL: {
    int obj_ref = stack[sp - arg_count];
    if (obj_ref == 0) {  // null check
        vm_throw_exception(vm, EXCEPTION_TYPE_NULL_POINTER, 
                          "Cannot invoke method on null object");
        goto exception_handler;
    }
    // ... existing method call logic
}
```

**Files to modify**:
- `vm/vm.c` - Add null checks to relevant opcodes

### Phase 11.3: ArrayIndexOutOfBoundsException Detection

**Goal**: Automatically detect and throw ArrayIndexOutOfBoundsException

**Detection Points**:
1. **Array element access** (`IALOAD`, `IASTORE`, `FALOAD`, `FASTORE`, `AALOAD`, `AASTORE`)
   - Check if index < 0 or index >= array.length
   - Throw ArrayIndexOutOfBoundsException with message: "Array index out of bounds: {index}"

**Implementation**:
```c
// Example for IALOAD
case OP_IALOAD: {
    int index = stack[sp--];
    int array_ref = stack[sp--];
    
    if (array_ref == 0) {
        vm_throw_exception(vm, EXCEPTION_TYPE_NULL_POINTER, 
                          "Cannot access null array");
        goto exception_handler;
    }
    
    int array_length = get_array_length(array_ref);
    if (index < 0 || index >= array_length) {
        char msg[64];
        sprintf(msg, "Array index out of bounds: %d", index);
        vm_throw_exception(vm, EXCEPTION_TYPE_ARRAY_INDEX_OUT_OF_BOUNDS, msg);
        goto exception_handler;
    }
    
    // ... existing array access logic
}
```

**Files to modify**:
- `vm/vm.c` - Add bounds checks to array access opcodes

### Phase 11.4: NumberFormatException Detection

**Goal**: Automatically detect and throw NumberFormatException

**Detection Points**:
1. **String to int conversion** (when `Integer.parseInt()` is implemented)
   - Check if string contains non-numeric characters
   - Check for overflow/underflow
   - Throw NumberFormatException with message: "Cannot parse '{string}' as integer"

2. **String to float conversion** (when `Float.parseFloat()` is implemented)
   - Check if string is not a valid float format
   - Throw NumberFormatException with message: "Cannot parse '{string}' as float"

**Note**: This requires implementing `Integer.parseInt()` and `Float.parseFloat()` methods first.

**Implementation approach**:
```c
// In Integer.parseInt() native implementation
int parse_int(const char* str) {
    // Validate string format
    if (!is_valid_integer_string(str)) {
        char msg[128];
        sprintf(msg, "Cannot parse '%s' as integer", str);
        vm_throw_exception(vm, EXCEPTION_TYPE_NUMBER_FORMAT, msg);
        return 0;
    }
    // ... parsing logic
}
```

**Files to modify**:
- `vm/native.c` - Add `Integer.parseInt()` and `Float.parseFloat()` implementations
- `compiler/codegen.c` - Add support for these method calls

### Phase 11.5: IllegalArgumentException Detection

**Goal**: Automatically detect and throw IllegalArgumentException

**Detection Points**:
1. **Array creation with negative size** (`NEWARRAY`, `ANEWARRAY`)
   - Check if size < 0
   - Throw IllegalArgumentException with message: "Negative array size: {size}"

2. **String operations with invalid arguments**
   - `String.substring(start, end)` where start > end or start < 0
   - Throw IllegalArgumentException with message: "Invalid substring range"

**Implementation**:
```c
// Example for NEWARRAY
case OP_NEWARRAY: {
    int size = stack[sp--];
    
    if (size < 0) {
        char msg[64];
        sprintf(msg, "Negative array size: %d", size);
        vm_throw_exception(vm, EXCEPTION_TYPE_ILLEGAL_ARGUMENT, msg);
        goto exception_handler;
    }
    
    // ... existing array creation logic
}
```

**Files to modify**:
- `vm/vm.c` - Add validation to array creation opcodes
- `vm/native.c` - Add validation to String methods

### Phase 11.6: StringIndexOutOfBoundsException Detection

**Goal**: Automatically detect and throw StringIndexOutOfBoundsException

**Detection Points**:
1. **String.charAt(index)**
   - Check if index < 0 or index >= string.length
   - Throw StringIndexOutOfBoundsException with message: "String index out of bounds: {index}"

2. **String.substring(start, end)**
   - Check if start < 0 or end > string.length
   - Throw StringIndexOutOfBoundsException with message: "String index out of bounds"

**Implementation**:
```c
// In String.charAt() native implementation
char string_char_at(String* str, int index) {
    if (index < 0 || index >= str->length) {
        char msg[64];
        sprintf(msg, "String index out of bounds: %d", index);
        vm_throw_exception(vm, EXCEPTION_TYPE_STRING_INDEX_OUT_OF_BOUNDS, msg);
        return 0;
    }
    return str->data[index];
}
```

**Files to modify**:
- `vm/native.c` - Add bounds checks to String methods

### Phase 11.7: Exception Message Retrieval

**Goal**: Allow user code to retrieve exception type and message

**Implementation**:
1. Add `Exception.getType()` method
   - Returns integer type code
   - Allows user to identify exception type

2. Add `Exception.getMessage()` method
   - Returns string message
   - Allows user to get error details

3. Update compiler to recognize these methods

**Example usage**:
```java
try {
    int[] arr = new int[5];
    arr[10] = 1;
} catch (Exception e) {
    int type = e.getType();
    String msg = e.getMessage();
    System.out.println("Exception type: " + type);
    System.out.println("Message: " + msg);
}
```

**Files to modify**:
- `vm/native.c` - Implement `getType()` and `getMessage()` methods
- `compiler/semantic.c` - Add method signatures
- `compiler/codegen.c` - Generate calls to these methods

### Phase 11.8: Testing

**Goal**: Comprehensive testing of all exception types

**Test files to create**:

1. **tests/excnull.jav** - NullPointerException tests
   - Null method call
   - Null field access
   - Null array access

2. **tests/excarr.jav** - ArrayIndexOutOfBoundsException tests
   - Negative index
   - Index >= length
   - Boundary cases

3. **tests/excnum.jav** - NumberFormatException tests
   - Invalid integer strings
   - Invalid float strings
   - Overflow cases

4. **tests/excarg.jav** - IllegalArgumentException tests
   - Negative array size
   - Invalid substring range

5. **tests/excstr.jav** - StringIndexOutOfBoundsException tests
   - charAt() with invalid index
   - substring() with invalid range

6. **tests/excall.jav** - Combined exception test
   - Tests all exception types in one program
   - Verifies exception type identification

**Test structure**:
```java
class ExcNull {
    public static void main(String[] args) {
        System.out.println("Testing NullPointerException...");
        
        // Test 1: Null method call
        try {
            String s = null;
            int len = s.length();
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Caught: " + e.getMessage());
        }
        
        // Test 2: Null array access
        try {
            int[] arr = null;
            int x = arr[0];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Caught: " + e.getMessage());
        }
        
        System.out.println("excnull.jav worked correctly.");
    }
}
```

### Phase 11.9: Documentation

**Goal**: Document the new exception system

**Documents to create/update**:

1. **PHASE11_COMPLETION.md**
   - Summary of implemented exceptions
   - Usage examples
   - Testing results

2. **Update TECHNICAL_SPEC.md**
   - Add exception type system documentation
   - Document auto-throw behavior
   - List all supported exceptions

3. **Update QUICKSTART.md**
   - Add exception handling examples
   - Show how to catch specific exception types

## Implementation Order

### Week 1: Foundation
- [ ] Phase 11.1: Exception Type System Enhancement
- [ ] Phase 11.2: NullPointerException Detection

### Week 2: Array and Number Safety
- [ ] Phase 11.3: ArrayIndexOutOfBoundsException Detection
- [ ] Phase 11.4: NumberFormatException Detection (basic)

### Week 3: Argument and String Safety
- [ ] Phase 11.5: IllegalArgumentException Detection
- [ ] Phase 11.6: StringIndexOutOfBoundsException Detection

### Week 4: Polish and Testing
- [ ] Phase 11.7: Exception Message Retrieval
- [ ] Phase 11.8: Comprehensive Testing
- [ ] Phase 11.9: Documentation

## Success Criteria

1. ✅ All 5 exception types are automatically detected and thrown
2. ✅ User code can catch and handle each exception type
3. ✅ Exception messages are clear and informative
4. ✅ `getType()` and `getMessage()` methods work correctly
5. ✅ All test files pass in DOSBox-X
6. ✅ No regression in existing functionality
7. ✅ Documentation is complete and accurate

## Technical Considerations

### Memory Management
- Exception objects are allocated on the heap
- Must ensure proper cleanup when exceptions are caught
- Avoid memory leaks in exception handling paths

### Performance
- Null checks and bounds checks add overhead
- Keep checks minimal and efficient
- Only check when necessary (e.g., don't check twice)

### Backward Compatibility
- Existing code without exception handling should still work
- Generic Exception catch blocks should catch all exception types
- Maintain compatibility with Phase 10 exception system

### DOS Constraints
- Limited memory for error messages (keep messages short)
- No dynamic string formatting (use fixed buffers)
- Efficient exception type identification (use integers, not strings)

## Risk Mitigation

### Risk 1: Performance Impact
- **Mitigation**: Profile code to ensure checks don't significantly slow execution
- **Fallback**: Make checks optional via compile flag if needed

### Risk 2: Memory Overhead
- **Mitigation**: Reuse exception objects when possible
- **Fallback**: Limit exception message length

### Risk 3: Complexity
- **Mitigation**: Implement one exception type at a time
- **Fallback**: Start with most critical (NullPointerException) and add others incrementally

## Future Enhancements (Post-Phase 11)

1. **ClassCastException** - For OOP type safety
2. **OutOfMemoryError** - For memory management
3. **StackOverflowError** - For recursion safety
4. **Custom exception types** - User-defined exceptions
5. **Exception chaining** - Caused-by relationships

## Notes

- This phase significantly improves code safety and debugging
- Auto-throw behavior matches standard Java semantics
- Exception type system is extensible for future exception types
- All changes maintain DOS compatibility and memory constraints