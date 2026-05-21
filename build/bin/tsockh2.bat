@echo off
REM Test sockhlp2.exe - doscurl-based version (8.3 filename)

echo Testing sockhlp2.exe...
echo.

REM Clean up previous test files
if exist SOCK.IN del SOCK.IN
if exist SOCK.OUT del SOCK.OUT
if exist SOCKH2.LOG del SOCKH2.LOG

echo Test 1: Connect to 192.168.0.2:8080
echo.
echo Running: sockhlp2.exe connect 192.168.0.2 8080 ^> SOCKH2.LOG
sockhlp2.exe connect 192.168.0.2 8080 > SOCKH2.LOG

echo.
echo === Output Log (SOCKH2.LOG) ===
type SOCKH2.LOG
echo.

echo.
echo === Result (SOCK.OUT) ===
type SOCK.OUT
echo.

echo.
echo Test complete!
echo.
echo Compare with doscurl.exe:
echo   doscurl.exe http://192.168.0.2:8080/ ^> DOSCURL.LOG
echo.

@REM Made with Bob