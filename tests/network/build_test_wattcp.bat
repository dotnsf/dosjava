@echo off
REM Build script for Wattcp initialization test

echo Building Wattcp initialization test...
echo.

REM Set Open Watcom environment
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINW64;%WATCOM%\BINW;%PATH%
set INCLUDE=%WATCOM%\H;%WATCOM%\H\WATTCP

echo Compiling test_wattcp_init.c...
wcc -ms -0 -w4 -zq -os -s -i=%WATCOM%\H -i=%WATCOM%\H\WATTCP test_wattcp_init.c
if errorlevel 1 goto error

echo Linking twatt.exe...
wlink system dos name twatt.exe file test_wattcp_init.obj library %WATCOM%\LIB286\WATTCPWS.LIB
if errorlevel 1 goto error

echo.
echo Build successful!
echo Output: twatt.exe
echo.
echo To test in DOSBox-X:
echo   1. Load packet driver (e.g., ne2000 0x60 3 0x300)
echo   2. Run: twatt.exe
echo.
goto end

:error
echo.
echo Build failed!
echo.

:end

@REM Made with Bob
