@echo off
REM runsock.bat - Run socket tests in DOSBox-X
REM Usage: runsock.bat [test_name]
REM   test_name: sockinit, sockconn, socksend (default: sockinit)

set TEST=%1
if "%TEST%"=="" set TEST=sockinit

echo ========================================
echo Socket Test Runner
echo ========================================
echo.
echo Test: %TEST%
echo.

REM Compile the test
echo Compiling %TEST%.jav...
..\build\bin\djc.exe %TEST%.jav
if errorlevel 1 goto error

echo.
echo Running %TEST%.djc...
..\build\bin\djvm.exe %TEST%.djc
if errorlevel 1 goto error

echo.
echo ========================================
echo Test completed successfully
echo ========================================
goto end

:error
echo.
echo ========================================
echo Test failed!
echo ========================================

:end

@REM Made with Bob
