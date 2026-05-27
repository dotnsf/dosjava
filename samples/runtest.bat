@echo off
REM DOSJava Feature Verification Script for 16-bit DOS
REM Tests all implemented features or a specific test file

if not "%1"=="" goto single_test

REM Run all tests
echo DOSJava Feature Verification
echo ============================
echo.

REM Initialize RESULT.TXT for all tests
echo DOSJava Feature Verification > RESULT.TXT
echo ============================ >> RESULT.TXT
echo. >> RESULT.TXT

REM Set RETURN to continue for all tests
set RETURN=continue
set FIRST_TEST=yes

REM Test Phase 1
set TESTFILE=hello.jav
set BASENAME=hello
goto run_test
:next1
set TESTFILE=int.jav
set BASENAME=int
goto run_test
:next2
set TESTFILE=bool.jav
set BASENAME=bool
goto run_test
:next3
set TESTFILE=if.jav
set BASENAME=if
goto run_test
:next4
set TESTFILE=while.jav
set BASENAME=while
goto run_test
:next5
set TESTFILE=arith.jav
set BASENAME=arith
goto run_test
:next6
set TESTFILE=compare.jav
set BASENAME=compare
goto run_test
:next7
set TESTFILE=logic.jav
set BASENAME=logic
goto run_test
:next8
set TESTFILE=println.jav
set BASENAME=println
goto run_test

REM Test Phase 2
:next9
set TESTFILE=string.jav
set BASENAME=string
goto run_test
:next9
set TESTFILE=strops.jav
set BASENAME=strops
goto run_test

REM Test Phase 3
:next10
set TESTFILE=intarr.jav
set BASENAME=intarr
goto run_test
:next11
set TESTFILE=boolarr.jav
set BASENAME=boolarr
goto run_test
:next12
set TESTFILE=fileread.jav
set BASENAME=fileread
goto run_test
:next13
set TESTFILE=filewrit.jav
set BASENAME=filewrit
goto run_test

REM Test Phase 5
:next14
set TESTFILE=long.jav
set BASENAME=long
goto run_test
:next15
set TESTFILE=longarr.jav
set BASENAME=longarr
goto run_test

REM Test Phase 6
:next16
set TESTFILE=float.jav
set BASENAME=float
goto run_test
:next17
set TESTFILE=floatarr.jav
set BASENAME=floatarr
goto run_test
:next18
set TESTFILE=cast.jav
set BASENAME=cast
goto run_test
:next19
set TESTFILE=mathint.jav
set BASENAME=mathint
goto run_test
:next20
set TESTFILE=nulltest.jav
set BASENAME=nulltest
goto run_test
:next21
set TESTFILE=math.jav
set BASENAME=math
goto run_test
:next22
set TESTFILE=trig.jav
set BASENAME=trig
goto run_test
:next23
set TESTFILE=explog.jav
set BASENAME=explog
goto run_test

REM Test Phase 7
:next24
set TESTFILE=for.jav
set BASENAME=for
goto run_test
:next25
set TESTFILE=trycatch.jav
set BASENAME=trycatch
goto run_test
:next26
set TESTFILE=throw.jav
set BASENAME=throw
goto run_test
:next27
set TESTFILE=swint.jav
set BASENAME=swint
goto run_test
:next28
set TESTFILE=swlong.jav
set BASENAME=swlong
goto run_test
:next29
set TESTFILE=swstring.jav
set BASENAME=swstring
goto run_test
:next30
set TESTFILE=swnest.jav
set BASENAME=swnest
goto run_test
:next31
set TESTFILE=varinit.jav
set BASENAME=varinit
goto run_test

:all_done
echo.
echo ============================
echo All tests completed
echo ============================
echo. >> RESULT.TXT
echo ============================ >> RESULT.TXT
echo All tests completed >> RESULT.TXT
echo ============================ >> RESULT.TXT
goto end

:single_test
REM Run single test - extract basename manually
echo Running single test: %1

REM Initialize RESULT.TXT for single test
echo Running single test: %1 > RESULT.TXT
echo. >> RESULT.TXT

set TESTFILE=%1
set RETURN=end
set FIRST_TEST=yes
REM Set basename based on filename
if "%1"=="hello.jav" set BASENAME=hello
if "%1"=="int.jav" set BASENAME=int
if "%1"=="bool.jav" set BASENAME=bool
if "%1"=="if.jav" set BASENAME=if
if "%1"=="while.jav" set BASENAME=while
if "%1"=="arith.jav" set BASENAME=arith
if "%1"=="compare.jav" set BASENAME=compare
if "%1"=="logic.jav" set BASENAME=logic
if "%1"=="println.jav" set BASENAME=println
if "%1"=="string.jav" set BASENAME=string
if "%1"=="strops.jav" set BASENAME=strops
if "%1"=="intarr.jav" set BASENAME=intarr
if "%1"=="boolarr.jav" set BASENAME=boolarr
if "%1"=="fileread.jav" set BASENAME=fileread
if "%1"=="filewrit.jav" set BASENAME=filewrit
if "%1"=="long.jav" set BASENAME=long
if "%1"=="longarr.jav" set BASENAME=longarr
if "%1"=="float.jav" set BASENAME=float
if "%1"=="floatarr.jav" set BASENAME=floatarr
if "%1"=="cast.jav" set BASENAME=cast
if "%1"=="mathint.jav" set BASENAME=mathint
if "%1"=="nulltest.jav" set BASENAME=nulltest
if "%1"=="math.jav" set BASENAME=math
if "%1"=="trig.jav" set BASENAME=trig
if "%1"=="explog.jav" set BASENAME=explog
if "%1"=="for.jav" set BASENAME=for
if "%1"=="trycatch.jav" set BASENAME=trycatch
if "%1"=="throw.jav" set BASENAME=throw
if "%1"=="swint.jav" set BASENAME=swint
if "%1"=="swlong.jav" set BASENAME=swlong
if "%1"=="swstring.jav" set BASENAME=swstring
if "%1"=="swnest.jav" set BASENAME=swnest
if "%1"=="varinit.jav" set BASENAME=varinit
goto run_test

:run_test
if not exist %TESTFILE% goto skip_test

REM Write test header to RESULT.TXT (append mode)
echo [TEST] %TESTFILE% >> RESULT.TXT
echo [TEST] %TESTFILE%
set FIRST_TEST=no

REM Compile the test
djc.exe %TESTFILE% > nul
if errorlevel 1 goto compile_fail

REM Check if .djc file was created
if not exist %BASENAME%.djc goto no_djc

REM Run the test and save output
djvm.exe %BASENAME%.djc > testout.tmp
if errorlevel 1 goto vm_fail

REM Append output to RESULT.TXT and show on screen
type testout.tmp >> RESULT.TXT
type testout.tmp
del testout.tmp > nul
echo [PASS] %TESTFILE% >> RESULT.TXT
echo [PASS] %TESTFILE%
echo. >> RESULT.TXT
echo.
goto next_test

:skip_test
echo [SKIP] %TESTFILE% - File not found >> RESULT.TXT
echo [SKIP] %TESTFILE% - File not found
echo. >> RESULT.TXT
echo.
set FIRST_TEST=no
goto next_test

:compile_fail
echo [FAIL] %TESTFILE% - Compilation failed >> RESULT.TXT
echo [FAIL] %TESTFILE% - Compilation failed
echo. >> RESULT.TXT
echo.
REM Clean up .djc file if it exists
if exist %BASENAME%.djc del %BASENAME%.djc > nul
set FIRST_TEST=no
goto next_test

:no_djc
echo [FAIL] %TESTFILE% - No .djc file generated >> RESULT.TXT
echo [FAIL] %TESTFILE% - No .djc file generated
echo. >> RESULT.TXT
echo.
REM Clean up .djc file if it exists
if exist %BASENAME%.djc del %BASENAME%.djc > nul
set FIRST_TEST=no
goto next_test

:vm_fail
if exist testout.tmp type testout.tmp >> RESULT.TXT
if exist testout.tmp type testout.tmp
if exist testout.tmp del testout.tmp > nul
echo [FAIL] %TESTFILE% - VM execution failed >> RESULT.TXT
echo [FAIL] %TESTFILE% - VM execution failed
echo. >> RESULT.TXT
echo.
REM Clean up .djc file
if exist %BASENAME%.djc del %BASENAME%.djc > nul
set FIRST_TEST=no
goto next_test

:next_test
REM If single test mode, exit after test
if "%RETURN%"=="end" goto end
REM Otherwise continue to next test
if "%TESTFILE%"=="hello.jav" goto next1
if "%TESTFILE%"=="int.jav" goto next2
if "%TESTFILE%"=="bool.jav" goto next3
if "%TESTFILE%"=="if.jav" goto next4
if "%TESTFILE%"=="while.jav" goto next5
if "%TESTFILE%"=="arith.jav" goto next6
if "%TESTFILE%"=="compare.jav" goto next7
if "%TESTFILE%"=="logic.jav" goto next8
if "%TESTFILE%"=="println.jav" goto next9
if "%TESTFILE%"=="string.jav" goto next10
if "%TESTFILE%"=="strops.jav" goto next11
if "%TESTFILE%"=="intarr.jav" goto next12
if "%TESTFILE%"=="boolarr.jav" goto next13
if "%TESTFILE%"=="fileread.jav" goto next14
if "%TESTFILE%"=="filewrit.jav" goto next15
if "%TESTFILE%"=="long.jav" goto next16
if "%TESTFILE%"=="longarr.jav" goto next17
if "%TESTFILE%"=="float.jav" goto next18
if "%TESTFILE%"=="floatarr.jav" goto next19
if "%TESTFILE%"=="cast.jav" goto next20
if "%TESTFILE%"=="mathint.jav" goto next21
if "%TESTFILE%"=="nulltest.jav" goto next22
if "%TESTFILE%"=="math.jav" goto next23
if "%TESTFILE%"=="trig.jav" goto next24
if "%TESTFILE%"=="explog.jav" goto next25
if "%TESTFILE%"=="for.jav" goto next26
if "%TESTFILE%"=="trycatch.jav" goto next27
if "%TESTFILE%"=="throw.jav" goto next28
if "%TESTFILE%"=="swint.jav" goto next29
if "%TESTFILE%"=="swlong.jav" goto next30
if "%TESTFILE%"=="swstring.jav" goto next31
if "%TESTFILE%"=="swnest.jav" goto next31
if "%TESTFILE%"=="varinit.jav" goto all_done
goto all_done

:end
