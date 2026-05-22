@echo off
echo Copying files to DOSBox environment...

REM Copy compiler and VM executables
copy build\bin\djc.exe C:\dosbox\djc.exe > nul
copy build\bin\djvm.exe C:\dosbox\djvm.exe > nul

REM Copy test files
copy tests\ftst1.jav C:\dosbox\ftst1.jav > nul
copy tests\ftst2.jav C:\dosbox\ftst2.jav > nul
copy tests\ftest_simple.jav C:\dosbox\ftest_simple.jav > nul

echo Files copied successfully.
echo.
echo Starting DOSBox-X to run float tests...
echo.

"C:\Program Files\DOSBox-X\dosbox-x.exe" -c "mount c c:\dosbox" -c "c:" -c "dir djc.exe" -c "djc ftest_simple.jav" -c "pause" -c "djc ftst1.jav" -c "pause" -c "djvm ftst1.jav" -c "pause" -c "exit"

echo.
echo Test completed. Check output above.
