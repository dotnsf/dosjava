@echo off
echo ========================================
echo Running All Exception Tests
echo ========================================
echo.

set PASS=0
set FAIL=0

echo Test 1: excnull.jav (NullPointerException)
djc.exe excnull.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test2
)
djvm.exe excnull.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test2
echo Test 2: excarr.jav (ArrayIndexOutOfBoundsException)
djc.exe excarr.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test3
)
djvm.exe excarr.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test3
echo Test 3: excnum.jav (NumberFormatException)
djc.exe excnum.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test4
)
djvm.exe excnum.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test4
echo Test 4: excarg.jav (IllegalArgumentException)
djc.exe excarg.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test5
)
djvm.exe excarg.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test5
echo Test 5: excstr.jav (StringIndexOutOfBoundsException)
djc.exe excstr.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test6
)
djvm.exe excstr.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test6
echo Test 6: excmsg.jav (Exception Message Retrieval)
djc.exe excmsg.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto test7
)
djvm.exe excmsg.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:test7
echo Test 7: excall.jav (Comprehensive Test)
djc.exe excall.jav
if errorlevel 1 (
    echo FAIL: Compilation failed
    set /a FAIL=%FAIL%+1
    goto summary
)
djvm.exe excall.djc
if errorlevel 1 (
    echo FAIL: Execution failed
    set /a FAIL=%FAIL%+1
) else (
    echo PASS
    set /a PASS=%PASS%+1
)
echo.

:summary
echo ========================================
echo Test Summary
echo ========================================
echo Passed: %PASS%
echo Failed: %FAIL%
echo Total:  7
echo ========================================

if %FAIL% == 0 (
    echo All tests PASSED!
    exit /b 0
) else (
    echo Some tests FAILED
    exit /b 1
)
