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

## Test Files

- `var1.jav` - Single variable (prints 10)
- `vartest.jav` - Multiple variables (prints 10)
- `arith.jav` - Arithmetic (prints 8)
- `iftest.jav` - If statement (prints 100)
- `loop.jav` - While loop (prints 10)

## Documentation

See [TESTING_GUIDE.md](TESTING_GUIDE.md) for complete documentation.

## Requirements

- DOSBox 0.74 or later
- Built executables in `../build/bin/`

## Scripts

- `runtest.bat` - Main test suite (runs in DOSBox)
- `run_dosbox_tests.bat` - Launches DOSBox with test suite
- `test_single.bat` - Tests a single file in DOSBox