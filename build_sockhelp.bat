@echo off
REM Build sockhelp.exe - External Socket Helper

set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt

REM mTCP settings
set MTCP_TCP_H_DIR=C:\mTCP\src\TCPINC
set MTCP_TCP_C_DIR=C:\mTCP\src\TCPLIB
set MTCP_COMMON_H_DIR=C:\mTCP\src\INCLUDE
set MTCP_CFG_DIR=tests\network

echo Building sockhelp.exe...

REM Compile sockhelp.c with Large model using doscurl.cfg
REM Large model is required for sufficient memory (Medium model has only 7KB available)
wpp -0 -ml -DCFG_H="doscurl.cfg" -oh -ok -ot -s -oa -ei -zp2 -zpw -ob -ol+ -oi+ -i=%MTCP_TCP_H_DIR% -i=%MTCP_COMMON_H_DIR% -i=..\doscurl\cpp -fo=build\obj\sockhelp.obj tools/sockhelp.c
if errorlevel 1 goto error

REM Link sockhelp.exe with Large model mTCP objects
wlink system dos option map option eliminate option stack=8192 name build\bin\sockhelp.exe file { build/obj/sockhelp.obj build/obj/packet_l.obj build/obj/arp_l.obj build/obj/eth_l.obj build/obj/ip_l.obj build/obj/tcp_l.obj build/obj/tcpsockm_l.obj build/obj/udp_l.obj build/obj/utils_l.obj build/obj/dns_l.obj build/obj/timer_l.obj build/obj/ipasm.obj build/obj/trace_l.obj }
if errorlevel 1 goto error

echo Build successful!
echo.
echo sockhelp.exe created in build\bin\
echo.
echo Test with:
echo   sockhelp.exe connect 192.168.1.1 3000
echo   type SOCK.OUT
goto end

:error
echo Build failed!
exit /b 1

:end

@REM Made with Bob
