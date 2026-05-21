@echo off
REM Test script for Long type operations
REM Tests all Phase 5.2 implementations
REM Note: Uses bytecode format (.djc) directly, not Java source

echo ========================================
echo Long Type Test Suite (Bytecode)
echo ========================================
echo.

echo Building VM...
cd ..
call build_vm.bat
if errorlevel 1 (
    echo ERROR: VM build failed
    exit /b 1
)

echo.
echo Running longtest.djc (bytecode)...
echo ----------------------------------------
build\bin\djvm.exe tests\longtest.djc

echo.
echo ========================================
echo Test completed
echo ========================================
echo.
echo Expected output: 26 lines of numbers
echo (pairs of high/low words for long values)

@REM Made with Bob
