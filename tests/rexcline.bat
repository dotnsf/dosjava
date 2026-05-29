@echo off
echo === Exception Line Number Test ===
echo.
echo Compiling excline.jav...
djc.exe excline.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running excline.djc...
echo.
djvm.exe excline.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end
