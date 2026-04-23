# DOS Java VM - Phase 3 & Phase 4 Implementation Plan

## Executive Summary

This document outlines the detailed implementation plan for Phase 3 (Remaining Opcodes) and Phase 4 (Runtime Library Integration) of the DOS Java Virtual Machine (djvm.exe). The plan is designed to incrementally add functionality while maintaining compatibility with the 16-bit DOS environment and Open Watcom C compiler constraints.

## Current Status

### ✅ Completed (Phase 1 & 2)
- Basic interpreter infrastructure
- Stack operations (PUSH, POP, DUP)
- Local variable operations (LOAD, STORE)
- Arithmetic operations (ADD, SUB, MUL, DIV, MOD, NEG)
- Comparison operations (EQ, NE, LT, LE, GT, GE)
- Control flow (GOTO, IF_TRUE, IF_FALSE, conditional jumps)
- Debug operations (PRINT_INT, PRINT_CHAR)
- Basic return operations (RETURN, RETURN_VALUE)
- Main VM program (djvm.exe) with debug mode

### 🔄 Current Implementation Gap Analysis

**Implemented Opcodes (40 opcodes):**
- Stack: NOP, PUSH_CONST, PUSH_INT, POP, DUP
- Locals: LOAD_LOCAL, STORE_LOCAL, LOAD_0-2, STORE_0-2, INC_LOCAL
- Arithmetic: ADD, SUB, MUL, DIV, MOD, NEG
- Comparison: CMP_EQ, CMP_NE, CMP_LT, CMP_LE, CMP_GT, CMP_GE
- Control: GOTO, IF_TRUE, IF_FALSE, IF_EQ, IF_NE, IF_LT, IF_LE, IF_GT, IF_GE
- Special: PRINT_INT, PRINT_CHAR, RETURN, RETURN_VALUE, HALT

**Missing Opcodes (12 opcodes):**
- Method Invocation: INVOKE_STATIC, INVOKE_VIRTUAL
- Object Operations: NEW, GET_FIELD, PUT_FIELD, GET_STATIC, PUT_STATIC
- Array Operations: NEW_ARRAY, ARRAY_LENGTH, ARRAY_LOAD, ARRAY_STORE
- Type Operations: INSTANCEOF, CHECKCAST

---

## Phase 3: Remaining Opcodes Implementation

### Overview
Phase 3 focuses on implementing the remaining opcodes needed for method calls, object operations, and array operations. This phase is divided into three sub-phases based on complexity and dependencies.

### Phase 3.1: Method Invocation (Priority: HIGH)

**Goal:** Enable static method calls to support modular code organization.

#### Implementation Tasks

1. **Add Call Stack Management**
   - File: `dosjava/src/vm/interpreter.h`
   - Add call frame structure:
     ```c
     typedef struct CallFrame {
         uint8_t* return_pc;        /* Return address */
         uint16_t* return_locals;   /* Caller's locals */
         uint16_t return_local_count;
         Stack* return_stack;       /* Caller's stack */
     } CallFrame;
     ```
   - Add call stack to ExecutionContext:
     ```c
     CallFrame call_frames[MAX_CALL_DEPTH];  /* Max 8 levels */
     uint8_t call_depth;
     ```

2. **Implement OP_INVOKE_STATIC (0x40)**
   - File: `dosjava/src/vm/interpreter.c`
   - Read 2-byte method index from bytecode
   - Look up method in DJC file's method table
   - Save current execution state to call frame
   - Initialize new execution context for called method
   - Set PC to method's code start
   - Handle parameter passing via stack

3. **Enhance OP_RETURN (0x42) and OP_RETURN_VALUE (0x43)**
   - Restore caller's execution state from call frame
   - Pop call frame from call stack
   - For RETURN_VALUE: preserve return value on stack
   - Handle main method return (call_depth == 0) as program exit

4. **Add Method Lookup Helper**
   - File: `dosjava/src/format/djc.c`
   - Function: `DJCMethod* djc_find_method(DJCFile* djc, uint16_t method_index)`
   - Validate method index bounds
   - Return pointer to method structure

#### Testing Strategy
- Create test program with static method calls
- Test nested method calls (2-3 levels deep)
- Test parameter passing and return values
- Test recursion (factorial, fibonacci)

#### Estimated Effort
- Implementation: 4-6 hours
- Testing: 2-3 hours
- **Total: 6-9 hours**

---

### Phase 3.2: Object Operations (Priority: MEDIUM)

**Goal:** Enable basic object creation and field access (simplified for DOS constraints).

#### Design Decisions

Given DOS memory constraints (64KB data segment), we'll implement a **simplified object model**:
- Objects are allocated from a fixed-size heap (e.g., 16KB)
- Object references are 16-bit handles (not pointers)
- Maximum 256 objects simultaneously
- No garbage collection (manual memory management)
- Fields stored as flat arrays (no inheritance support initially)

#### Implementation Tasks

1. **Add Object Heap Manager**
   - File: `dosjava/src/vm/object_heap.h` (new)
   - File: `dosjava/src/vm/object_heap.c` (new)
   - Structure:
     ```c
     typedef struct ObjectHeader {
         uint16_t class_id;      /* Class identifier */
         uint16_t field_count;   /* Number of fields */
         uint8_t in_use;         /* Allocation flag */
     } ObjectHeader;
     
     typedef struct ObjectHeap {
         uint8_t* heap_memory;   /* 16KB heap */
         uint16_t heap_size;
         uint16_t next_free;
         ObjectHeader objects[256];
     } ObjectHeap;
     ```
   - Functions:
     - `object_heap_init(ObjectHeap* heap, uint16_t size)`
     - `uint16_t object_heap_alloc(ObjectHeap* heap, uint16_t class_id, uint16_t field_count)`
     - `void object_heap_free(ObjectHeap* heap, uint16_t handle)`
     - `uint16_t* object_heap_get_fields(ObjectHeap* heap, uint16_t handle)`

2. **Implement OP_NEW (0x50)**
   - Read 2-byte class index
   - Look up class metadata (field count)
   - Allocate object from heap
   - Push object handle onto stack
   - Initialize fields to zero

3. **Implement OP_GET_FIELD (0x51) and OP_PUT_FIELD (0x52)**
   - GET_FIELD:
     - Pop object handle from stack
     - Read 2-byte field index
     - Load field value from object
     - Push value onto stack
   - PUT_FIELD:
     - Pop value from stack
     - Pop object handle from stack
     - Read 2-byte field index
     - Store value to object field

4. **Implement OP_GET_STATIC (0x53) and OP_PUT_STATIC (0x54)**
   - Add static field storage to DJC file structure
   - GET_STATIC: Read field index, load from static storage, push
   - PUT_STATIC: Read field index, pop value, store to static storage

5. **Update ExecutionContext**
   - Add object heap pointer to context
   - Initialize heap in `interpreter_init_context()`
   - Free heap in `interpreter_free_context()`

#### Testing Strategy
- Test object creation and field access
- Test multiple objects simultaneously
- Test static field access
- Test heap exhaustion handling

#### Estimated Effort
- Implementation: 6-8 hours
- Testing: 3-4 hours
- **Total: 9-12 hours**

---

### Phase 3.3: Array Operations (Priority: LOW)

**Goal:** Enable basic array creation and element access.

#### Design Decisions

Arrays will be implemented as special objects:
- Array header contains length and element type
- Elements stored contiguously after header
- Only primitive type arrays initially (int arrays)
- Maximum array size: 1024 elements (2KB for int arrays)

#### Implementation Tasks

1. **Extend Object Heap for Arrays**
   - Add array allocation function:
     ```c
     uint16_t object_heap_alloc_array(ObjectHeap* heap, uint8_t element_type, uint16_t length)
     ```
   - Array header structure:
     ```c
     typedef struct ArrayHeader {
         uint16_t length;
         uint8_t element_type;
     } ArrayHeader;
     ```

2. **Implement OP_NEW_ARRAY (0x60)**
   - Pop array length from stack
   - Read 1-byte element type
   - Allocate array from heap
   - Initialize elements to zero
   - Push array handle onto stack

3. **Implement OP_ARRAY_LENGTH (0x61)**
   - Pop array handle from stack
   - Read array length from header
   - Push length onto stack

4. **Implement OP_ARRAY_LOAD (0x62)**
   - Pop index from stack
   - Pop array handle from stack
   - Validate index bounds
   - Load element value
   - Push value onto stack

5. **Implement OP_ARRAY_STORE (0x63)**
   - Pop value from stack
   - Pop index from stack
   - Pop array handle from stack
   - Validate index bounds
   - Store value to array element

#### Testing Strategy
- Test array creation with various sizes
- Test array element access (load/store)
- Test array length operation
- Test bounds checking (index out of range)

#### Estimated Effort
- Implementation: 4-6 hours
- Testing: 2-3 hours
- **Total: 6-9 hours**

---

### Phase 3.4: Type Operations (Priority: LOW)

**Goal:** Enable runtime type checking (simplified implementation).

#### Implementation Tasks

1. **Implement OP_INSTANCEOF (0x70)**
   - Pop object handle from stack
   - Read 2-byte class index
   - Check if object's class matches or is subclass
   - Push 1 (true) or 0 (false) onto stack
   - For simplified implementation: exact class match only

2. **Implement OP_CHECKCAST (0x71)**
   - Pop object handle from stack
   - Read 2-byte class index
   - Verify object's class matches
   - If match: push object handle back
   - If no match: halt with error (no exception handling yet)

#### Testing Strategy
- Test instanceof with matching and non-matching types
- Test checkcast with valid and invalid casts

#### Estimated Effort
- Implementation: 2-3 hours
- Testing: 1-2 hours
- **Total: 3-5 hours**

---

## Phase 4: Runtime Library Integration

### Overview
Phase 4 integrates the existing runtime library (System, String, Integer) with the VM to enable console I/O and basic Java API functionality.

### Phase 4.1: System.out.println Integration (Priority: HIGH)

**Goal:** Enable `System.out.println()` for debugging and output.

#### Current Runtime Library Status
- ✅ `dosjava/src/runtime/system.h` - System class interface defined
- ✅ `dosjava/src/runtime/system.c` - Implementation exists
- ✅ Functions available:
  - `system_print_int(int16_t value)`
  - `system_println_int(int16_t value)`
  - `system_print_char(char c)`
  - `system_println_cstr(const char* cstr)`

#### Implementation Tasks

1. **Define Native Method Calling Convention**
   - File: `dosjava/src/vm/native.h` (new)
   - File: `dosjava/src/vm/native.c` (new)
   - Structure:
     ```c
     typedef int (*NativeMethod)(ExecutionContext* ctx);
     
     typedef struct NativeMethodEntry {
         const char* class_name;
         const char* method_name;
         const char* signature;
         NativeMethod handler;
     } NativeMethodEntry;
     ```

2. **Implement Native Method Registry**
   - Function: `native_register_method()`
   - Function: `native_find_method(const char* class_name, const char* method_name)`
   - Pre-register System.out methods at VM startup

3. **Create System.out Native Methods**
   - `native_system_out_println_int()`:
     - Pop int value from stack
     - Call `system_println_int(value)`
   - `native_system_out_println_string()`:
     - Pop string handle from stack
     - Get string data from heap
     - Call `system_println_cstr(string_data)`
   - `native_system_out_println()`:
     - Call `system_println_empty()`

4. **Enhance OP_INVOKE_STATIC for Native Methods**
   - Check if method is native (flag in method metadata)
   - If native:
     - Look up native handler
     - Call handler directly (no call frame needed)
     - Continue execution
   - If not native:
     - Use existing call frame mechanism

5. **Update Compiler to Mark Native Methods**
   - File: `dosjava/tools/compiler/codegen.c`
   - Detect `System.out.println()` calls
   - Mark method as native in DJC file
   - Store native method signature

#### Testing Strategy
- Test `System.out.println()` with no arguments
- Test `System.out.println(int)` with various values
- Test `System.out.println(String)` with string literals
- Test multiple println calls in sequence

#### Estimated Effort
- Implementation: 5-7 hours
- Testing: 2-3 hours
- **Total: 7-10 hours**

---

### Phase 4.2: String Operations (Priority: MEDIUM)

**Goal:** Enable basic string creation and manipulation.

#### Implementation Tasks

1. **Integrate String Runtime**
   - File: `dosjava/src/runtime/string.h` - Already exists
   - File: `dosjava/src/runtime/string.c` - Already exists
   - Functions available:
     - `string_create(const char* cstr)`
     - `string_length(String* str)`
     - `string_concat(String* s1, String* s2)`
     - `string_equals(String* s1, String* s2)`

2. **Add String Pool to VM**
   - Store string literals in DJC constant pool
   - Load strings at VM startup
   - Create String objects for literals
   - Map constant pool indices to string handles

3. **Implement String Native Methods**
   - `native_string_length()`: Return string length
   - `native_string_concat()`: Concatenate two strings
   - `native_string_equals()`: Compare strings

4. **Update OP_PUSH_CONST for Strings**
   - Detect string constants
   - Push string handle instead of raw value

#### Testing Strategy
- Test string literal creation
- Test string length operation
- Test string concatenation
- Test string comparison

#### Estimated Effort
- Implementation: 4-6 hours
- Testing: 2-3 hours
- **Total: 6-9 hours**

---

### Phase 4.3: Integer Wrapper Class (Priority: LOW)

**Goal:** Support Integer.parseInt() and Integer.toString().

#### Implementation Tasks

1. **Implement Integer Native Methods**
   - `native_integer_parse_int()`:
     - Pop string handle
     - Parse string to integer
     - Push integer value
   - `native_integer_to_string()`:
     - Pop integer value
     - Convert to string
     - Push string handle

2. **Add Number Parsing Utilities**
   - File: `dosjava/src/runtime/integer.c` - Already exists
   - Enhance with parsing functions

#### Testing Strategy
- Test parseInt with valid numbers
- Test parseInt with invalid input
- Test toString with various values

#### Estimated Effort
- Implementation: 3-4 hours
- Testing: 1-2 hours
- **Total: 4-6 hours**

---

## Implementation Timeline

### Week 1: Method Invocation
- Days 1-2: Call stack implementation
- Days 3-4: INVOKE_STATIC and enhanced RETURN
- Day 5: Testing and debugging

### Week 2: Object Operations
- Days 1-2: Object heap manager
- Days 3-4: NEW, GET_FIELD, PUT_FIELD
- Day 5: Static fields and testing

### Week 3: Runtime Integration
- Days 1-2: Native method infrastructure
- Days 3-4: System.out.println integration
- Day 5: String operations

### Week 4: Arrays and Polish
- Days 1-2: Array operations
- Days 3-4: Type operations
- Day 5: Integration testing and documentation

**Total Estimated Time: 4 weeks (80-100 hours)**

---

## Testing Strategy

### Unit Tests
Each opcode implementation should have dedicated unit tests:
- File: `dosjava/tests/vm/test_opcodes.c` (new)
- Test each opcode in isolation
- Test edge cases and error conditions

### Integration Tests
Test complete programs using multiple features:
- Method calls with parameters and returns
- Object creation and field access
- Array operations
- System.out.println output

### End-to-End Tests
Compile and run example programs:
- `examples/01_hello.java` - Basic program
- `examples/02_arithmetic.java` - Arithmetic operations
- `examples/05_fibonacci.java` - Loops and method calls
- New examples with objects and arrays

### Regression Tests
Ensure existing functionality still works:
- Re-run all Phase 1 & 2 tests
- Verify basic opcodes still function correctly

---

## Risk Mitigation

### Memory Constraints
**Risk:** 64KB data segment limit in DOS small memory model
**Mitigation:**
- Use fixed-size heaps (16KB for objects)
- Limit maximum objects (256) and arrays (1024 elements)
- Implement memory usage monitoring
- Add heap exhaustion error handling

### Call Stack Depth
**Risk:** Stack overflow with deep recursion
**Mitigation:**
- Limit call depth to 8 levels
- Check depth before each method call
- Return error on stack overflow

### Compiler Compatibility
**Risk:** Compiler may not generate expected opcodes
**Mitigation:**
- Update compiler in parallel with VM
- Add opcode generation tests
- Verify bytecode format with hex dumps

### Testing Coverage
**Risk:** Insufficient testing may miss edge cases
**Mitigation:**
- Write tests before implementation (TDD approach)
- Test both success and failure paths
- Use debug mode to trace execution

---

## Success Criteria

### Phase 3 Complete When:
- ✅ All 12 remaining opcodes implemented
- ✅ Method calls work with parameters and returns
- ✅ Objects can be created and fields accessed
- ✅ Arrays can be created and elements accessed
- ✅ All unit tests pass
- ✅ Integration tests pass

### Phase 4 Complete When:
- ✅ System.out.println() works for int and String
- ✅ String operations (length, concat, equals) work
- ✅ Native method calling convention established
- ✅ Example programs produce correct output
- ✅ All tests pass

---

## Next Steps

1. **Review and Approve Plan**
   - Discuss timeline and priorities
   - Adjust scope if needed
   - Confirm resource availability

2. **Set Up Development Environment**
   - Create test infrastructure
   - Set up debugging tools
   - Prepare example programs

3. **Begin Phase 3.1 Implementation**
   - Start with call stack management
   - Implement INVOKE_STATIC
   - Write and run tests

---

## Appendix A: Opcode Reference

### Currently Implemented (40 opcodes)

| Opcode | Hex | Description | Status |
|--------|-----|-------------|--------|
| NOP | 0x00 | No operation | ✅ |
| PUSH_CONST | 0x01 | Push constant from pool | ✅ |
| PUSH_INT | 0x02 | Push immediate integer | ✅ |
| POP | 0x03 | Pop and discard | ✅ |
| DUP | 0x04 | Duplicate top of stack | ✅ |
| LOAD_LOCAL | 0x10 | Load local variable | ✅ |
| STORE_LOCAL | 0x11 | Store to local variable | ✅ |
| LOAD_0 | 0x12 | Load local 0 | ✅ |
| LOAD_1 | 0x13 | Load local 1 | ✅ |
| LOAD_2 | 0x14 | Load local 2 | ✅ |
| STORE_0 | 0x15 | Store to local 0 | ✅ |
| STORE_1 | 0x16 | Store to local 1 | ✅ |
| STORE_2 | 0x17 | Store to local 2 | ✅ |
| ADD | 0x20 | Integer addition | ✅ |
| SUB | 0x21 | Integer subtraction | ✅ |
| MUL | 0x22 | Integer multiplication | ✅ |
| DIV | 0x23 | Integer division | ✅ |
| MOD | 0x24 | Integer modulo | ✅ |
| NEG | 0x25 | Negate | ✅ |
| INC_LOCAL | 0x26 | Increment local | ✅ |
| CMP_EQ | 0x30 | Compare equal | ✅ |
| CMP_NE | 0x31 | Compare not equal | ✅ |
| CMP_LT | 0x32 | Compare less than | ✅ |
| CMP_LE | 0x33 | Compare less or equal | ✅ |
| CMP_GT | 0x34 | Compare greater than | ✅ |
| CMP_GE | 0x35 | Compare greater or equal | ✅ |
| GOTO | 0x38 | Unconditional jump | ✅ |
| IF_TRUE | 0x39 | Jump if true | ✅ |
| IF_FALSE | 0x3A | Jump if false | ✅ |
| IF_EQ | 0x3B | Jump if equal | ✅ |
| IF_NE | 0x3C | Jump if not equal | ✅ |
| IF_LT | 0x3D | Jump if less than | ✅ |
| IF_LE | 0x3E | Jump if less or equal | ✅ |
| IF_GT | 0x3F | Jump if greater than | ✅ |
| IF_GE | 0x40 | Jump if greater or equal | ✅ |
| RETURN | 0x42 | Return void | ✅ |
| RETURN_VALUE | 0x43 | Return with value | ✅ |
| PRINT_INT | 0x80 | Print integer (debug) | ✅ |
| PRINT_CHAR | 0x81 | Print character (debug) | ✅ |
| HALT | 0xFF | Halt execution | ✅ |

### To Be Implemented (12 opcodes)

| Opcode | Hex | Description | Phase | Priority |
|--------|-----|-------------|-------|----------|
| INVOKE_STATIC | 0x40 | Static method call | 3.1 | HIGH |
| INVOKE_VIRTUAL | 0x41 | Virtual method call | 3.1 | MEDIUM |
| NEW | 0x50 | Create new object | 3.2 | MEDIUM |
| GET_FIELD | 0x51 | Read instance field | 3.2 | MEDIUM |
| PUT_FIELD | 0x52 | Write instance field | 3.2 | MEDIUM |
| GET_STATIC | 0x53 | Read static field | 3.2 | MEDIUM |
| PUT_STATIC | 0x54 | Write static field | 3.2 | MEDIUM |
| NEW_ARRAY | 0x60 | Create new array | 3.3 | LOW |
| ARRAY_LENGTH | 0x61 | Get array length | 3.3 | LOW |
| ARRAY_LOAD | 0x62 | Load from array | 3.3 | LOW |
| ARRAY_STORE | 0x63 | Store to array | 3.3 | LOW |
| INSTANCEOF | 0x70 | Check instance type | 3.4 | LOW |
| CHECKCAST | 0x71 | Cast to type | 3.4 | LOW |

---

## Appendix B: Memory Layout

### DOS Small Memory Model Constraints
- Code Segment: 64KB (shared with other segments)
- Data Segment: 64KB (all global/static data)
- Stack Segment: 64KB (call stack + local variables)

### Proposed Memory Allocation
```
Data Segment (64KB):
├── VM Core Structures (8KB)
│   ├── ExecutionContext
│   ├── Call Stack (8 frames × 32 bytes)
│   └── Native Method Registry
├── Object Heap (16KB)
│   ├── Object Headers (256 × 8 bytes = 2KB)
│   └── Object Data (14KB)
├── String Pool (8KB)
│   └── String literals from constant pool
├── Static Fields (4KB)
│   └── Class static variables
├── Operand Stack (4KB)
│   └── 2048 stack slots (16-bit values)
└── Available (24KB)
    └── DJC file data, buffers, etc.
```

---

## Appendix C: File Structure

### New Files to Create

```
dosjava/src/vm/
├── object_heap.h          # Object heap manager interface
├── object_heap.c          # Object heap implementation
├── native.h               # Native method interface
└── native.c               # Native method registry

dosjava/tests/vm/
├── test_opcodes.c         # Unit tests for opcodes
├── test_objects.c         # Object operation tests
├── test_arrays.c          # Array operation tests
└── test_native.c          # Native method tests

dosjava/examples/
├── 08_methods.java        # Method call examples
├── 09_objects.java        # Object creation examples
└── 10_arrays.java         # Array operation examples
```

### Files to Modify

```
dosjava/src/vm/
├── interpreter.h          # Add call stack, object heap
├── interpreter.c          # Implement new opcodes
└── djvm.c                 # Initialize new subsystems

dosjava/src/format/
├── djc.h                  # Add native method flags
└── djc.c                  # Add method lookup functions

dosjava/tools/compiler/
├── codegen.h              # Add native method support
└── codegen.c              # Generate native method calls

dosjava/Makefile           # Add new source files
```

---

*Document created: 2026-04-20*
*Author: Bob (Plan Mode)*
*Status: Draft for Review*