@echo off
REM Build sockhlp2.exe - Simplified version based on doscurl (8.3 filename)

set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt

REM mTCP settings
set MTCP_TCP_H_DIR=C:\mTCP\src\TCPINC
set MTCP_TCP_C_DIR=C:\mTCP\src\TCPLIB
set MTCP_COMMON_H_DIR=C:\mTCP\src\INCLUDE

echo Building sockhlp2.exe...

REM Compile sockhelp_v2.c with Large model using doscurl.cfg
wpp -0 -ml -DCFG_H="doscurl.cfg" -oh -ok -ot -s -oa -ei -zp2 -zpw -ob -ol+ -oi+ -i=%MTCP_TCP_H_DIR% -i=%MTCP_COMMON_H_DIR% -i=..\doscurl\cpp -fo=build\obj\sockhlp2.obj tools/sockhelp_v2.c
if errorlevel 1 goto error

REM Link sockhlp2.exe with Large model mTCP objects (same as sockhelp.exe)
wlink system dos option map option eliminate option stack=8192 name build\bin\sockhlp2.exe file { build/obj/sockhlp2.obj build/obj/packet_l.obj build/obj/arp_l.obj build/obj/eth_l.obj build/obj/ip_l.obj build/obj/tcp_l.obj build/obj/tcpsockm_l.obj build/obj/udp_l.obj build/obj/utils_l.obj build/obj/dns_l.obj build/obj/timer_l.obj build/obj/ipasm.obj build/obj/trace_l.obj }
if errorlevel 1 goto error

echo Build successful!
echo.
echo sockhlp2.exe created in build\bin\
echo.
echo Test with:
echo   sockhlp2.exe connect 192.168.0.2 8080
echo   type SOCK.OUT
goto end

:error
echo Build failed!
exit /b 1

:end

@REM Made with Bob