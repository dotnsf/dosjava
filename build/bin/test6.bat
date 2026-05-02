@echo off
echo Testing hello_simple.jav compilation and execution...

rem Delete old files
if exist SIMPLE.DJC del SIMPLE.DJC

rem Compile hello_simple.jav
djc.exe ..\..\simple.jav > a
if errorlevel 1 goto compile_error

rem Check if SIMPLE.DJC was created
if not exist SIMPLE.DJC goto no_output

rem Execute SIMPLE.DJC
djvm.exe SIMPLE.DJC > b
if errorlevel 1 goto runtime_error

rem Success
echo.
echo === TEST PASSED ===
goto end

:compile_error
echo ERROR: Compilation failed
goto end

:no_output
echo ERROR: SIMPLE.DJC not created
goto end

:runtime_error
echo ERROR: Runtime error
goto end

:end

@REM Made with Bob
