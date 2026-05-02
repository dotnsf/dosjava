@echo off
echo Copying files to DOSBox environment...

REM Copy compiler and VM executables
copy build\bin\djc.exe C:\dosbox\djc.exe > nul
copy build\bin\djvm.exe C:\dosbox\djvm.exe > nul

REM Copy test files
copy tests\array.jav C:\dosbox\array.jav > nul
copy tests\arrays.jav C:\dosbox\arrays.jav > nul
copy tests\test_array.bat C:\dosbox\test_array.bat > nul

echo Files copied successfully.
echo.
echo Starting DOSBox to run array tests...
echo.

"C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" -c "mount c c:\dosbox" -c "c:" -c "test_array.bat" -c "pause" -c "exit"

echo.
echo Test completed. Check output above.

@REM Made with Bob