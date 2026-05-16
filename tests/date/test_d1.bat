@echo off
echo ========================================
echo Phase 3.5.3 Day 1: Semantic Analyzer Test
echo ========================================
echo.
echo Compiling dtest1.jav...
..\..\build\bin\djc.exe dtest1.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo dtest1.djc created.
echo.
echo ========================================
echo Day 1 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 1 Test FAILED
echo ========================================

:end
