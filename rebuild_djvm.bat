@echo off
REM Rebuild djvm.exe with latest changes
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h

echo Cleaning old object files...
del build\obj\interpreter.obj 2>nul
del build\bin\djvm.exe 2>nul

echo.
echo Compiling interpreter.c...
wcc -ms -0 -w4 -zq -os -s -i=C:\WATCOM\h -fo=build\obj\interpreter.obj src\vm\interpreter.c
if errorlevel 1 goto error

echo.
echo Linking djvm.exe...
wlink system dos option stack=16384 name build\bin\djvm.exe file { build\obj\djvm.obj build\obj\memory.obj build\obj\stack.obj build\obj\interpreter.obj build\obj\djc.obj build\obj\opcodes.obj build\obj\object.obj build\obj\string.obj build\obj\system.obj build\obj\integer.obj }
if errorlevel 1 goto error

echo.
echo Build successful!
echo.
echo Now test with:
echo   djvm.exe tests\obj3.djc
goto end

:error
echo.
echo Build failed!
exit /b 1

:end

@REM Made with Bob
