@echo off
echo ========================================
echo Date Test 6: Date Formatting
echo ========================================

echo.
echo Compiling dtest6.jav...
djc.exe dtest6.jav
if errorlevel 1 goto error

echo.
echo Running dtest6.djc...
djvm.exe dtest6.djc
if errorlevel 1 goto error

echo.
echo ========================================
echo Day 6 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 6 Test FAILED
echo ========================================

:end
