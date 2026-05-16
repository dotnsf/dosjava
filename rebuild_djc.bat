@echo off
REM Rebuild djc.exe with latest changes
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h

echo Cleaning old object files...
del build\obj\codegen.obj 2>nul
del build\obj\djc_main.obj 2>nul
del build\bin\djc.exe 2>nul

echo.
echo Compiling codegen.c...
wcc -ms -0 -w4 -zq -os -s -i=C:\WATCOM\h -fo=build\obj\codegen.obj tools\compiler\codegen.c
if errorlevel 1 goto error

echo.
echo Compiling djc.c...
wcc -ms -0 -w4 -zq -os -s -i=C:\WATCOM\h -fo=build\obj\djc_main.obj tools\compiler\djc.c
if errorlevel 1 goto error

echo.
echo Linking djc.exe...
wlink system dos option stack=16384 name build\bin\djc.exe file { build\obj\djc_main.obj build\obj\lexer.obj build\obj\parser.obj build\obj\symtable.obj build\obj\semantic.obj build\obj\codegen.obj build\obj\djc.obj build\obj\opcodes.obj build\obj\memory.obj }
if errorlevel 1 goto error

echo.
echo Build successful!
echo.
echo Now test with:
echo   djc.exe tests\obj3.jav
echo   djvm.exe tests\obj3.djc
goto end

:error
echo.
echo Build failed!
exit /b 1

:end
