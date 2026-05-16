@echo off
echo ========================================
echo Date Test 4: Date(int), setTime(), getTime()
echo ========================================

echo.
echo Compiling dtest4.jav...
djc.exe dtest4.jav
if errorlevel 1 goto error

echo.
echo Running dtest4.djc...
djvm.exe dtest4.djc
if errorlevel 1 goto error

echo.
echo ========================================
echo Day 4 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 4 Test FAILED
echo ========================================

:end
