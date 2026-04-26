@echo off
echo Testing hello.jav compilation and execution...

rem Compile hello.jav
djc.exe ..\..\hello.jav
if errorlevel 1 goto compile_error

rem Check if HELLO.DJC was created
if not exist HELLO.DJC goto no_output

rem Execute HELLO.DJC
djvm.exe HELLO.DJC > output.txt 2>&1
if errorlevel 1 goto runtime_error

rem Display output
type output.txt

rem Success
echo.
echo === TEST PASSED ===
goto end

:compile_error
echo ERROR: Compilation failed
goto end

:no_output
echo ERROR: HELLO.DJC not created
goto end

:runtime_error
echo ERROR: Runtime error
type output.txt
goto end

:end

@REM Made with Bob
