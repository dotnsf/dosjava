# linechk.exe - Line Content Checker

## Overview
`linechk.exe` is a simple DOS utility to verify that a specific line in a text file matches expected content. It's designed for automated testing in PC-DOS environments.

## Usage

```
linechk.exe <filename> <line_number> <expected_content>
```

### Parameters
- `filename`: Path to the text file to check
- `line_number`: Line number to check (1-based)
- `expected_content`: Expected content of the line (use quotes if it contains spaces)

### Return Codes
- `0`: Line matches expected content (success)
- `1`: Line doesn't match, file not found, or other error (failure)

## Examples

### Example 1: Check single value
```batch
REM Check that line 5 contains "42"
linechk.exe output.txt 5 "42"
if errorlevel 1 goto :fail
echo Line 5 is correct!
```

### Example 2: Check multiple lines
```batch
REM Verify sorted array output
djvm.exe arrays.djc > arrays.out

linechk.exe arrays.out 6 "12"
if errorlevel 1 goto :fail

linechk.exe arrays.out 7 "22"
if errorlevel 1 goto :fail

linechk.exe arrays.out 8 "25"
if errorlevel 1 goto :fail

linechk.exe arrays.out 9 "34"
if errorlevel 1 goto :fail

linechk.exe arrays.out 10 "64"
if errorlevel 1 goto :fail

echo All lines match!
```

### Example 3: Use in test script
```batch
@echo off
echo Testing arrays.jav...

REM Compile and run
..\build\bin\djc.exe arrays.jav
if errorlevel 1 goto :compile_fail

..\build\bin\djvm.exe arrays.djc > arrays.out
if errorlevel 1 goto :runtime_fail

REM Verify output (last 5 lines should be sorted)
..\build\bin\linechk.exe arrays.out 6 "12"
if errorlevel 1 goto :output_fail

..\build\bin\linechk.exe arrays.out 7 "22"
if errorlevel 1 goto :output_fail

..\build\bin\linechk.exe arrays.out 8 "25"
if errorlevel 1 goto :output_fail

..\build\bin\linechk.exe arrays.out 9 "34"
if errorlevel 1 goto :output_fail

..\build\bin\linechk.exe arrays.out 10 "64"
if errorlevel 1 goto :output_fail

echo SUCCESS: All tests passed!
goto :end

:compile_fail
echo FAILED: Compilation error
goto :end

:runtime_fail
echo FAILED: Runtime error
goto :end

:output_fail
echo FAILED: Output mismatch
type arrays.out
goto :end

:end
```

## Error Messages

### File not found
```
ERROR: Cannot open file: output.txt
```

### Invalid line number
```
ERROR: Invalid line number: abc
```

### Line mismatch
```
MISMATCH at line 5:
  Expected: [42]
  Actual:   [43]
```

### File too short
```
ERROR: File has only 3 lines, cannot check line 5
```

## Technical Details

- **Platform**: 16-bit PC-DOS
- **Memory Model**: Medium (-mm)
- **Max Line Length**: 256 characters
- **Line Ending**: Handles both CR+LF and LF
- **Comparison**: Exact string match (case-sensitive)

## Building

```batch
cd dosjava
build_linechk.bat
```

Output: `build\bin\linechk.exe`

## Integration with Test Suite

The `linechk.exe` utility can be integrated into `runtest.bat` to provide more robust output verification:

```batch
REM Old method (only checks if value exists somewhere)
find "64" RT_OUT.TXT > nul
if errorlevel 1 goto :fail

REM New method (checks specific line content and order)
linechk.exe RT_OUT.TXT 10 "64"
if errorlevel 1 goto :fail
```

## Limitations

- Maximum line length: 256 characters
- Case-sensitive comparison only
- No regular expression support
- No wildcard matching
- Requires exact match (no trimming of whitespace)

## See Also

- `runtest.bat` - Main test runner script
- `arrays.jav` - Example test that benefits from line checking