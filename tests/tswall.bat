@echo off
echo ========================================
echo Testing All Switch Statement Features
echo ========================================
echo.

echo [1/7] Testing swsimple.jav (basic switch)...
djc.exe swsimple.jav
if errorlevel 1 goto error
djvm.exe swsimple.djc
echo.

echo [2/7] Testing swint.jav (int switch with multiple cases)...
djc.exe swint.jav
if errorlevel 1 goto error
djvm.exe swint.djc
echo.

echo [3/7] Testing swlong.jav (long switch)...
djc.exe swlong.jav
if errorlevel 1 goto error
djvm.exe swlong.djc
echo.

echo [4/7] Testing swstring.jav (string switch)...
djc.exe swstring.jav
if errorlevel 1 goto error
djvm.exe swstring.djc
echo.

echo [5/7] Testing swdef.jav (default case handling)...
djc.exe swdef.jav
if errorlevel 1 goto error
djvm.exe swdef.djc
echo.

echo [6/7] Testing swnest.jav (nested switch)...
djc.exe swnest.jav
if errorlevel 1 goto error
djvm.exe swnest.djc
echo.

echo [7/7] Testing swmany.jav (performance with 15 cases)...
djc.exe swmany.jav
if errorlevel 1 goto error
djvm.exe swmany.djc
echo.

echo ========================================
echo Testing swfall.jav (fall-through behavior)...
echo Note: This tests the new fall-through feature
echo ========================================
djc.exe swfall.jav
if errorlevel 1 goto error
djvm.exe swfall.djc
echo.

echo ========================================
echo All switch tests completed!
echo ========================================
goto end

:error
echo.
echo ========================================
echo ERROR: Test failed!
echo ========================================

REM exit /b 1

:end

