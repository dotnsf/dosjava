@echo off
echo Testing minimal.djc...
djvm minimal.djc
if errorlevel 1 goto error
echo.
echo Test passed!
goto end

:error
echo.
echo Test failed!

:end

@REM Made with Bob
