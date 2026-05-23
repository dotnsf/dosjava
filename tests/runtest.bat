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
REM find "Hello World!" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Hello World!" > nul
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
REM find "10" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "10" > nul
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
REM find "8" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "8" > nul
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
REM find "100" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "100" > nul
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
REM find "10" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "10" > nul
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
REM find "12\n8\n20\n5\n0\n1\n0\n2\n-8" RT_OUT.TXT > nul
REM find "-8" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "12" > nul
linechk RT_OUT.TXT 2 "8" > nul
linechk RT_OUT.TXT 3 "20" > nul
linechk RT_OUT.TXT 4 "5" > nul
linechk RT_OUT.TXT 5 "0" > nul
linechk RT_OUT.TXT 6 "1" > nul
linechk RT_OUT.TXT 7 "0" > nul
linechk RT_OUT.TXT 8 "2" > nul
linechk RT_OUT.TXT 9 "-8" > nul
if errorlevel 1 goto :test5_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "12\n8\n20\n5\n0\n1\n0\n2\n-8"
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
echo Expected: "12\n8\n20\n5\n0\n1\n0\n2\n-8"
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
REM find "10" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "10" > nul
if errorlevel 1 goto :test6_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value 10
echo.

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
REM find "12\n22\n25\n34\n64" RT_OUT.TXT > nul
REM find "64" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "64" > nul
linechk RT_OUT.TXT 2 "34" > nul
linechk RT_OUT.TXT 3 "25" > nul
linechk RT_OUT.TXT 4 "12" > nul
linechk RT_OUT.TXT 5 "22" > nul
linechk RT_OUT.TXT 6 "12" > nul
linechk RT_OUT.TXT 7 "22" > nul
linechk RT_OUT.TXT 8 "25" > nul
linechk RT_OUT.TXT 9 "34" > nul
linechk RT_OUT.TXT 10 "64" > nul

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
REM find "3\n6\n6" RT_OUT.TXT > nul
REM find "3" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "3" > nul
linechk RT_OUT.TXT 2 "6" > nul
linechk RT_OUT.TXT 3 "6" > nul
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
REM find "Hello World!" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Hello World!" > nul
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
REM find "Hello World!\n12\n5" RT_OUT.TXT > nul
REM find "12" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Hello World!" > nul
linechk RT_OUT.TXT 2 "12" > nul
linechk RT_OUT.TXT 3 "5" > nul
if errorlevel 1 goto :test10_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Hello World!\n12\n5" 
echo.

goto :test11

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


:test11

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 11: String concat
echo ------------------------------
..\build\bin\djc.exe strcat.jav
if errorlevel 1 goto :test11_fail
if not exist STRCAT.DJC goto :test11_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STRCAT.DJC > RT_OUT.TXT
if errorlevel 1 goto :test11_runfail
REM find "1234\nAABB\n123456" RT_OUT.TXT > nul
REM find "123456" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "1234" > nul
linechk RT_OUT.TXT 2 "AABB" > nul
linechk RT_OUT.TXT 3 "123456" > nul
if errorlevel 1 goto :test11_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "1234\nAABB\n123456" 
echo.

goto :test12

:test11_fail
echo FAILED: Compilation error
goto :end

:test11_nofile
echo FAILED: DJC file not created
goto :end

:test11_runfail
echo FAILED: Runtime error
goto :end

:test11_badout
echo FAILED: Output mismatch for Test 11
echo Expected: "1234\nAABB\n123456"
echo Actual:
type RT_OUT.TXT
goto :end


:test12

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 12: String in function
echo ------------------------------
..\build\bin\djc.exe strfunc.jav
if errorlevel 1 goto :test12_fail
if not exist STRFUNC.DJC goto :test12_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STRFUNC.DJC > RT_OUT.TXT
if errorlevel 1 goto :test12_runfail
REM find "ABC\nHello ABC\nABCABCABC" RT_OUT.TXT > nul
REM find "ABCABCABC" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "ABC" > nul
linechk RT_OUT.TXT 2 "Hello ABC" > nul
linechk RT_OUT.TXT 3 "ABCABCABC" > nul
if errorlevel 1 goto :test12_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "ABC\nHello ABC\nABCABCABC" 
echo.

goto :test13

:test12_fail
echo FAILED: Compilation error
goto :end

:test12_nofile
echo FAILED: DJC file not created
goto :end

:test12_runfail
echo FAILED: Runtime error
goto :end

:test12_badout
echo FAILED: Output mismatch for Test 12
echo Expected: "ABC\nHello ABC\nABCABCABC"
echo Actual:
type RT_OUT.TXT
goto :end


:test13

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 13: String extend features
echo ------------------------------
..\build\bin\djc.exe strext.jav
if errorlevel 1 goto :test13_fail
if not exist STREXT.DJC goto :test13_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STREXT.DJC > RT_OUT.TXT
if errorlevel 1 goto :test13_runfail
REM find "1\n0\n1\n0\n6\n11\n4\n6\n11\n7\nel\nWorld!" RT_OUT.TXT > nul
REM find "World!" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "HELLO WORLD!" > nul
linechk RT_OUT.TXT 2 "hello world!" > nul
linechk RT_OUT.TXT 3 "1" > nul
linechk RT_OUT.TXT 4 "0" > nul
linechk RT_OUT.TXT 5 "1" > nul
linechk RT_OUT.TXT 6 "0" > nul
linechk RT_OUT.TXT 7 "6" > nul
linechk RT_OUT.TXT 8 "11" > nul
linechk RT_OUT.TXT 9 "4" > nul
linechk RT_OUT.TXT 10 "6" > nul
linechk RT_OUT.TXT 11 "11" > nul
linechk RT_OUT.TXT 12 "7" > nul
linechk RT_OUT.TXT 13 "el" > nul
linechk RT_OUT.TXT 14 "World!" > nul
if errorlevel 1 goto :test13_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "HELLO WORLD\!\nhello world\!\n1\n0\n1\n0\n6\n11\n4\n6\n11\n7\nel\nWorld!" 
echo.

goto :test14

:test13_fail
echo FAILED: Compilation error
goto :end

:test13_nofile
echo FAILED: DJC file not created
goto :end

:test13_runfail
echo FAILED: Runtime error
goto :end

:test13_badout
echo FAILED: Output mismatch for Test 13
echo Expected: "1\n0\n1\n0\n6\n11\n4\n6\n11\n7\nel\nWorld!"
echo Actual:
type RT_OUT.TXT
goto :end


:test14

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 14: File open("r","w","a"), readLine, writeLine
echo ------------------------------
..\build\bin\djc.exe files.jav
if errorlevel 1 goto :test14_fail
if not exist FILES.DJC goto :test14_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe FILES.DJC > RT_OUT.TXT
if errorlevel 1 goto :test14_runfail
REM find "ABC\n123\nHello World!" RT_OUT.TXT > nul
REM find "Hello World!" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "ABC" > nul
linechk RT_OUT.TXT 2 "123" > nul
linechk RT_OUT.TXT 3 "Hello World!" > nul

if errorlevel 1 goto :test14_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "ABC\n123\nHello World!"
echo.

goto :test15

:test14_fail
echo FAILED: Compilation error
goto :end

:test14_nofile
echo FAILED: DJC file not created
goto :end

:test14_runfail
echo FAILED: Runtime error
goto :end

:test14_badout
echo FAILED: Output mismatch for Test 14
echo Expected: "ABC\n123\nHello World!"
echo Actual:
type RT_OUT.TXT
goto :end


:test15

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 15: String.equals()
echo ------------------------------
..\build\bin\djc.exe str6.jav
if errorlevel 1 goto :test15_fail
if not exist STR6.DJC goto :test15_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STR6.DJC > RT_OUT.TXT
if errorlevel 1 goto :test15_runfail
REM find "true" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "true" > nul
if errorlevel 1 goto :test15_badout
REM find "false" RT_OUT.TXT > nul
linechk RT_OUT.TXT 2 "false" > nul
if errorlevel 1 goto :test15_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "true\nfalse"
echo.

goto :test16

:test15_fail
echo FAILED: Compilation error
goto :end

:test15_nofile
echo FAILED: DJC file not created
goto :end

:test15_runfail
echo FAILED: Runtime error
goto :end

:test15_badout
echo FAILED: Output mismatch for Test 15
echo Expected: "true\nfalse"
echo Actual:
type RT_OUT.TXT
goto :end


:test16

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 16: String + Integer
echo ------------------------------
..\build\bin\djc.exe strint1.jav
if errorlevel 1 goto :test16_fail
if not exist STRINT1.DJC goto :test16_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe STRINT1.DJC > RT_OUT.TXT
if errorlevel 1 goto :test16_runfail
REM find "Count: 42" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Count: 42" > nul
if errorlevel 1 goto :test16_badout
linechk RT_OUT.TXT 2 "42 items" > nul
if errorlevel 1 goto :test16_badout
REM find "Total: 42 items" RT_OUT.TXT > nul
linechk RT_OUT.TXT 3 "Total: 42 items" > nul
if errorlevel 1 goto :test16_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Count: 42\n42 items\nTotal: 42 items"
echo.

goto :test17

:test16_fail
echo FAILED: Compilation error
goto :end

:test16_nofile
echo FAILED: DJC file not created
goto :end

:test16_runfail
echo FAILED: Runtime error
goto :end

:test16_badout
echo FAILED: Output mismatch for Test 16
echo Expected: "Count: 42\n42 items\nTotal: 42 items"
echo Actual:
type RT_OUT.TXT
goto :end


:test17

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 17: new Object
echo ------------------------------
..\build\bin\djc.exe obj3.jav
if errorlevel 1 goto :test17_fail
if not exist OBJ3.DJC goto :test17_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe OBJ3.DJC > RT_OUT.TXT
if errorlevel 1 goto :test17_runfail
REM find "42" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "42" > nul
if errorlevel 1 goto :test17_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "42"
echo.

goto :test18

:test17_fail
echo FAILED: Compilation error
goto :end

:test17_nofile
echo FAILED: DJC file not created
goto :end

:test17_runfail
echo FAILED: Runtime error
goto :end

:test17_badout
echo FAILED: Output mismatch for Test 17
echo Expected: "42"
echo Actual:
type RT_OUT.TXT
goto :end


:test18

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 18: BuffereedReader/BufferedWriter
echo ------------------------------
..\build\bin\djc.exe bufrw.jav
if errorlevel 1 goto :test18_fail
if not exist bufrw.DJC goto :test18_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe bufrw.DJC > RT_OUT.TXT
if errorlevel 1 goto :test18_runfail
REM find "" RT_OUT.TXT > nul
linechk TEST.TXT 1 "Hello File" > nul
if errorlevel 1 goto :test18_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Hello File"
echo.

goto :test19

:test18_fail
echo FAILED: Compilation error
goto :end

:test18_nofile
echo FAILED: DJC file not created
goto :end

:test18_runfail
echo FAILED: Runtime error
goto :end

:test18_badout
echo FAILED: Output mismatch for Test 18
echo Expected: "Hello File"
echo Actual:
type RT_OUT.TXT
goto :end


:test19

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 19: E2E Write
echo ------------------------------
..\build\bin\djc.exe e2ewrit.jav
if errorlevel 1 goto :test19_fail
if not exist e2ewrit.DJC goto :test19_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe e2ewrit.DJC > RT_OUT.TXT
if errorlevel 1 goto :test19_runfail
REM find "Write test completed" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Write test completed" > nul
if errorlevel 1 goto :test19_badout
linechk E2EWRIT.TXT 1 "Line 1" > nul
if errorlevel 1 goto :test19_badout
linechk E2EWRIT.TXT 2 "Line 2" > nul
if errorlevel 1 goto :test19_badout
linechk E2EWRIT.TXT 3 "Line 3" > nul
if errorlevel 1 goto :test19_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Write test completed"
echo.

goto :test20

:test19_fail
echo FAILED: Compilation error
goto :end

:test19_nofile
echo FAILED: DJC file not created
goto :end

:test19_runfail
echo FAILED: Runtime error
goto :end

:test19_badout
echo FAILED: Output mismatch for Test 19
echo Expected: "Write test completed"
echo Actual:
type RT_OUT.TXT
goto :end


:test20

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 20: E2E Read
echo ------------------------------
..\build\bin\djc.exe e2eread.jav
if errorlevel 1 goto :test20_fail
if not exist e2eread.DJC goto :test20_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe e2eread.DJC > RT_OUT.TXT
if errorlevel 1 goto :test20_runfail
REM find "Read test completed" RT_OUT.TXT > nul
linechk RT_OUT.TXT 1 "Reading file:" > nul
if errorlevel 1 goto :test20_badout
linechk RT_OUT.TXT 2 "Line 1" > nul
if errorlevel 1 goto :test20_badout
linechk RT_OUT.TXT 3 "Line 2" > nul
if errorlevel 1 goto :test20_badout
linechk RT_OUT.TXT 4 "Line 3" > nul
if errorlevel 1 goto :test20_badout
linechk RT_OUT.TXT 5 "Read test completed" > nul
if errorlevel 1 goto :test20_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Read test completed"
echo.

goto :test21

:test20_fail
echo FAILED: Compilation error
goto :end

:test20_nofile
echo FAILED: DJC file not created
goto :end

:test20_runfail
echo FAILED: Runtime error
goto :end

:test20_badout
echo FAILED: Output mismatch for Test 20
echo Expected: "Read test completed"
echo Actual:
type RT_OUT.TXT
goto :end


:test21

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 21: try{}catch{}finally{}
echo ------------------------------
..\build\bin\djc.exe exc2.jav
if errorlevel 1 goto :test21_fail
if not exist exc2.DJC goto :test21_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe exc2.DJC > RT_OUT.TXT
if errorlevel 1 goto :test21_runfail
REM find "  Result: 20" RT_OUT.TXT > nul
linechk RT_OUT.TXT 5 "  Exception caught" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 6 "  Status: 3" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 12 "  Exception caught, counter: 1" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 13 "  Final counter: 1" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 17 "  Throwing exception" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 18 "  Handling exception" > nul
if errorlevel 1 goto :test21_badout
linechk RT_OUT.TXT 19 "  Result: 20" > nul
if errorlevel 1 goto :test21_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "  Result: 20"
echo.

goto :test22

:test21_fail
echo FAILED: Compilation error
goto :end

:test21_nofile
echo FAILED: DJC file not created
goto :end

:test21_runfail
echo FAILED: Runtime error
goto :end

:test21_badout
echo FAILED: Output mismatch for Test 21
echo Expected: "  Result: 20"
echo Actual:
type RT_OUT.TXT
goto :end


:test22

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 22: try{}catch{}finally{}
echo ------------------------------
..\build\bin\djc.exe exc3.jav
if errorlevel 1 goto :test22_fail
if not exist exc3.DJC goto :test22_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe exc3.DJC > RT_OUT.TXT
if errorlevel 1 goto :test22_runfail
REM find "  Result: 20" RT_OUT.TXT > nul
linechk RT_OUT.TXT 5 "  Exception caught" > nul
if errorlevel 1 goto :test22_badout
linechk RT_OUT.TXT 6 "  Status: 3" > nul
if errorlevel 1 goto :test22_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "  Status: 3"
echo.

goto :test23

:test22_fail
echo FAILED: Compilation error
goto :end

:test22_nofile
echo FAILED: DJC file not created
goto :end

:test22_runfail
echo FAILED: Runtime error
goto :end

:test22_badout
echo FAILED: Output mismatch for Test 22
echo Expected: "  Result: 20"
echo Actual:


:test23

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 23: try{}catch{}finally{}
echo ------------------------------
..\build\bin\djc.exe exc4.jav
if errorlevel 1 goto :test23_fail
if not exist exc4.DJC goto :test23_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe exc4.DJC > RT_OUT.TXT
if errorlevel 1 goto :test23_runfail
linechk RT_OUT.TXT 1 "Exception caught" > nul
if errorlevel 1 goto :test23_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "Exception caught"
echo.

goto :test24

:test23_fail
echo FAILED: Compilation error
goto :end

:test23_nofile
echo FAILED: DJC file not created
goto :end

:test23_runfail
echo FAILED: Runtime error
goto :end

:test23_badout
echo FAILED: Output mismatch for Test 23
echo Expected: "Exception caught"
echo Actual:


:test24

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 24: Date
echo ------------------------------
..\build\bin\djc.exe dtest4.jav
if errorlevel 1 goto :test24_fail
if not exist dtest4.DJC goto :test24_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe dtest4.DJC > RT_OUT.TXT
if errorlevel 1 goto :test24_runfail
find "20000" RT_OUT.TXT > nul
if errorlevel 1 goto :test24_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "20000"
echo.

goto :test25

:test24_fail
echo FAILED: Compilation error
goto :end

:test24_nofile
echo FAILED: DJC file not created
goto :end

:test24_runfail
echo FAILED: Runtime error
goto :end

:test24_badout
echo FAILED: Output mismatch for Test 24
echo Expected: "20000"
echo Actual:
type RT_OUT.TXT
goto :end


:test25

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 25: Long
echo ------------------------------
..\build\bin\djc.exe tlonarr.jav
if errorlevel 1 goto :test25_fail
if not exist tlonarr.DJC goto :test25_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe tlonarr.DJC > RT_OUT.TXT
if errorlevel 1 goto :test25_runfail
linechk RT_OUT.TXT 5 "arr[0] = 1000000" > nul
if errorlevel 1 goto :test25_badout
linechk RT_OUT.TXT 10 "Sum = 15000000" > nul
if errorlevel 1 goto :test25_badout
linechk RT_OUT.TXT 11 "Array length: 5" > nul
if errorlevel 1 goto :test25_badout
linechk RT_OUT.TXT 12 "Modified arr[2] = 9999999" > nul
if errorlevel 1 goto :test25_badout
linechk RT_OUT.TXT 13 "arr[0] + arr[1] = 3000000" > nul
if errorlevel 1 goto :test25_badout
linechk RT_OUT.TXT 14 "arr[4] - arr[3] = 1000000" > nul
if errorlevel 1 goto :test25_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "arr[4] - arr[3] = 1000000"
echo.

goto :test26

:test25_fail
echo FAILED: Compilation error
goto :end

:test25_nofile
echo FAILED: DJC file not created
goto :end

:test25_runfail
echo FAILED: Runtime error
goto :end

:test25_badout
echo FAILED: Output mismatch for Test 25
echo Expected: "arr[4] - arr[3] = 1000000"
echo Actual:
type RT_OUT.TXT
goto :end


:test26

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 26: Date setTime/getTime
echo ------------------------------
..\build\bin\djc.exe tdatlong.jav
if errorlevel 1 goto :test26_fail
if not exist tdatlong.DJC goto :test26_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe tdatlong.DJC > RT_OUT.TXT
if errorlevel 1 goto :test26_runfail
linechk RT_OUT.TXT 1 "=== Date Long Type Test ===" > nul
if errorlevel 1 goto :test26_badout
linechk RT_OUT.TXT 6 "YEAR: 2026" > nul
if errorlevel 1 goto :test26_badout
linechk RT_OUT.TXT 25 "Second precision: OK" > nul
if errorlevel 1 goto :test26_badout
linechk RT_OUT.TXT 31 "=== All tests completed ===" > nul
if errorlevel 1 goto :test26_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "=== All tests completed ==="
echo.

goto :test27

:test26_fail
echo FAILED: Compilation error
goto :end

:test26_nofile
echo FAILED: DJC file not created
goto :end

:test26_runfail
echo FAILED: Runtime error
goto :end

:test26_badout
echo FAILED: Output mismatch for Test 26
echo Expected: "=== All tests completed ==="
echo Actual:
type RT_OUT.TXT
goto :end


:test27

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 27: Float(1)
echo ------------------------------
..\build\bin\djc.exe ftst1.jav
if errorlevel 1 goto :test27_fail
if not exist ftst1.DJC goto :test27_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ftst1.DJC > RT_OUT.TXT
if errorlevel 1 goto :test27_runfail
linechk RT_OUT.TXT 1 "5.14" > nul
if errorlevel 1 goto :test27_badout
linechk RT_OUT.TXT 2 "1.14" > nul
if errorlevel 1 goto :test27_badout
linechk RT_OUT.TXT 3 "6.28" > nul
if errorlevel 1 goto :test27_badout
linechk RT_OUT.TXT 4 "1.57" > nul
if errorlevel 1 goto :test27_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "1.57"
echo.

goto :test28

:test27_fail
echo FAILED: Compilation error
goto :end

:test27_nofile
echo FAILED: DJC file not created
goto :end

:test27_runfail
echo FAILED: Runtime error
goto :end

:test27_badout
echo FAILED: Output mismatch for Test 27
echo Expected: "1.57"
echo Actual:
type RT_OUT.TXT
goto :end


:test28

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 28: Float(2)
echo ------------------------------
..\build\bin\djc.exe ftst2.jav
if errorlevel 1 goto :test28_fail
if not exist ftst2.DJC goto :test28_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe ftst2.DJC > RT_OUT.TXT
if errorlevel 1 goto :test28_runfail
linechk RT_OUT.TXT 1 "1.50" > nul
if errorlevel 1 goto :test28_badout
linechk RT_OUT.TXT 2 "10.00" > nul
if errorlevel 1 goto :test28_badout
linechk RT_OUT.TXT 3 "3.70" > nul
if errorlevel 1 goto :test28_badout
linechk RT_OUT.TXT 4 "1" > nul
if errorlevel 1 goto :test28_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "1"
echo.

goto :test29

:test28_fail
echo FAILED: Compilation error
goto :end

:test28_nofile
echo FAILED: DJC file not created
goto :end

:test28_runfail
echo FAILED: Runtime error
goto :end

:test28_badout
echo FAILED: Output mismatch for Test 28
echo Expected: "1"
echo Actual:
type RT_OUT.TXT
goto :end


:test29

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 29: Float Array(1)
echo ------------------------------
..\build\bin\djc.exe farr2.jav
if errorlevel 1 goto :test29_fail
if not exist farr2.DJC goto :test29_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe farr2.DJC > RT_OUT.TXT
if errorlevel 1 goto :test29_runfail
linechk RT_OUT.TXT 1 "1.00" > nul
if errorlevel 1 goto :test29_badout
linechk RT_OUT.TXT 2 "2.00" > nul
if errorlevel 1 goto :test29_badout
linechk RT_OUT.TXT 3 "3.00" > nul
if errorlevel 1 goto :test29_badout
linechk RT_OUT.TXT 4 "4.00" > nul
if errorlevel 1 goto :test29_badout
linechk RT_OUT.TXT 5 "5.00" > nul
if errorlevel 1 goto :test29_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "5.00"
echo.

goto :test30

:test29_fail
echo FAILED: Compilation error
goto :end

:test29_nofile
echo FAILED: DJC file not created
goto :end

:test29_runfail
echo FAILED: Runtime error
goto :end

:test29_badout
echo FAILED: Output mismatch for Test 29
echo Expected: "5.00"
echo Actual:
type RT_OUT.TXT
goto :end


:test30

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 30: Float Array(2)
echo ------------------------------
..\build\bin\djc.exe farr3.jav
if errorlevel 1 goto :test30_fail
if not exist farr3.DJC goto :test30_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe farr3.DJC > RT_OUT.TXT
if errorlevel 1 goto :test30_runfail
linechk RT_OUT.TXT 1 "7.50" > nul
if errorlevel 1 goto :test30_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "7.50"
echo.

goto :test31

:test30_fail
echo FAILED: Compilation error
goto :end

:test30_nofile
echo FAILED: DJC file not created
goto :end

:test30_runfail
echo FAILED: Runtime error
goto :end

:test30_badout
echo FAILED: Output mismatch for Test 30
echo Expected: "7.50"
echo Actual:
type RT_OUT.TXT
goto :end


:test31

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 31: Math Basic
echo ------------------------------
..\build\bin\djc.exe mathbas.jav
if errorlevel 1 goto :test31_fail
if not exist mathbas.DJC goto :test31_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe mathbas.DJC > RT_OUT.TXT
if errorlevel 1 goto :test31_runfail
linechk RT_OUT.TXT 2 "3.50" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 3 "3.50" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 5 "2.10" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 6 "1.00" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 8 "3.50" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 9 "5.00" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 11 "3.00" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 12 "4.00" > nul
if errorlevel 1 goto :test31_badout
linechk RT_OUT.TXT 13 "1.41" > nul
if errorlevel 1 goto :test31_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "1.41"
echo.

goto :test32

:test31_fail
echo FAILED: Compilation error
goto :end

:test31_nofile
echo FAILED: DJC file not created
goto :end

:test31_runfail
echo FAILED: Runtime error
goto :end

:test31_badout
echo FAILED: Output mismatch for Test 31
echo Expected: "1.41"
echo Actual:
type RT_OUT.TXT
goto :end


:test32

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 32: Math Triangle
echo ------------------------------
..\build\bin\djc.exe mathtrig.jav
if errorlevel 1 goto :test32_fail
if not exist mathtrig.DJC goto :test32_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe mathtrig.DJC > RT_OUT.TXT
if errorlevel 1 goto :test32_runfail
linechk RT_OUT.TXT 2 "0.00" > nul
if errorlevel 1 goto :test32_badout
linechk RT_OUT.TXT 3 "1.00" > nul
if errorlevel 1 goto :test32_badout
linechk RT_OUT.TXT 5 "1.00" > nul
if errorlevel 1 goto :test32_badout
linechk RT_OUT.TXT 6 "0.00" > nul
if errorlevel 1 goto :test32_badout
linechk RT_OUT.TXT 8 "0.00" > nul
if errorlevel 1 goto :test32_badout
linechk RT_OUT.TXT 9 "0.99" > nul
if errorlevel 1 goto :test32_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "0.99"
echo.

goto :test33

:test32_fail
echo FAILED: Compilation error
goto :end

:test32_nofile
echo FAILED: DJC file not created
goto :end

:test32_runfail
echo FAILED: Runtime error
goto :end

:test32_badout
echo FAILED: Output mismatch for Test 32
echo Expected: "0.99"
echo Actual:
type RT_OUT.TXT
goto :end


:test33

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 33: Math Exp
echo ------------------------------
..\build\bin\djc.exe mathexp.jav
if errorlevel 1 goto :test33_fail
if not exist mathexp.DJC goto :test33_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe mathexp.DJC > RT_OUT.TXT
if errorlevel 1 goto :test33_runfail
linechk RT_OUT.TXT 2 "8.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 3 "100.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 4 "1.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 6 "1.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 7 "2.72" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 8 "7.39" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 10 "0.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 11 "1.00" > nul
if errorlevel 1 goto :test33_badout
linechk RT_OUT.TXT 12 "2.30" > nul
if errorlevel 1 goto :test33_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "2.30"
echo.

goto :test34

:test33_fail
echo FAILED: Compilation error
goto :end

:test33_nofile
echo FAILED: DJC file not created
goto :end

:test33_runfail
echo FAILED: Runtime error
goto :end

:test33_badout
echo FAILED: Output mismatch for Test 33
echo Expected: "2.30"
echo Actual:
type RT_OUT.TXT
goto :end


:test34

if exist RT_OUT.TXT del RT_OUT.TXT
echo Test 34: Math All
echo ------------------------------
..\build\bin\djc.exe mathall.jav
if errorlevel 1 goto :test34_fail
if not exist mathall.DJC goto :test34_nofile
echo SUCCESS: Compilation passed
..\build\bin\djvm.exe mathall.DJC > RT_OUT.TXT
if errorlevel 1 goto :test34_runfail
linechk RT_OUT.TXT 2 "9.00" > nul
if errorlevel 1 goto :test34_badout
linechk RT_OUT.TXT 3 "16.00" > nul
if errorlevel 1 goto :test34_badout
linechk RT_OUT.TXT 4 "25.00" > nul
if errorlevel 1 goto :test34_badout
linechk RT_OUT.TXT 5 "5.00" > nul
if errorlevel 1 goto :test34_badout
linechk RT_OUT.TXT 7 "78.50" > nul
if errorlevel 1 goto :test34_badout
linechk RT_OUT.TXT 9 "5.00" > nul
if errorlevel 1 goto :test34_badout
echo Output:
type RT_OUT.TXT
echo SUCCESS: Output matched expected value "78.5, 5.00"
echo.

goto :completed

:test34_fail
echo FAILED: Compilation error
goto :end

:test34_nofile
echo FAILED: DJC file not created
goto :end

:test34_runfail
echo FAILED: Runtime error
goto :end

:test34_badout
echo FAILED: Output mismatch for Test 34
echo Expected: "78.5, 5.00"
echo Actual:
type RT_OUT.TXT
goto :end


:completed

echo ========================================
echo All tests completed!
echo ========================================

:end

