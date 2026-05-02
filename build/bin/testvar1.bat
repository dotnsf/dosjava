@echo off
echo Testing var1.djc...
if exist stdout.txt del stdout.txt
if exist stderr.txt del stderr.txt

djvm var1.djc > stdout.txt
if errorlevel 1 goto runtime_error

find "10" stdout.txt > nul
if errorlevel 1 goto output_error

echo Output:
type stdout.txt
echo.
echo Test passed!
goto end

:runtime_error
echo.
echo Runtime error!
rem stderr redirection is not available in DOS batch
goto fail

:output_error
echo.
echo Output mismatch! Expected: 10
echo Actual output:
if exist stdout.txt type stdout.txt
rem stderr redirection is not available in DOS batch
goto fail

:fail
echo.
echo Test failed!

:end

@REM Made with Bob
