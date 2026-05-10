@echo off
set WATCOM=C:\WATCOM
set PATH=C:\WATCOM\binw;%PATH%
set INCLUDE=C:\WATCOM\h;C:\WATCOM\h\nt

cd /d %~dp0
wmake -f Makefile all

