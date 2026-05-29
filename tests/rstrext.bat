@echo off
echo === String Extension Methods Test ===
echo.
echo Compiling strext.jav...
djc.exe strext.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running strext.djc...
echo.
djvm.exe strext.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end