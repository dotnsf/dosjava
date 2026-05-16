@echo off
echo ========================================
echo Date Test 5: Multiple Date Objects
echo ========================================

echo.
echo Compiling dtest5.jav...
djc.exe dtest5.jav
if errorlevel 1 goto error

echo.
echo Running dtest5.djc...
djvm.exe dtest5.djc
if errorlevel 1 goto error

echo.
echo ========================================
echo Day 5 Test PASSED
echo ========================================
goto end

:error
echo.
echo ========================================
echo Day 5 Test FAILED
echo ========================================

:end
