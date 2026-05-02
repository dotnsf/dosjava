@echo off
REM DOSBox Test Automation Script for DOS Java Compiler
REM This script runs the test suite in DOSBox

echo ========================================
echo DOS Java - DOSBox Test Runner
echo ========================================
echo.

REM Check if DOSBox is installed
set DOSBOX_PATH=C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe
if not exist "%DOSBOX_PATH%" (
    echo ERROR: DOSBox not found at: %DOSBOX_PATH%
    echo.
    echo Please install DOSBox from: https://www.dosbox.com/
    echo Or update DOSBOX_PATH in this script to point to your DOSBox installation.
    echo.
    pause
    exit /b 1
)

REM Get the current directory
set TEST_DIR=%~dp0
set PROJECT_DIR=%TEST_DIR%..

echo Test Directory: %TEST_DIR%
echo Project Directory: %PROJECT_DIR%
echo.

REM Create DOSBox configuration file
echo Creating DOSBox configuration...
(
echo [autoexec]
echo @echo off
echo mount c "%PROJECT_DIR%"
echo c:
echo cd tests
echo echo.
echo echo ========================================
echo echo Running DOS Java Test Suite
echo echo ========================================
echo echo.
echo call runtest.bat
echo echo.
echo echo ========================================
echo echo Test execution complete
echo echo ========================================
echo echo.
echo echo Press any key to exit DOSBox...
echo pause ^> nul
echo exit
) > "%TEST_DIR%dosbox_test.conf"

echo Starting DOSBox with test suite...
echo.

REM Run DOSBox with the configuration
"%DOSBOX_PATH%" -conf "%TEST_DIR%dosbox_test.conf" -noconsole

REM Clean up
if exist "%TEST_DIR%dosbox_test.conf" del "%TEST_DIR%dosbox_test.conf"

echo.
echo DOSBox test session ended.
echo Check the DOSBox window for test results.
echo.
pause

@REM Made with Bob
