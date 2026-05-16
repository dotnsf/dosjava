@echo off
REM E2E Integration Tests for Phase 3 I/O System
echo ========================================
echo Phase 3 E2E Integration Tests
echo ========================================
echo.

REM Test 1: BufferedWriter with newLine()
echo Test 1: BufferedWriter with newLine()
echo Compiling e2ewrit.jav...
djc.exe e2ewrit.jav
if errorlevel 1 goto error
echo Running e2ewrit.djc...
djvm.exe e2ewrit.djc
if errorlevel 1 goto error
echo.

REM Test 2: BufferedReader with readLine()
echo Test 2: BufferedReader with readLine()
echo Compiling e2eread.jav...
djc.exe e2eread.jav
if errorlevel 1 goto error
echo Running e2eread.djc...
djvm.exe e2eread.djc
if errorlevel 1 goto error
echo.

REM Test 3: File copy with FileInputStream/FileOutputStream
echo Test 3: File copy with FileInputStream/FileOutputStream
echo Compiling e2ecopy.jav...
djc.exe e2ecopy.jav
if errorlevel 1 goto error
echo Running e2ecopy.djc...
djvm.exe e2ecopy.djc
if errorlevel 1 goto error
echo.

REM Verify copied file
echo Verifying copied file...
fc /b E2EWRIT.TXT E2ECOPY.TXT
if errorlevel 1 goto error
echo Files match!
echo.

echo ========================================
echo All E2E tests PASSED!
echo ========================================
goto end

:error
echo.
echo ========================================
echo E2E tests FAILED!
echo ========================================
exit /b 1

:end
