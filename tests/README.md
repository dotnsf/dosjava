# DOS Java Test Suite

## Quick Start

### Run All Tests
```batch
run_dosbox_tests.bat
```

### Run Single Test
```batch
test_single.bat var1.jav
```

### Manual Testing
```batch
# In DOSBox:
mount c C:\Users\KEIKIMURA\src\dosjava
c:
cd tests
..\build\bin\djc.exe var1.jav
..\build\bin\djvm.exe VAR1.DJC
```

### DOS 8.3 Naming Rule
実機 DOS / DOSBox を前提としたテストでは、ソース名は 8.3 形式を使用します。
例:
- `str.jav`
- `strlen.jav`
- `strcat.jav`
- `func.jav` または `FUNC.JAV`

## Test Files

- `HELLO.JAV` - Basic hello/string output
- `var1.jav` - Single variable (prints 10)
- `vartest.jav` - Multiple variables
- `arith.jav` - Arithmetic
- `calc.jav` - Arithmetic and expressions
- `iftest.jav` - If / else
- `loop.jav` - While loop
- `for.jav` - For loop
- `func.JAV` - Static method call / int parameter / int return
- `array.jav` - Basic array operations
- `arrays.jav` - Extended array operations
- `arraysim.jav` - Array-oriented stress/simple simulation case
- `str.jav` - String locals and `println(String)`
- `strlen.jav` - `String.length()`
- `strcat.jav` - `String + String` and chained concatenation
- `switch.jav` - Additional control-flow-oriented test asset

## Documentation

See [TESTING_GUIDE.md](TESTING_GUIDE.md) for complete documentation.

## Requirements

- DOSBox 0.74 or later
- Built executables in `../build/bin/`

## Scripts

- `runtest.bat` - Main test suite (runs in DOSBox)
- `run_dosbox_tests.bat` - Launches DOSBox with test suite
- `test_single.bat` - Tests a single file in DOSBox
- `test_array.bat` - Array test helper
- `test_arith.bat` - Arithmetic test helper
- `test_calc.bat` - Calculation test helper
- `test_switch.bat` - Switch/control test helper