@echo off
echo Testing switch.jav compilation...
echo.

cd \DOSJAVA

echo Compiling switch.jav...
build\bin\djc.exe tests\switch.jav
if errorlevel 1 goto error

echo.
echo Compilation successful!
echo.
echo Running SWITCH.DJC...
src\vm\djvm.exe tests\SWITCH.DJC
echo.
echo Test complete.
goto end

:error
echo.
echo Compilation failed!

:end

@REM Made with Bob
