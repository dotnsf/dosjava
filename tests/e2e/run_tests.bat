@echo off
setlocal enabledelayedexpansion

echo ========================================
echo DOS Java Compiler - End-to-End Tests
echo ========================================
echo.

set PASSED=0
set FAILED=0
set TOTAL=0
set DJC=..\..\build\bin\djc.exe

rem Check if djc.exe exists
if not exist %DJC% (
    echo Error: djc.exe not found at %DJC%
    echo Please build the compiler first: wmake djc
    exit /b 1
)

rem Basic tests
echo [1/4] Running basic tests...
call :run_test_dir basic
echo.

rem Arithmetic tests
echo [2/4] Running arithmetic tests...
call :run_test_dir arithmetic
echo.

rem Control flow tests
echo [3/4] Running control flow tests...
call :run_test_dir control
echo.

rem Complex tests
echo [4/4] Running complex tests...
call :run_test_dir complex
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
    echo   Testing %%f...
    
    %DJC% %%f > nul 2>&1
    
    if errorlevel 1 (
        echo     FAIL: %%f (compilation failed)
        set /a FAILED+=1
    ) else (
        set BASENAME=%%~nf
        if exist !BASENAME!.djc (
            echo     PASS: %%f
            set /a PASSED+=1
            del !BASENAME!.djc
            if exist !BASENAME!.tok del !BASENAME!.tok
            if exist !BASENAME!.ast del !BASENAME!.ast
            if exist !BASENAME!.sym del !BASENAME!.sym
        ) else (
            echo     FAIL: %%f (no output file)
            set /a FAILED+=1
        )
    )
)

popd
goto :eof

@REM Made with Bob
