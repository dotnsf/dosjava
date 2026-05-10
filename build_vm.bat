@echo off
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h
cd src\vm
wcc -zq -ms -bt=dos -fo=djvm.obj djvm.c
wcc -zq -ms -bt=dos -fo=interpreter.obj interpreter.c
wcc -zq -ms -bt=dos -fo=memory.obj memory.c
wcc -zq -ms -bt=dos -fo=classfile.obj ..\..\tools\classfile.c
wlink system dos option quiet name ..\..\build\bin\djvm.exe file djvm.obj,interpreter.obj,memory.obj,classfile.obj
cd ..\..
echo Build complete

@REM Made with Bob
