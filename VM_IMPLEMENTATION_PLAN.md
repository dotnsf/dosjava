# DOS Java VM (djvm.exe) Implementation Plan

## Overview

This document outlines the development plan for implementing the DOS Java Virtual Machine (djvm.exe), which will execute .djc bytecode files compiled by the djc compiler.

## Current Status

### Completed Components
- ✅ Memory manager (`src/vm/memory.c`)
- ✅ Stack implementation (`src/vm/stack.c`)
- ✅ Partial interpreter (`src/vm/interpreter.c`)
- ✅ DJC file format (`src/format/djc.c`)
- ✅ Opcode definitions (`src/format/opcodes.h`)
- ✅ Runtime library stubs (`src/runtime/`)

### Missing Components
- ❌ Complete interpreter implementation (control flow, method calls)
- ❌ Main VM program (djvm.exe)
- ❌ Method invocation and call stack
- ❌ Object and array support
- ❌ System.out.println integration
- ❌ Error handling and debugging

## Phase 1: Complete Interpreter Implementation

### 1.1 Control Flow Instructions (Priority: HIGH)
**File:** `src/vm/interpreter.c`

Implement remaining control flow opcodes:
- `OP_GOTO` - Unconditional jump
- `OP_IF_TRUE` - Conditional jump if true
- `OP_IF_FALSE` - Conditional jump if false
- `OP_IF_EQ`, `OP_IF_NE`, `OP_IF_LT`, `OP_IF_LE`, `OP_IF_GT`, `OP_IF_GE` - Comparison jumps

**Implementation Notes:**
- Jumps use 16-bit signed offsets relative to current PC
- Must validate jump targets are within code bounds
- Update PC correctly for both forward and backward jumps

**Estimated Time:** 2-3 hours

### 1.2 Method Invocation (Priority: HIGH)
**File:** `src/vm/interpreter.c`

Implement method call support:
- `OP_INVOKE_STATIC` - Static method calls
- `OP_RETURN` - Return from void method
- `OP_RETURN_VALUE` - Return with value

**New Data Structures Needed:**
```c
typedef struct CallFrame {
    ExecutionContext* ctx;      /* Method execution context */
    struct CallFrame* caller;   /* Previous frame */
    uint16_t return_address;    /* Where to return */
} CallFrame;

typedef struct {
    CallFrame* frames[64];      /* Call stack (max 64 deep) */
    uint16_t frame_count;       /* Current depth */
} CallStack;
```

**Implementation Steps:**
1. Create call stack management functions
2. Implement method lookup in DJC file
3. Handle parameter passing via operand stack
4. Save/restore execution context on calls
5. Handle return values

**Estimated Time:** 4-6 hours

### 1.3 Print Operations (Priority: HIGH)
**File:** `src/vm/interpreter.c`

Implement debug/output opcodes:
- `OP_PRINT_INT` - Print integer to stdout
- `OP_PRINT_CHAR` - Print character to stdout

These are essential for "Hello World" and basic programs.

**Implementation:**
```c
case OP_PRINT_INT:
    value1 = stack_pop(ctx->stack);
    printf("%d", (int16_t)value1);
    break;

case OP_PRINT_CHAR:
    value1 = stack_pop(ctx->stack);
    putchar((char)value1);
    break;
```

**Estimated Time:** 30 minutes

## Phase 2: Create djvm.exe Main Program

### 2.1 Main Program Structure
**New File:** `src/vm/djvm.c`

Create the main VM executable with:
- Command-line argument parsing
- DJC file loading
- Method lookup and execution
- Error handling and reporting

**Program Flow:**
```
1. Parse command line: djvm <file.djc> [method_name]
2. Load DJC file using djc_open()
3. Find entry method (default: "main")
4. Initialize execution context
5. Execute method using interpreter_execute()
6. Report results or errors
7. Cleanup and exit
```

**Command-Line Interface:**
```
Usage: djvm [options] <file.djc> [method]

Options:
  -v, --verbose    Verbose output
  -d, --debug      Debug mode (trace execution)
  -h, --help       Show help
  --version        Show version

Examples:
  djvm hello.djc           # Execute main method
  djvm hello.djc main      # Execute specific method
  djvm -v hello.djc        # Verbose execution
```

**Estimated Time:** 3-4 hours

### 2.2 Error Handling
**File:** `src/vm/djvm.c`

Implement comprehensive error handling:
- File not found
- Invalid DJC format
- Method not found
- Runtime errors (division by zero, stack overflow, etc.)
- Out of memory

**Error Messages:**
```
ERROR: File not found: hello.djc
ERROR: Invalid DJC file format
ERROR: Method 'main' not found
ERROR: Stack overflow at PC=0x0042
ERROR: Division by zero at PC=0x0123
```

**Estimated Time:** 2 hours

## Phase 3: Implement Remaining Opcodes

### 3.1 Object Operations (Priority: MEDIUM)
**File:** `src/vm/interpreter.c`

Implement basic object support:
- `OP_NEW` - Create new object
- `OP_GET_FIELD` - Read instance field
- `OP_PUT_FIELD` - Write instance field
- `OP_GET_STATIC` - Read static field
- `OP_PUT_STATIC` - Write static field

**Note:** For Phase 1, we can implement minimal object support (just for System.out).

**Estimated Time:** 4-6 hours

### 3.2 Array Operations (Priority: LOW)
**File:** `src/vm/interpreter.c`

Implement array support:
- `OP_NEW_ARRAY` - Create new array
- `OP_ARRAY_LENGTH` - Get array length
- `OP_ARRAY_LOAD` - Load from array
- `OP_ARRAY_STORE` - Store to array

**Note:** Can be deferred to Phase 2 if time is limited.

**Estimated Time:** 3-4 hours

### 3.3 Type Operations (Priority: LOW)
**File:** `src/vm/interpreter.c`

Implement type checking:
- `OP_INSTANCEOF` - Check instance type
- `OP_CHECKCAST` - Cast to type

**Note:** Can be deferred or simplified for initial release.

**Estimated Time:** 2-3 hours

## Phase 4: Runtime Library Integration

### 4.1 System.out.println Support
**File:** `src/runtime/system.c`

Implement System.out.println for basic output:
- String printing
- Integer printing
- Character printing

**Integration with Interpreter:**
- Recognize System.out.println calls
- Route to native implementation
- Handle string constants from constant pool

**Estimated Time:** 2-3 hours

### 4.2 String Support
**File:** `src/runtime/string.c`

Implement basic string operations:
- String creation from constant pool
- String concatenation (if needed)
- String comparison

**Estimated Time:** 2-3 hours

## Phase 5: Testing and Debugging

### 5.1 Unit Tests
**New Files:** `tests/vm/test_*.c`

Create unit tests for:
- Individual opcodes
- Control flow
- Method calls
- Stack operations
- Error conditions

**Test Programs:**
```c
// test_arithmetic.c
void test_add() { /* ... */ }
void test_sub() { /* ... */ }
void test_mul() { /* ... */ }
void test_div() { /* ... */ }

// test_control_flow.c
void test_goto() { /* ... */ }
void test_if_true() { /* ... */ }
void test_if_false() { /* ... */ }

// test_methods.c
void test_method_call() { /* ... */ }
void test_return() { /* ... */ }
void test_recursion() { /* ... */ }
```

**Estimated Time:** 4-6 hours

### 5.2 Integration Tests
**New Directory:** `tests/e2e/vm/`

Create end-to-end tests with .djc files:
1. Hello World
2. Arithmetic operations
3. Conditional statements
4. Loops
5. Method calls
6. Recursion (factorial, fibonacci)

**Test Script:**
```batch
@echo off
echo Testing DOS Java VM...

djvm tests\e2e\vm\hello.djc > output.txt
fc output.txt tests\e2e\vm\hello.expected

djvm tests\e2e\vm\arithmetic.djc > output.txt
fc output.txt tests\e2e\vm\arithmetic.expected

echo All tests passed!
```

**Estimated Time:** 3-4 hours

### 5.3 Debug Mode
**File:** `src/vm/djvm.c`

Implement debug/trace mode:
- Print each instruction before execution
- Show stack state
- Show local variables
- Show PC and current method

**Example Output:**
```
[DEBUG] PC=0x0000 OP_PUSH_INT 42
[DEBUG] Stack: [42]
[DEBUG] PC=0x0003 OP_STORE_0
[DEBUG] Stack: []
[DEBUG] Locals: [42, 0, 0]
[DEBUG] PC=0x0004 OP_LOAD_0
[DEBUG] Stack: [42]
```

**Estimated Time:** 2-3 hours

## Phase 6: Documentation and Examples

### 6.1 User Documentation
**New File:** `VM_USAGE.md`

Document:
- How to run djvm
- Command-line options
- Error messages and troubleshooting
- Performance tips
- Memory limitations

**Estimated Time:** 2 hours

### 6.2 Developer Documentation
**New File:** `VM_ARCHITECTURE.md`

Document:
- VM architecture
- Execution model
- Memory layout
- Opcode implementation details
- Extension points

**Estimated Time:** 2-3 hours

### 6.3 Example Programs
**New Directory:** `examples/vm/`

Create example .djc programs:
1. `hello.djc` - Hello World
2. `arithmetic.djc` - Basic arithmetic
3. `factorial.djc` - Recursive factorial
4. `fibonacci.djc` - Fibonacci sequence
5. `loop.djc` - Loop examples

**Estimated Time:** 2 hours

## Implementation Priority

### Phase 1 (Essential - Week 1)
1. Complete control flow instructions (2-3 hours)
2. Implement print operations (30 min)
3. Create djvm.exe main program (3-4 hours)
4. Basic error handling (2 hours)
5. Simple method calls (4-6 hours)

**Total: ~12-16 hours**

### Phase 2 (Important - Week 2)
1. Complete method invocation with call stack (4-6 hours)
2. System.out.println integration (2-3 hours)
3. Basic unit tests (4-6 hours)
4. Integration tests (3-4 hours)

**Total: ~13-19 hours**

### Phase 3 (Nice to Have - Week 3)
1. Object operations (4-6 hours)
2. Array operations (3-4 hours)
3. Debug mode (2-3 hours)
4. Documentation (4-5 hours)

**Total: ~13-18 hours**

## Makefile Updates

Add djvm target to Makefile:

```makefile
# VM executable
djvm: $(BIN_DIR)/djvm.exe

$(BIN_DIR)/djvm.exe: $(OBJ_DIR)/djvm.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS)
	@echo Linking djvm.exe...
	$(LD) $(LDFLAGS) name $@ file { $(OBJ_DIR)/djvm.obj $(VM_OBJS) $(FORMAT_OBJS) $(RUNTIME_OBJS) }

$(OBJ_DIR)/djvm.obj: src/vm/djvm.c
	@echo Compiling djvm.c...
	$(CC) $(CFLAGS) -fo=$@ src/vm/djvm.c
```

## Memory Constraints

### DOS Small Memory Model Limits
- Code segment: 64KB
- Data segment: 64KB
- Stack: ~4KB (configurable)
- Heap: ~30KB (after static data)

### VM Memory Budget
- Interpreter code: ~8KB
- Runtime library: ~6KB
- DJC file data: ~10KB
- Execution stack: ~4KB
- Call stack: ~2KB
- Heap for objects: ~4KB

**Total: ~34KB (within 64KB limit)**

## Success Criteria

### Minimum Viable Product (MVP)
- ✅ Can execute simple .djc files
- ✅ Supports arithmetic operations
- ✅ Supports control flow (if, while)
- ✅ Supports method calls
- ✅ Can print output (System.out.println)
- ✅ Handles errors gracefully

### Full Release
- ✅ All opcodes implemented
- ✅ Object and array support
- ✅ Complete runtime library
- ✅ Comprehensive test suite
- ✅ Debug mode
- ✅ Full documentation

## Risk Assessment

### High Risk
1. **Memory constraints** - 16-bit DOS has limited memory
   - Mitigation: Careful memory management, small buffers
   
2. **Call stack depth** - Limited stack space
   - Mitigation: Limit recursion depth, use heap for call frames

### Medium Risk
1. **Performance** - 8086 CPU is slow
   - Mitigation: Optimize hot paths, minimize memory access
   
2. **Debugging** - Hard to debug on DOS
   - Mitigation: Extensive logging, test on modern systems first

### Low Risk
1. **Compatibility** - Different DOS versions
   - Mitigation: Use standard DOS APIs only

## Next Steps

1. **Start with Phase 1** - Focus on getting basic execution working
2. **Test incrementally** - Test each opcode as implemented
3. **Use existing compiler** - Test with djc-compiled programs
4. **Document as you go** - Keep notes on design decisions

## Timeline Estimate

- **Week 1:** Phase 1 (Essential features) - MVP
- **Week 2:** Phase 2 (Important features) - Beta
- **Week 3:** Phase 3 (Polish and documentation) - Release

**Total: 3 weeks part-time or 1 week full-time**

## Conclusion

The VM implementation is well-structured with existing components. The main work is:
1. Completing the interpreter (control flow, method calls)
2. Creating the djvm.exe main program
3. Testing and debugging

With the existing foundation, this is achievable in 2-3 weeks of focused development.