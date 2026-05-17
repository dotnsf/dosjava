@echo off
REM Wattcp Initialization Test Runner for dosjava
REM This batch file runs tmtcp.exe in DOS environment

echo ========================================
echo Wattcp Initialization Test for dosjava
echo ========================================
echo.

REM Check if WATTCP.CFG exists
REM Note: DOS is case-insensitive, so WATTCP.CFG = wattcp.cfg
if not exist WATTCP.CFG goto no_config

REM Display WATTCP.CFG contents
echo Configuration file found: WATTCP.CFG
echo ------------------
type WATTCP.CFG
echo ------------------
echo.
goto check_exe

:no_config
echo WARNING: WATTCP.CFG not found in current directory
echo Please create WATTCP.CFG with network settings
echo.
echo Sample WATTCP.CFG content:
echo   my_ip = 192.168.0.123
echo   netmask = 255.255.255.0
echo   gateway = 192.168.0.1
echo   nameserver = 8.8.8.8
echo.
goto end

:check_exe
REM Check if tmtcp.exe exists
REM if not exist tmtcp.exe goto no_exe

REM Run the test
echo Running tmtcp.exe...
echo.
tmtcp.exe
goto end

:no_exe
echo ERROR: tmtcp.exe not found
echo Please build it first with: wmake test_mtcp
echo.

:end
echo.
echo Test completed.

@REM Made with Bob
