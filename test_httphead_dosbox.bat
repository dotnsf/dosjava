@echo off
echo Testing HTTP GET with custom headers in DOSBox-X...
echo.

REM Copy files to DOSBox-X directory
echo Copying files to DOSBox-X...
copy build\bin\djc.exe "C:\Users\dotns\DOSBox-X\djc.exe" > nul
copy build\bin\djvm.exe "C:\Users\dotns\DOSBox-X\djvm.exe" > nul
copy tests\httphead.jav "C:\Users\dotns\DOSBox-X\httphead.jav" > nul

echo Files copied successfully.
echo.
echo Please run the following commands in DOSBox-X:
echo.
echo   djc.exe httphead.jav
echo   djvm.exe httphead.djc
echo.
echo Note: Make sure doscurl.exe is available in DOSBox-X
echo.
pause
