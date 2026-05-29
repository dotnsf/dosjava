@echo off
echo === String Extension Test Part 3 ===
echo.
echo Compiling strext3.jav...
djc.exe strext3.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running strext3.djc...
echo.
djvm.exe strext3.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end