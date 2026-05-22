@echo off
echo Building linechk.exe...
echo.

REM Set Watcom environment
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt

REM Compile linechk.c
echo Compiling linechk.c...
wcc -mm -0 -w4 -zq -os -s -i=%WATCOM%\h -fo=build\obj\linechk.obj tools\linechk.c
if errorlevel 1 goto :error

REM Link linechk.exe
echo Linking linechk.exe...
wlink system dos name build\bin\linechk.exe file build\obj\linechk.obj
if errorlevel 1 goto :error

echo.
echo Build successful!
echo Output: build\bin\linechk.exe
goto :end

:error
echo.
echo Build failed!
exit /b 1

:end
