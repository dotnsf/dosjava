@echo off
echo === String Extension Test Part 1 ===
echo.
echo Compiling strext1.jav...
djc.exe strext1.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running strext1.djc...
echo.
djvm.exe strext1.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end