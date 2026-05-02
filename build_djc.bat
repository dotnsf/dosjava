@echo off
set WATCOM=C:\Watcom
set PATH=C:\Watcom\BINNT;%PATH%
set INCLUDE=C:\Watcom\H
cd /d c:\Users\KEIKIMURA\src\dosjava
wmake djc
if exist build\bin\djc.exe (
    echo djc.exe built successfully
) else (
    echo djc.exe not found in build\bin
)

@REM Made with Bob
