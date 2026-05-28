@echo off
echo Compiling excstr.jav...
djc.exe excstr.jav
if errorlevel 1 goto error

echo.
echo Running excstr.djc...
djvm.exe excstr.djc
if errorlevel 1 goto error

echo.
echo Test completed successfully
goto end

:error
echo.
echo Test failed with error
exit /b 1

:end
