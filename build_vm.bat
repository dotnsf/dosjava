@echo off
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h

cd /d %~dp0
wmake all

@REM Made with Bob
