@echo off
echo Testing E2E compilation...
echo.

echo Compiling e2ewrit.jav...
djc.exe e2ewrit.jav
if errorlevel 1 (
    echo FAILED: e2ewrit.jav compilation error
    goto end
)
echo OK: e2ewrit.jav compiled successfully
echo.

echo Compiling e2eread.jav...
djc.exe e2eread.jav
if errorlevel 1 (
    echo FAILED: e2eread.jav compilation error
    goto end
)
echo OK: e2eread.jav compiled successfully
echo.

echo Compiling e2ecopy.jav...
djc.exe e2ecopy.jav
if errorlevel 1 (
    echo FAILED: e2ecopy.jav compilation error
    goto end
)
echo OK: e2ecopy.jav compiled successfully
echo.

echo All E2E tests compiled successfully!

:end
