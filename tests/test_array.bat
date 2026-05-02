@echo off
echo ========================================
echo DOS Java Compiler - Array Tests
echo ========================================
echo.

if exist RT_OUT.TXT del RT_OUT.TXT

echo Test 1: Basic Array Operations
echo ------------------------------
..\build\bin\djc.exe array.jav
if errorlevel 1 goto :test1_fail
if not exist ARRAY.DJC goto :test1_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ARRAY.DJC > RT_OUT.TXT
if errorlevel 1 goto :test1_runfail
echo Output:
type RT_OUT.TXT
echo Expected: 10 20 30 40 50 5 150
echo.
goto :test2

:test1_fail
echo FAILED: Compilation error
goto :end

:test1_nofile
echo FAILED: DJC file not created
goto :end

:test1_runfail
echo FAILED: Runtime error
type RT_OUT.TXT
goto :end

:test2

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 2: Array Sorting (Bubble Sort)
echo ------------------------------
..\build\bin\djc.exe arrays.jav
if errorlevel 1 goto :test2_fail
if not exist ARRAYS.DJC goto :test2_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ARRAYS.DJC > RT_OUT.TXT
if errorlevel 1 goto :test2_runfail
echo Output:
type RT_OUT.TXT
echo Expected: 64 34 25 12 22 12 22 25 34 64
echo.
goto :completed

:test2_fail
echo FAILED: Compilation error
goto :end

:test2_nofile
echo FAILED: DJC file not created
goto :end

:test2_runfail
echo FAILED: Runtime error
type RT_OUT.TXT
goto :end

:completed

echo ========================================
echo All array tests completed!
echo ========================================

:end

@REM Made with Bob