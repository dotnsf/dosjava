@echo off
REM Wattcp Initialization Test Runner
REM This batch file runs twatt.exe with wattcp.cfg in current directory

echo ========================================
echo Wattcp Initialization Test
echo ========================================
echo.

REM IMPORTANT: Unset WATTCP.CFG environment variable completely
REM Wattcp will then search current directory for wattcp.cfg
SET WATTCP.CFG=

REM Verify it's cleared
SET | FIND "WATTCP.CFG"
IF ERRORLEVEL 1 (
    echo Environment variable WATTCP.CFG cleared successfully
) ELSE (
    echo WARNING: WATTCP.CFG still set, trying to clear again...
    SET WATTCP.CFG=
)
echo.

REM Check if wattcp.cfg file exists in current directory
if not exist wattcp.cfg (
    echo WARNING: wattcp.cfg not found in current directory!
    echo.
    echo Please copy WATTCP.CFG to wattcp.cfg in the same directory as twatt.exe
    echo.
    echo Sample content:
    echo   my_ip = 192.168.1.100
    echo   netmask = 255.255.255.0
    echo   gateway = 192.168.1.1
    echo   nameserver = 8.8.8.8
    echo.
    pause
) else (
    echo Found wattcp.cfg in current directory
    echo.
)

REM Run the test
echo Running twatt.exe...
echo.
twatt.exe

echo.
echo ========================================
echo Test completed
echo ========================================

@REM Made with Bob
