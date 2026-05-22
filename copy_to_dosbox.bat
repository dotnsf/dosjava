@echo off
echo Copying latest executables and test files to C:\dosbox...

REM Create directory if it doesn't exist
if not exist C:\dosbox mkdir C:\dosbox

REM Copy compiler and VM executables
copy build\bin\djc.exe C:\dosbox\djc.exe
copy build\bin\djvm.exe C:\dosbox\djvm.exe

REM Copy test files
copy tests\ftst1.jav C:\dosbox\ftst1.jav
copy tests\ftst2.jav C:\dosbox\ftst2.jav
copy tests\ftest_simple.jav C:\dosbox\ftest_simple.jav

echo.
echo Files copied successfully to C:\dosbox
echo.
echo In DOSBox-X, run:
echo   mount c c:\dosbox
echo   c:
echo   djc ftest_simple.jav
echo   djc ftst1.jav
echo   djvm ftst1.jav
echo.
pause
