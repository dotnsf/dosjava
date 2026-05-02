@echo off
echo Starting DOSBox test...
dosbox -c "mount c ." -c "c:" -c "test_dosbox.bat" -c "exit"

@REM Made with Bob
