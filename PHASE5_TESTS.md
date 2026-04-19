# Phase 5: End-to-End Test Cases

## Test Strategy

### Test Levels
1. **Smoke Tests**: Basic compilation functionality
2. **Feature Tests**: Test each language feature
3. **Error Tests**: Test error handling
4. **Integration Tests**: Test full pipeline
5. **Regression Tests**: Ensure existing functionality works

## Test Suite Structure

```
tests/
├── e2e/                    # End-to-end tests
│   ├── basic/             # Basic functionality
│   │   ├── hello.java
│   │   ├── empty.java
│   │   └── minimal.java
│   ├── arithmetic/        # Arithmetic operations
│   │   ├── add.java
│   │   ├── multiply.java
│   │   └── complex.java
│   ├── control/           # Control flow
│   │   ├── if.java
│   │   ├── while.java
│   │   └── nested.java
│   ├── variables/         # Variable declarations
│   │   ├── locals.java
│   │   ├── scope.java
│   │   └── shadowing.java
│   ├── methods/           # Method calls
│   │   ├── simple.java
│   │   ├── params.java
│   │   └── recursive.java
│   ├── complex/           # Complex programs
│   │   ├── fibonacci.java
│   │   ├── factorial.java
│   │   └── prime.java
│   └── errors/            # Error cases
│       ├── syntax_error.java
│       ├── type_error.java
│       └── undefined.java
├── run_tests.bat          # Test runner script
└── verify_output.bat      # Output verification script
```

## Test Cases

### 1. Basic Tests

#### 1.1 Hello World (hello.java)
```java
class Hello {
    public static void main() {
        int x = 42;
        return;
    }
}
```

**Expected Output**:
- Compilation succeeds
- Generates hello.djc
- Bytecode size: ~50 bytes
- No errors

**Verification**:
```batch
djc hello.java
if errorlevel 1 goto error
if not exist hello.djc goto error
echo PASS: hello.java
```

#### 1.2 Empty Class (empty.java)
```java
class Empty {
}
```

**Expected Output**:
- Compilation succeeds
- Generates empty.djc
- Minimal bytecode

#### 1.3 Minimal Program (minimal.java)
```java
class Minimal {
    public static void main() {
        return;
    }
}
```

**Expected Output**:
- Compilation succeeds
- Generates minimal.djc
- Contains main method

### 2. Arithmetic Tests

#### 2.1 Addition (add.java)
```java
class Add {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b;
        return;
    }
}
```

**Expected Bytecode**:
```
ICONST 10
ISTORE 0
ICONST 20
ISTORE 1
ILOAD 0
ILOAD 1
IADD
ISTORE 2
RETURN
```

#### 2.2 Multiplication (multiply.java)
```java
class Multiply {
    public static void main() {
        int x = 5;
        int y = 6;
        int z = x * y;
        return;
    }
}
```

#### 2.3 Complex Expression (complex.java)
```java
class Complex {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = 30;
        int result = (a + b) * c - a / 2;
        return;
    }
}
```

**Tests**:
- Operator precedence
- Parentheses
- Multiple operations

### 3. Control Flow Tests

#### 3.1 If Statement (if.java)
```java
class IfTest {
    public static void main() {
        int x = 10;
        int y = 0;
        
        if (x > 5) {
            y = 100;
        }
        
        return;
    }
}
```

**Expected Bytecode**:
```
ICONST 10
ISTORE 0
ICONST 0
ISTORE 1
ILOAD 0
ICONST 5
IF_ICMPGT label1
GOTO label2
label1:
ICONST 100
ISTORE 1
label2:
RETURN
```

#### 3.2 While Loop (while.java)
```java
class WhileTest {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        return;
    }
}
```

**Tests**:
- Loop initialization
- Condition evaluation
- Loop body execution
- Loop increment

#### 3.3 Nested Control Flow (nested.java)
```java
class Nested {
    public static void main() {
        int i = 0;
        
        while (i < 10) {
            if (i > 5) {
                int x = i * 2;
            }
            i = i + 1;
        }
        
        return;
    }
}
```

**Tests**:
- Nested scopes
- Label management
- Jump instructions

### 4. Variable Tests

#### 4.1 Local Variables (locals.java)
```java
class Locals {
    public static void main() {
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        int e = 5;
        return;
    }
}
```

**Tests**:
- Multiple local variables
- Variable slot allocation
- Load/store instructions

#### 4.2 Variable Scope (scope.java)
```java
class Scope {
    public static void main() {
        int x = 10;
        
        if (x > 0) {
            int y = 20;
            int z = x + y;
        }
        
        int w = x + 5;
        return;
    }
}
```

**Tests**:
- Block scope
- Variable lifetime
- Scope exit

#### 4.3 Variable Shadowing (shadowing.java)
```java
class Shadowing {
    public static void main() {
        int x = 10;
        
        if (x > 0) {
            int x = 20;
            int y = x + 5;
        }
        
        int z = x + 1;
        return;
    }
}
```

**Tests**:
- Inner scope shadows outer
- Correct variable resolution
- Scope-based slot allocation

### 5. Method Tests

#### 5.1 Simple Method Call (simple.java)
```java
class SimpleMethod {
    public static int getValue() {
        return 42;
    }
    
    public static void main() {
        int x = getValue();
        return;
    }
}
```

**Tests**:
- Method invocation
- Return value handling
- Stack management

#### 5.2 Method with Parameters (params.java)
```java
class Params {
    public static int add(int a, int b) {
        return a + b;
    }
    
    public static void main() {
        int result = add(10, 20);
        return;
    }
}
```

**Tests**:
- Parameter passing
- Argument evaluation
- Return value

#### 5.3 Recursive Method (recursive.java)
```java
class Recursive {
    public static int factorial(int n) {
        if (n <= 1) {
            return 1;
        }
        return n * factorial(n - 1);
    }
    
    public static void main() {
        int result = factorial(5);
        return;
    }
}
```

**Tests**:
- Recursive calls
- Stack frame management
- Base case handling

### 6. Complex Programs

#### 6.1 Fibonacci (fibonacci.java)
```java
class Fibonacci {
    public static void main() {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        
        while (i < n) {
            int temp = a + b;
            a = b;
            b = temp;
            i = i + 1;
        }
        
        return;
    }
}
```

**Tests**:
- Loop with multiple variables
- Variable updates
- Temporary variables

#### 6.2 Factorial (factorial.java)
```java
class Factorial {
    public static void main() {
        int n = 5;
        int result = 1;
        int i = 1;
        
        while (i <= n) {
            result = result * i;
            i = i + 1;
        }
        
        return;
    }
}
```

**Tests**:
- Accumulator pattern
- Loop condition
- Multiplication

#### 6.3 Prime Numbers (prime.java)
```java
class Prime {
    public static void main() {
        int n = 20;
        int i = 2;
        
        while (i <= n) {
            int isPrime = 1;
            int j = 2;
            
            while (j < i) {
                if (i / j * j == i) {
                    isPrime = 0;
                }
                j = j + 1;
            }
            
            i = i + 1;
        }
        
        return;
    }
}
```

**Tests**:
- Nested loops
- Boolean logic
- Division and modulo

### 7. Error Tests

#### 7.1 Syntax Error (syntax_error.java)
```java
class SyntaxError {
    public static void main() {
        int x = 10
        return;
    }
}
```

**Expected Output**:
```
Compilation failed in phase: Parsing
Error: Parse error at line 3, column 16
Expected ';' after expression
```

#### 7.2 Type Error (type_error.java)
```java
class TypeError {
    public static void main() {
        int x = 10;
        boolean y = x;
        return;
    }
}
```

**Expected Output**:
```
Compilation failed in phase: Semantic analysis
Error: Type mismatch: cannot assign int to boolean
```

#### 7.3 Undefined Variable (undefined.java)
```java
class Undefined {
    public static void main() {
        int x = y + 10;
        return;
    }
}
```

**Expected Output**:
```
Compilation failed in phase: Semantic analysis
Error: Undefined variable: y
```

## Test Runner Script

### run_tests.bat
```batch
@echo off
setlocal enabledelayedexpansion

echo ========================================
echo DOS Java Compiler - End-to-End Tests
echo ========================================
echo.

set PASSED=0
set FAILED=0
set TOTAL=0

rem Basic tests
echo [1/7] Running basic tests...
call :run_test_dir basic
echo.

rem Arithmetic tests
echo [2/7] Running arithmetic tests...
call :run_test_dir arithmetic
echo.

rem Control flow tests
echo [3/7] Running control flow tests...
call :run_test_dir control
echo.

rem Variable tests
echo [4/7] Running variable tests...
call :run_test_dir variables
echo.

rem Method tests
echo [5/7] Running method tests...
call :run_test_dir methods
echo.

rem Complex tests
echo [6/7] Running complex tests...
call :run_test_dir complex
echo.

rem Error tests
echo [7/7] Running error tests...
call :run_test_dir errors
echo.

echo ========================================
echo Test Results
echo ========================================
echo Total:  %TOTAL%
echo Passed: %PASSED%
echo Failed: %FAILED%
echo.

if %FAILED% equ 0 (
    echo All tests passed!
    exit /b 0
) else (
    echo Some tests failed!
    exit /b 1
)

:run_test_dir
set DIR=%1
pushd %DIR%

for %%f in (*.java) do (
    set /a TOTAL+=1
    echo Testing %%f...
    
    ..\..\..\..\build\bin\djc.exe %%f > nul 2>&1
    
    if errorlevel 1 (
        if "%DIR%"=="errors" (
            echo   PASS: %%f (expected error)
            set /a PASSED+=1
        ) else (
            echo   FAIL: %%f (compilation failed)
            set /a FAILED+=1
        )
    ) else (
        if "%DIR%"=="errors" (
            echo   FAIL: %%f (should have failed)
            set /a FAILED+=1
        ) else (
            set BASENAME=%%~nf
            if exist !BASENAME!.djc (
                echo   PASS: %%f
                set /a PASSED+=1
                del !BASENAME!.djc
            ) else (
                echo   FAIL: %%f (no output file)
                set /a FAILED+=1
            )
        )
    )
)

popd
goto :eof
```

## Verification Strategy

### Bytecode Verification
1. Check file format (magic number, version)
2. Verify constant pool entries
3. Check method table
4. Validate bytecode instructions
5. Verify jump targets

### Runtime Verification
1. Load .djc file in VM
2. Execute main method
3. Check final state
4. Verify no crashes

### Performance Verification
1. Measure compilation time
2. Check memory usage
3. Verify bytecode size

## Success Criteria

### Compilation Tests
- ✓ All basic tests pass
- ✓ All arithmetic tests pass
- ✓ All control flow tests pass
- ✓ All variable tests pass
- ✓ All method tests pass
- ✓ All complex tests pass

### Error Tests
- ✓ Syntax errors detected
- ✓ Type errors detected
- ✓ Undefined symbols detected
- ✓ Clear error messages

### Performance
- ✓ Compilation time < 5 seconds per file
- ✓ Memory usage < 64KB
- ✓ Bytecode size reasonable

### Quality
- ✓ No compiler crashes
- ✓ Consistent behavior
- ✓ Correct bytecode generation

## Test Execution Plan

### Phase 1: Basic Tests
1. Create test files
2. Run djc on each file
3. Verify output files exist
4. Check for errors

### Phase 2: Bytecode Verification
1. Parse .djc files
2. Verify structure
3. Check instruction validity

### Phase 3: Runtime Verification
1. Load in VM
2. Execute
3. Verify results

### Phase 4: Error Testing
1. Test error cases
2. Verify error messages
3. Check error recovery

### Phase 5: Regression Testing
1. Run all previous tests
2. Verify no regressions
3. Document any issues

## Test Maintenance

### Adding New Tests
1. Create .java file in appropriate directory
2. Add expected output/behavior
3. Update run_tests.bat if needed
4. Document test purpose

### Updating Tests
1. Modify .java file
2. Update expected output
3. Re-run test suite
4. Document changes

### Removing Tests
1. Remove .java file
2. Update documentation
3. Re-run test suite
4. Verify no impact

## Continuous Testing

### Pre-commit Tests
- Run basic tests
- Check for regressions

### Build Tests
- Run full test suite
- Verify all tests pass

### Release Tests
- Run all tests
- Performance testing
- Stress testing
- Documentation review