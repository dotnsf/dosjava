@echo off
echo Testing calc.jav
echo.
echo Compiling...
..\build\bin\djc.exe calc.jav
if errorlevel 1 goto error
echo.
echo Running...
..\build\bin\djvm.exe calc.djc
if errorlevel 1 goto error
echo.
echo Test completed successfully!
goto end

:error
echo.
echo Test failed!

:end

@REM Made with Bob
