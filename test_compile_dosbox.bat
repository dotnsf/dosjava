@echo off
cd c:\Users\KEIKIMURA\src\dosjava\build\bin
"C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" -c "mount c ." -c "c:" -c "cd build\bin" -c "test_compile.bat" -c "pause" -c "exit"
cd c:\Users\KEIKIMURA\src\dosjava

@REM Made with Bob
