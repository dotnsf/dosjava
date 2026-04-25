@echo off
echo === Starting Test ===
echo.
echo [1] Compiling hello.jav with djc.exe...
djc.exe hello.jav
echo ERRORLEVEL: %ERRORLEVEL%
echo.
echo [2] Checking if HELLO.DJC was created...
if exist HELLO.DJC (
    echo SUCCESS: HELLO.DJC created
) else (
    echo FAILED: HELLO.DJC not found
    goto end
)
echo.
echo [3] Running HELLO.DJC with djvm.exe...
djvm.exe HELLO.DJC
echo ERRORLEVEL: %ERRORLEVEL%
echo.
:end
echo === Test Complete ===

@REM Made with Bob
