@echo off
REM Build script for obj3 test
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h

echo Compiling codegen.c...
wcc -ms -0 -w4 -zq -os -s -i=C:\WATCOM\h -fo=build\obj\codegen.obj tools\compiler\codegen.c
if errorlevel 1 goto error

echo Linking djc.exe...
wlink system dos option stack=16384 name build\bin\djc.exe file { build\obj\djc_main.obj build\obj\lexer.obj build\obj\parser.obj build\obj\symtable.obj build\obj\semantic.obj build\obj\codegen.obj build\obj\djc.obj build\obj\opcodes.obj build\obj\memory.obj }
if errorlevel 1 goto error

echo Build successful!
goto end

:error
echo Build failed!
exit /b 1

:end
