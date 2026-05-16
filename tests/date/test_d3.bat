@echo off
echo ========================================
echo Phase 3.5.3 Day 3: VM OP_INVOKE_VIRTUAL Test
echo ========================================
echo.
echo Compiling dtest3.jav...
..\..\build\bin\djc.exe dtest3.jav
if errorlevel 1 goto error
echo.
echo Running dtest3.djc...
..\..\build\bin\djvm.exe dtest3.djc
if errorlevel 1 goto error
echo.
echo Expected output:
echo   Year: 2024
echo   Month: 0 (January)
echo   Day: 15
echo   Hours: 12
echo   Minutes: 30
echo   Seconds: 45
echo.
echo ========================================
echo Day 3 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 3 Test FAILED
echo ========================================

:end
