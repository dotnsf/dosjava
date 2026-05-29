@echo off
echo === String.charAt() Test ===
echo.
echo Compiling charat.jav...
djc.exe charat.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running charat.djc...
echo.
djvm.exe charat.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end