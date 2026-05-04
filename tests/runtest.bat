@echo off
echo ========================================
echo DOS Java Compiler - Control Flow Tests
echo ========================================
echo.

if exist RT_OUT.TXT del RT_OUT.TXT

echo Test 0: Hello World
echo ------------------------------
..\build\bin\djc.exe hello.jav
if errorlevel 1 goto :test0_fail
if not exist HELLO.DJC goto :test0_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe HELLO.DJC > RT_OUT.TXT
if errorlevel 1 goto :test0_runfail
find "Hello World!" RT_OUT.TXT > nul
if errorlevel 1 goto :test0_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Hello World!"
echo.
goto :test1

:test0_fail
echo FAILED: Compilation error
goto :end

:test0_nofile
echo FAILED: DJC file not created
goto :end

:test0_runfail
echo FAILED: Runtime error
goto :end

:test0_badout
echo FAILED: Output mismatch for Test 0
echo Expected: 10
echo Actual:
type RT_OUT.TXT
goto :end

:test1

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 1: Variable Declaration
echo ------------------------------
..\build\bin\djc.exe vartest.jav
if errorlevel 1 goto :test1_fail
if not exist VARTEST.DJC goto :test1_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe VARTEST.DJC > RT_OUT.TXT
if errorlevel 1 goto :test1_runfail
find "10" RT_OUT.TXT > nul
if errorlevel 1 goto :test1_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 10
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
goto :end

:test1_badout
echo FAILED: Output mismatch for Test 1
echo Expected: 10
echo Actual:
type RT_OUT.TXT
goto :end

:test2

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 2: Arithmetic Operations
echo ------------------------------
..\build\bin\djc.exe arith.jav
if errorlevel 1 goto :test2_fail
if not exist ARITH.DJC goto :test2_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ARITH.DJC > RT_OUT.TXT
if errorlevel 1 goto :test2_runfail
find "8" RT_OUT.TXT > nul
if errorlevel 1 goto :test2_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 8
echo.
goto :test3

:test2_fail
echo FAILED: Compilation error
goto :end

:test2_nofile
echo FAILED: DJC file not created
goto :end

:test2_runfail
echo FAILED: Runtime error
goto :end

:test2_badout
echo FAILED: Output mismatch for Test 2
echo Expected: 8
echo Actual:
type RT_OUT.TXT
goto :end

:test3

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 3: If Statement
echo ------------------------------
..\build\bin\djc.exe iftest.jav
if errorlevel 1 goto :test3_fail
if not exist IFTEST.DJC goto :test3_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe IFTEST.DJC > RT_OUT.TXT
if errorlevel 1 goto :test3_runfail
find "100" RT_OUT.TXT > nul
if errorlevel 1 goto :test3_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 100
echo.
goto :test4

:test3_fail
echo FAILED: Compilation error
goto :end

:test3_nofile
echo FAILED: DJC file not created
goto :end

:test3_runfail
echo FAILED: Runtime error
goto :end

:test3_badout
echo FAILED: Output mismatch for Test 3
echo Expected: 100
echo Actual:
type RT_OUT.TXT
goto :end

:test4

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 4: While Loop
echo ------------------------------
..\build\bin\djc.exe loop.jav
if errorlevel 1 goto :test4_fail
if not exist LOOP.DJC goto :test4_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe LOOP.DJC > RT_OUT.TXT
if errorlevel 1 goto :test4_runfail
find "10" RT_OUT.TXT > nul
if errorlevel 1 goto :test4_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 10
echo.

goto :test5

:test4_fail
echo FAILED: Compilation error
goto :end

:test4_nofile
echo FAILED: DJC file not created
goto :end

:test4_runfail
echo FAILED: Runtime error
goto :end

:test4_badout
echo FAILED: Output mismatch for Test 4
echo Expected: 10
echo Actual:
type RT_OUT.TXT
goto :end

:test5

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 5: Calc
echo ------------------------------
..\build\bin\djc.exe calc.jav
if errorlevel 1 goto :test5_fail
if not exist CALC.DJC goto :test5_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe CALC.DJC > RT_OUT.TXT
if errorlevel 1 goto :test5_runfail
find "20\n5\n0\n1\n0" RT_OUT.TXT > nul
if errorlevel 1 goto :test5_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "20\n5\n0\n1\n0"
echo.

goto :test6

:test5_fail
echo FAILED: Compilation error
goto :end

:test5_nofile
echo FAILED: DJC file not created
goto :end

:test5_runfail
echo FAILED: Runtime error
goto :end

:test5_badout
echo FAILED: Output mismatch for Test 5
echo Expected: "20\n5\n0\n1\n0"
echo Actual:
type RT_OUT.TXT
goto :end

:test6

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 6: For
echo ------------------------------
..\build\bin\djc.exe for.jav
if errorlevel 1 goto :test6_fail
if not exist FOR.DJC goto :test6_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe FOR.DJC > RT_OUT.TXT
if errorlevel 1 goto :test6_runfail
find "10" RT_OUT.TXT > nul
if errorlevel 1 goto :test6_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 10
echo.

#goto :completed
goto :test7

:test6_fail
echo FAILED: Compilation error
goto :end

:test6_nofile
echo FAILED: DJC file not created
goto :end

:test6_runfail
echo FAILED: Runtime error
goto :end

:test6_badout
echo FAILED: Output mismatch for Test 6
echo Expected: 10
echo Actual:
type RT_OUT.TXT
goto :end

:test7

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 7: Arrays
echo ------------------------------
..\build\bin\djc.exe arrays.jav
if errorlevel 1 goto :test7_fail
if not exist ARRAYS.DJC goto :test7_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ARRAYS.DJC > RT_OUT.TXT
if errorlevel 1 goto :test7_runfail
find "12\n22\n25\n34\n64" RT_OUT.TXT > nul
if errorlevel 1 goto :test7_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "12\n22\n25\n34\n64"
echo.

goto :test8

:test7_fail
echo FAILED: Compilation error
goto :end

:test7_nofile
echo FAILED: DJC file not created
goto :end

:test7_runfail
echo FAILED: Runtime error
goto :end

:test7_badout
echo FAILED: Output mismatch for Test 7
echo Expected: "12\n22\n25\n34\n64"
echo Actual:
type RT_OUT.TXT
goto :end

:test8

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 8: Function and return value
echo ------------------------------
..\build\bin\djc.exe func.jav
if errorlevel 1 goto :test8_fail
if not exist FUNC.DJC goto :test8_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe FUNC.DJC > RT_OUT.TXT
if errorlevel 1 goto :test8_runfail
find "3\n6\n6" RT_OUT.TXT > nul
if errorlevel 1 goto :test8_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "3\n6\n6"
echo.

goto :test9

:test8_fail
echo FAILED: Compilation error
goto :end

:test8_nofile
echo FAILED: DJC file not created
goto :end

:test8_runfail
echo FAILED: Runtime error
goto :end

:test8_badout
echo FAILED: Output mismatch for Test 8
echo Expected: "3\n6\n6"
echo Actual:
type RT_OUT.TXT
goto :end

:test9

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 9: String
echo ------------------------------
..\build\bin\djc.exe str.jav
if errorlevel 1 goto :test9_fail
if not exist STR.DJC goto :test9_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STR.DJC > RT_OUT.TXT
if errorlevel 1 goto :test9_runfail
find "Hello World!" RT_OUT.TXT > nul
if errorlevel 1 goto :test9_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Hello World!" 
echo.

goto :test10

:test9_fail
echo FAILED: Compilation error
goto :end

:test9_nofile
echo FAILED: DJC file not created
goto :end

:test9_runfail
echo FAILED: Runtime error
goto :end

:test9_badout
echo FAILED: Output mismatch for Test 9
echo Expected: "Hello World!"
echo Actual:
type RT_OUT.TXT
goto :end


:test10

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 10: String length
echo ------------------------------
..\build\bin\djc.exe strlen.jav
if errorlevel 1 goto :test10_fail
if not exist STRLEN.DJC goto :test10_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STRLEN.DJC > RT_OUT.TXT
if errorlevel 1 goto :test10_runfail
find "Hello World!\n12\n5" RT_OUT.TXT > nul
if errorlevel 1 goto :test10_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Hello World!\n12\n5" 
echo.

goto :completed

:test10_fail
echo FAILED: Compilation error
goto :end

:test10_nofile
echo FAILED: DJC file not created
goto :end

:test10_runfail
echo FAILED: Runtime error
goto :end

:test10_badout
echo FAILED: Output mismatch for Test 10
echo Expected: "Hello World!\n12\n5"
echo Actual:
type RT_OUT.TXT
goto :end



:completed

echo ========================================
echo All tests completed!
echo ========================================

:end

