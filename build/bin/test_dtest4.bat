@echo off
echo Compiling dtest4.jav...
djc.exe ..\..\tests\dtest4.jav
if errorlevel 1 goto error

echo Running dtest4.djc...
djvm.exe dtest4.djc
if errorlevel 1 goto error

echo.
echo Test completed successfully!
goto end

:error
echo.
echo Test failed!

:end
