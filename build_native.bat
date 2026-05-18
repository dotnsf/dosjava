@echo off
REM Build script for native method mechanism

echo Setting up Open Watcom environment...
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINW;%PATH%
set INCLUDE=%WATCOM%\H

echo.
echo Building djvm with native method support...
wmake djvm

if errorlevel 1 (
    echo.
    echo Build failed!
    exit /b 1
)

echo.
echo Build successful!
echo.
echo djvm.exe has been built with native method mechanism support.
echo.

