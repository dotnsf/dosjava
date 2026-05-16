@echo off
cd /d %~dp0
call C:\WATCOM\owsetenv.bat
wmake clean
wmake all
