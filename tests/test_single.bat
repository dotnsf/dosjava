@echo off
REM Single Test Runner for DOSBox
REM Usage: test_single.bat <test_file.jav>

if "%1"=="" (
    echo Usage: test_single.bat ^<test_file.jav^>
    echo Example: test_single.bat var1.jav
    exit /b 1
)

set TEST_FILE=%1
set DOSBOX_PATH=C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe

if not exist "%TEST_FILE%" (
    echo ERROR: Test file not found: %TEST_FILE%
    exit /b 1
)

if not exist "%DOSBOX_PATH%" (
    echo ERROR: DOSBox not found at: %DOSBOX_PATH%
    exit /b 1
)

REM Get base name without extension
for %%F in ("%TEST_FILE%") do set BASENAME=%%~nF

echo ========================================
echo Testing: %TEST_FILE%
echo ========================================
echo.

REM Create DOSBox configuration
set TEST_DIR=%~dp0
set PROJECT_DIR=%TEST_DIR%..

(
echo [autoexec]
echo @echo off
echo mount c "%PROJECT_DIR%"
echo c:
echo cd tests
echo echo.
echo echo Compiling %TEST_FILE%...
echo ..\build\bin\djc.exe %TEST_FILE%
echo if errorlevel 1 goto error
echo if not exist %BASENAME%.DJC goto nofile
echo echo.
echo echo Compilation successful!
echo echo Running %BASENAME%.DJC...
echo echo.
echo ..\build\bin\djvm.exe %BASENAME%.DJC
echo if errorlevel 1 goto runerror
echo echo.
echo echo Execution successful!
echo goto end
echo :error
echo echo ERROR: Compilation failed
echo goto end
echo :nofile
echo echo ERROR: Output file not created
echo goto end
echo :runerror
echo echo ERROR: Runtime error
echo :end
echo echo.
echo pause
echo exit
) > "%TEST_DIR%dosbox_single.conf"

"%DOSBOX_PATH%" -conf "%TEST_DIR%dosbox_single.conf" -noconsole

if exist "%TEST_DIR%dosbox_single.conf" del "%TEST_DIR%dosbox_single.conf"

echo.
echo Test complete.
pause

@REM Made with Bob
