@echo off
REM Check build dates of executables
echo Checking executable versions...
echo.
echo djc.exe:
dir build\bin\djc.exe
echo.
echo djvm.exe:
dir build\bin\djvm.exe
echo.
echo Current directory executables:
dir djc.exe 2>nul
dir djvm.exe 2>nul
echo.
echo PATH:
echo %PATH%
