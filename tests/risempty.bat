@echo off
echo === String.isEmpty() Test ===
echo.
echo Compiling isempty.jav...
djc.exe isempty.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running isempty.djc...
echo.
djvm.exe isempty.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end