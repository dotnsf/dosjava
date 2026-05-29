@echo off
echo === String Extension Test Part 2 ===
echo.
echo Compiling strext2.jav...
djc.exe strext2.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running strext2.djc...
echo.
djvm.exe strext2.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end