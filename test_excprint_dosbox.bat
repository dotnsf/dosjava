@echo off
REM Test Exception variable output in DOSBox-X
echo Testing Exception variable output...
echo.

REM Copy files to DOSBox directory
echo Copying files to DOSBox...
copy build\bin\djc.exe C:\dosbox\djc.exe
copy build\bin\djvm.exe C:\dosbox\djvm.exe
copy tests\excprint.jav C:\dosbox\excprint.jav
copy tests\excsimp.jav C:\dosbox\excsimp.jav

echo.
echo Files copied. Now run in DOSBox-X:
echo   cd \
echo   djc excprint.jav
echo   djvm excprint.djc
echo.
echo Or test simple version:
echo   djc excsimp.jav
echo   djvm excsimp.djc
echo.
pause
