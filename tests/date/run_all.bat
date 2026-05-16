@echo off
echo ========================================
echo Date Class - All Tests
echo ========================================
echo.
echo Running all Date class tests...
echo.

echo ----------------------------------------
echo Test 1: Date Class Registration
echo ----------------------------------------
call test_d1.bat
echo.

echo ----------------------------------------
echo Test 2: Date Constructor
echo ----------------------------------------
call test_d2.bat
echo.

echo ----------------------------------------
echo Test 3: Date Getter Methods
echo ----------------------------------------
call test_d3.bat
echo.

echo ----------------------------------------
echo Test 4: Date setTime/getTime
echo ----------------------------------------
call test_d4.bat
echo.

echo ----------------------------------------
echo Test 5: Multiple Date Objects
echo ----------------------------------------
call test_d5.bat
echo.

echo ----------------------------------------
echo Test 6: Date Formatting
echo ----------------------------------------
call test_d6.bat
echo.

echo ========================================
echo All Date Tests Complete
echo ========================================
