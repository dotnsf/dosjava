@echo off
REM Build mTCP Initialization Test
REM This script builds the mTCP test program using Open Watcom

echo ========================================
echo Building mTCP Initialization Test
echo ========================================
echo.

REM Set Open Watcom environment
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINW;%PATH%
set INCLUDE=%WATCOM%\H

REM Set mTCP paths
set MTCP_INCLUDE=C:\mTCP\src\TCPINC
set MTCP_LIB_DIR=C:\mTCP\lib

echo Building test_mtcp target (using mTCP library)...
wmake test_mtcp

if errorlevel 1 (
    echo.
    echo ========================================
    echo BUILD FAILED
    echo ========================================
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL
echo ========================================
echo.
echo Output: build\bin\tmtcp.exe
echo.
echo Next steps:
echo   1. Copy tmtcp.exe to DOSBox-X
echo   2. Set MTCP_CFG environment variable in DOSBox-X
echo      Example: SET MTCP_CFG=D:\MTCP.CFG
echo   3. Copy your MTCP.CFG file to DOSBox-X
echo   4. In DOSBox-X, load packet driver: ne2000 0x60 3 0x300
echo   5. Run: tmtcp.exe
echo.
echo Note: Using actual mTCP library (C++)
echo       Compiled with wpp (C++ compiler)

@REM Made with Bob
