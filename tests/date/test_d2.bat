@echo off
echo ========================================
echo Phase 3.5.3 Day 2: VM OP_NEW Test
echo ========================================
echo.
echo Compiling dtest2.jav...
..\..\build\bin\djc.exe dtest2.jav
if errorlevel 1 goto error
echo.
echo Running dtest2.djc...
..\..\build\bin\djvm.exe dtest2.djc
if errorlevel 1 goto error
echo.
echo ========================================
echo Day 2 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 2 Test FAILED
echo ========================================

:end
