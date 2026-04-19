# Phase 3: Testing and Integration

## Overview

Phase 3 focuses on testing the interpreter with actual bytecode files.

## New Components

### 1. mkdjc.exe - .djc File Generator
Creates test .djc files for testing the interpreter.

**Location:** `tools/mkdjc.c`

**Usage:**
```batch
mkdjc arithmetic    # Create arithmetic test
mkdjc loop          # Create loop test
mkdjc all           # Create all tests
```

### 2. test_int.exe - Interpreter Test Program
Tests the bytecode interpreter with .djc files.

**Location:** `src/test_interpreter.c`

**Usage:**
```batch
test_int test_arithmetic.djc
test_int test_loop.djc
```

## Build Instructions

### 1. Build All Components
```batch
cd dosjava
wmake all
```

This builds:
- `build/bin/test_mem.exe` - Memory manager test
- `build/bin/test_int.exe` - Interpreter test
- `build/bin/mkdjc.exe` - .djc file generator

### 2. Generate Test Files
```batch
cd build\bin
mkdjc all
```

This creates:
- `test_arithmetic.djc` - Arithmetic test (10 + 20 = 30)
- `test_loop.djc` - Loop test (prints 0-4)

### 3. Run Tests
```batch
test_int test_arithmetic.djc
test_int test_loop.djc
```

## Test Programs

### Arithmetic Test
```
Equivalent Java code:
int main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    print(c);  // Should print 30
    return;
}
```

**Expected Output:** `30`

### Loop Test
```
Equivalent Java code:
int main() {
    int i = 0;
    while (i < 5) {
        print(i);
        i = i + 1;
    }
    return;
}
```

**Expected Output:** `01234`

## Bytecode Format

### File Structure
```
Header (12 bytes):
  - Magic: 0x444A ('DJ')
  - Version: 0x0001
  - Constant pool count
  - Method count
  - Field count
  - Code size

Constant Pool:
  - Type tag (1 byte)
  - Length (2 bytes)
  - Data (variable)

Method Descriptors:
  - Name index (2 bytes)
  - Descriptor index (2 bytes)
  - Code offset (2 bytes)
  - Code length (2 bytes)
  - Max stack (1 byte)
  - Max locals (1 byte)
  - Flags (1 byte)

Bytecode Section:
  - Raw bytecode instructions
```

### Supported Instructions

**Stack Operations:**
- `PUSH_INT value` - Push immediate integer
- `POP` - Pop and discard
- `DUP` - Duplicate top

**Local Variables:**
- `LOAD_0/1/2` - Load local variable
- `STORE_0/1/2` - Store to local variable

**Arithmetic:**
- `ADD` - Addition
- `SUB` - Subtraction
- `MUL` - Multiplication
- `DIV` - Division
- `MOD` - Modulo
- `NEG` - Negate

**Comparison:**
- `CMP_EQ/NE/LT/LE/GT/GE` - Comparisons

**Control Flow:**
- `GOTO offset` - Unconditional jump
- `IF_TRUE offset` - Jump if true
- `IF_FALSE offset` - Jump if false

**Debug:**
- `PRINT_INT` - Print integer
- `PRINT_CHAR` - Print character

**Termination:**
- `RETURN` - Return from method

## Troubleshooting

### "File not found"
- Make sure you're in the `build/bin` directory
- Run `mkdjc all` to generate test files

### "Invalid bytecode"
- Check that .djc file was created correctly
- Verify magic number is 0x444A

### "Stack overflow"
- Increase max_stack in method descriptor
- Check for infinite loops

### "Execution failed"
- Enable debug output in interpreter
- Check bytecode offsets are correct

## Next Steps

After successful testing:

1. **Add More Tests**
   - Conditional branches
   - More complex arithmetic
   - Nested loops

2. **Implement Missing Features**
   - Method calls
   - Object creation
   - String operations

3. **Build Preprocessor**
   - Parse Java .class files
   - Translate to .djc format
   - Handle more complex programs

## Performance Notes

### Current Limitations
- No optimization
- Interpreted execution only
- Simple reference counting GC

### Expected Performance
- Arithmetic: ~1000 ops/sec
- Loops: ~500 iterations/sec
- Memory: ~30KB heap available

## Success Criteria

Phase 3 is complete when:
- [x] mkdjc.exe builds successfully
- [x] test_int.exe builds successfully
- [ ] Arithmetic test passes (prints 30)
- [ ] Loop test passes (prints 01234)
- [ ] No memory leaks detected
- [ ] All bytecode instructions work correctly

## Files Created

```
dosjava/
├── tools/
│   └── mkdjc.c (227 lines) - .djc file generator
├── src/
│   └── test_interpreter.c (119 lines) - Interpreter test
└── PHASE3_TESTING.md (this file)
```

## Build Output

Expected files after `wmake all`:
```
build/
├── obj/
│   ├── memory.obj
│   ├── stack.obj
│   ├── interpreter.obj
│   ├── djc.obj
│   ├── opcodes.obj
│   ├── object.obj
│   ├── string.obj
│   ├── system.obj
│   ├── test_memory.obj
│   ├── test_interpreter.obj
│   └── mkdjc.obj
└── bin/
    ├── test_mem.exe
    ├── test_int.exe
    └── mkdjc.exe
```

## Testing Workflow

```
1. Build everything:
   wmake all

2. Go to bin directory:
   cd build\bin

3. Generate test files:
   mkdjc all

4. Run tests:
   test_int test_arithmetic.djc
   test_int test_loop.djc

5. Verify output matches expected results
```

## Debug Tips

### Enable Verbose Output
Modify `test_interpreter.c` to print more details:
- Bytecode disassembly
- Stack state after each instruction
- Memory allocation tracking

### Hex Dump .djc Files
Use a hex editor to verify file format:
- First 2 bytes should be 0x4A 0x44 (little-endian 0x444A)
- Check offsets and lengths

### Step Through Execution
Add breakpoints in `interpreter_step()` to see each instruction execute.

---

**Phase 3 Status:** Implementation Complete, Testing Pending