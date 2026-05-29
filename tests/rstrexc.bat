@echo off
echo === String Exception Test ===
echo.
echo Compiling strexc.jav...
djc.exe strexc.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running strexc.djc...
echo.
djvm.exe strexc.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end