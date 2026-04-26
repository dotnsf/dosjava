@echo off
echo Compiling hello.jav...
djc.exe ..\..\hello.jav > compile.log 2>&1
type compile.log
echo.
echo Compilation complete. Check HELLO.DJC

@REM Made with Bob
