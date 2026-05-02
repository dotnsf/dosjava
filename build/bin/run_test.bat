@echo off
echo === DOSBox Test Script ===
echo.
cd /d %~dp0
"C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" -c "mount c ." -c "c:" -c "test_dosbox.bat" -c "pause" -c "exit"

@REM Made with Bob
