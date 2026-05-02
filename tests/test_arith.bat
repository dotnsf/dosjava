@echo off
echo === Testing arith.jav compilation and execution ===
echo.

rem Clean up old files
if exist ARITH.DJC del ARITH.DJC

rem Compile
echo Compiling arith.jav...
..\build\bin\djc.exe arith.jav
if errorlevel 1 goto compile_error
if not exist ARITH.DJC goto compile_error
echo Compilation successful
echo.

rem Show hex dump of first 100 bytes
echo Hex dump of ARITH.DJC (first 100 bytes):
debug ARITH.DJC < nul | find ":" | more
echo.

rem Run
echo Running arith.djc...
..\build\bin\djvm.exe ARITH.DJC
if errorlevel 1 goto run_error
echo.
echo Test completed successfully
goto end

:compile_error
echo ERROR: Compilation failed
goto end

:run_error
echo ERROR: Execution failed
goto end

:end

@REM Made with Bob
