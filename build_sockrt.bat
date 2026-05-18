@echo off
REM Build script for runtime Socket test program
REM Uses wmake with Makefile

echo Building runtime Socket test program...

REM Set Watcom environment
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt

REM Build using wmake
wmake test_sockrt

if errorlevel 1 goto error

echo.
echo Build successful!
echo Output: build\bin\tsockrt.exe
echo.
echo To run in DOSBox-X:
echo   1. Copy build\bin\tsockrt.exe to DOSBox-X
echo   2. Copy tests\network\MTCP.CFG to DOSBox-X
echo   3. SET MTCP.CFG=C:\MTCP\MTCP.CFG
echo   4. Run: tsockrt.exe
echo.
goto end

:error
echo.
echo Build failed!
echo.
exit /b 1

:end

@REM Made with Bob
