@echo off
REM Build test_memory_avail.exe

set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt

echo Building test_memory_avail.exe...
wcc -mm -0 -w4 -zq -os -s -i=%WATCOM%\h -fo=build\obj\test_memory_avail.obj tests\memory\test_memory_avail.c
if errorlevel 1 goto error

wlink system dos name build\bin\tmemavl.exe file { build/obj/test_memory_avail.obj }
if errorlevel 1 goto error

echo Build successful!
goto end

:error
echo Build failed!
exit /b 1

:end
