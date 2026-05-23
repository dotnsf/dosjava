@echo off
echo Testing String switch statement...
echo.

echo Compiling swstring.jav...
..\djc swstring.jav
if errorlevel 1 goto error

echo.
echo Running swstring...
..\djvm swstring
if errorlevel 1 goto error

echo.
echo Test completed successfully!
goto end

:error
echo.
echo Test failed!

:end

@REM Made with Bob
