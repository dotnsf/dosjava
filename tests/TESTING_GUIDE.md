# DOS Java Testing Guide

## Overview

This guide explains how to test the DOS Java compiler (djc.exe) and virtual machine (djvm.exe) using DOSBox.

## Prerequisites

1. **DOSBox Installation**
   - Download from: https://www.dosbox.com/
   - Install to default location: `C:\Program Files (x86)\DOSBox-0.74-3\`
   - Or update the `DOSBOX_PATH` variable in test scripts

2. **Built Executables**
   - `build/bin/djc.exe` - DOS Java Compiler
   - `build/bin/djvm.exe` - DOS Java Virtual Machine
   - Build using: `wmake all` in the dosjava directory

## Test Files

### Current Test Suite

| File | Description | Expected Output |
|------|-------------|-----------------|
| `var1.jav` | Single variable declaration | Prints: 10 |
| `vartest.jav` | Multiple variables | Prints: 10 |
| `arith.jav` | Arithmetic operations | Prints: 8 |
| `iftest.jav` | If statement | Prints: 100 |
| `loop.jav` | While loop | Prints: 10 |

### Test File Details

#### var1.jav
```java
class Var1 {
    public static void main() {
        int x = 10;
        System.out.println(x);
        return;
    }
}
```
**Tests**: Variable declaration, println

#### vartest.jav
```java
class VarTest {
    public static void main() {
        int x = 10;
        int y = 20;
        int z = x;
        System.out.println(z);
        return;
    }
}
```
**Tests**: Multiple variables, variable assignment

#### arith.jav
```java
class Arith {
    public static void main() {
        int a = 5;
        int b = 3;
        int sum = a + b;
        System.out.println(sum);
        return;
    }
}
```
**Tests**: Arithmetic operations (addition)

#### iftest.jav
```java
class IfTest {
    public static void main() {
        int x = 15;
        int result = 0;
        
        if (x > 10) {
            result = 100;
        }
        
        System.out.println(result);
        return;
    }
}
```
**Tests**: If statement, comparison operators

#### loop.jav
```java
class Loop {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 5) {
            sum = sum + i;
            i = i + 1;
        }
        
        System.out.println(sum);
        return;
    }
}
```
**Tests**: While loop, loop counter, accumulator pattern

## Running Tests

### Method 1: Full Test Suite

Run all tests automatically:

```batch
cd dosjava\tests
run_dosbox_tests.bat
```

This will:
1. Launch DOSBox
2. Mount the project directory
3. Run all tests in sequence
4. Display results in DOSBox window

### Method 2: Single Test

Test one file at a time:

```batch
cd dosjava\tests
test_single.bat var1.jav
```

This will:
1. Launch DOSBox
2. Compile the specified file
3. Run the compiled bytecode
4. Display output

### Method 3: Manual Testing in DOSBox

1. **Launch DOSBox**
   ```batch
   "C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe"
   ```

2. **Mount and navigate**
   ```
   mount c C:\Users\KEIKIMURA\src\dosjava
   c:
   cd tests
   ```

3. **Compile a test**
   ```
   ..\build\bin\djc.exe var1.jav
   ```

4. **Run the bytecode**
   ```
   ..\build\bin\djvm.exe VAR1.DJC
   ```

## Test Results Format

### Successful Compilation
```
DOS Java Compiler v1.0
Compiling: var1.jav
[1/4] Lexical analysis... OK
[2/4] Parsing... OK
[3/4] Semantic analysis... OK
[4/4] Code generation... OK
Output: VAR1.DJC
Compilation successful!
```

### Successful Execution
```
DOS Java Virtual Machine v1.0
Loading: VAR1.DJC
Executing main method...
10
Execution completed successfully
```

### Compilation Error Example
```
DOS Java Compiler v1.0
Compiling: error.jav
[1/4] Lexical analysis... OK
[2/4] Parsing... ERROR
Parse error at line 5, column 12
Expected ';' after statement
Compilation failed!
```

## Troubleshooting

### DOSBox Not Found
**Error**: `DOSBox not found at: C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe`

**Solution**: 
- Install DOSBox from https://www.dosbox.com/
- Or edit the test scripts to point to your DOSBox installation

### Executables Not Found
**Error**: `djc.exe not found` or `djvm.exe not found`

**Solution**:
```batch
cd dosjava
wmake clean
wmake all
```

### Test File Not Found
**Error**: `Test file not found: var1.jav`

**Solution**: Make sure you're in the `dosjava\tests` directory

### Compilation Fails
**Possible causes**:
1. Syntax error in test file
2. Compiler bug
3. Memory limitation

**Debug steps**:
1. Check the error message
2. Verify test file syntax
3. Try a simpler test file
4. Check compiler logs

### Runtime Fails
**Possible causes**:
1. Invalid bytecode
2. Stack overflow
3. Memory allocation failure

**Debug steps**:
1. Verify .DJC file was created
2. Check bytecode with hex editor
3. Try with verbose mode: `djvm.exe -v VAR1.DJC`

## Expected Test Results

### Test 1: Variable Declaration (var1.jav)
- ✓ Compilation succeeds
- ✓ VAR1.DJC created
- ✓ Execution prints: `10`
- ✓ Exit code: 0

### Test 2: Multiple Variables (vartest.jav)
- ✓ Compilation succeeds
- ✓ VARTEST.DJC created
- ✓ Execution prints: `10`
- ✓ Exit code: 0

### Test 3: Arithmetic (arith.jav)
- ✓ Compilation succeeds
- ✓ ARITH.DJC created
- ✓ Execution prints: `8`
- ✓ Exit code: 0

### Test 4: If Statement (iftest.jav)
- ✓ Compilation succeeds
- ✓ IFTEST.DJC created
- ✓ Execution prints: `100`
- ✓ Exit code: 0

### Test 5: While Loop (loop.jav)
- ✓ Compilation succeeds
- ✓ LOOP.DJC created
- ✓ Execution prints: `10`
- ✓ Exit code: 0

## Adding New Tests

### Step 1: Create Test File
Create a new `.jav` file in the `tests` directory:

```java
// test_name.jav
class TestName {
    public static void main() {
        // Your test code here
        return;
    }
}
```

### Step 2: Add to Test Suite
Edit `runtest.bat` to include your new test:

```batch
:testN
echo Test N: Description
echo ------------------------------
..\build\bin\djc.exe test_name.jav
if errorlevel 1 goto :testN_fail
if not exist TEST_NAME.DJC goto :testN_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe TEST_NAME.DJC > RT_OUT.TXT
if errorlevel 1 goto :testN_runfail
find "expected_output" RT_OUT.TXT > nul
if errorlevel 1 goto :testN_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched
goto :testN+1
```

### Step 3: Document Expected Behavior
Add to this guide:
- Test description
- Expected output
- What it tests

### Step 4: Run Test
```batch
test_single.bat test_name.jav
```

## Performance Benchmarks

### Compilation Time
- Simple program (10 lines): < 1 second
- Medium program (50 lines): < 2 seconds
- Complex program (100 lines): < 5 seconds

### Execution Time
- Arithmetic operations: ~1000 ops/sec
- Loop iterations: ~500 iterations/sec
- Method calls: ~200 calls/sec

### Memory Usage
- Compiler: ~40KB code + ~20KB data
- VM: ~25KB code + ~10KB data
- Heap: 30KB available for programs

## Continuous Integration

### Pre-commit Checklist
- [ ] All existing tests pass
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] No compiler warnings

### Release Checklist
- [ ] Full test suite passes
- [ ] Performance benchmarks met
- [ ] All documentation updated
- [ ] Example programs work

## Known Issues

### Current Limitations
1. No floating-point support
2. Limited string operations
3. No array support yet
4. No object creation yet

### Planned Improvements
1. Add more test cases
2. Implement automated regression testing
3. Add performance profiling
4. Create stress tests

## Support

For issues or questions:
1. Check this guide
2. Review error messages
3. Check project documentation
4. Review source code comments

---

**Last Updated**: 2026-04-30
**Version**: 1.0
**Status**: Active Development