@echo off
REM Test script for Date class with long type support
REM Tests getTime() and setTime() with millisecond precision

echo ========================================
echo Date Long Type Test
echo ========================================
echo.

echo Building compiler and VM...
cd ..
call build_djc.bat
if errorlevel 1 (
    echo ERROR: Compiler build failed
    exit /b 1
)

call build_vm.bat
if errorlevel 1 (
    echo ERROR: VM build failed
    exit /b 1
)

echo.
echo Compiling tdatlong.jav...
echo ----------------------------------------
build\bin\djc.exe tests\tdatlong.jav
if errorlevel 1 (
    echo ERROR: Compilation failed
    exit /b 1
)

echo.
echo Running tdatlong.djc...
echo ----------------------------------------
build\bin\djvm.exe tests\tdatlong.djc

echo.
echo ========================================
echo Test completed
echo ========================================
