@echo off
echo Testing djc.exe compilation... > result.txt
djc.exe hello.jav >> result.txt 2>&1
if errorlevel 1 (
    echo Compilation failed! >> result.txt
    type result.txt
    exit /b 1
)
echo Compilation successful! >> result.txt
echo. >> result.txt
echo Testing djvm.exe execution... >> result.txt
djvm.exe HELLO.DJC >> result.txt 2>&1
if errorlevel 1 (
    echo Execution failed! >> result.txt
    type result.txt
    exit /b 1
)
echo Execution successful! >> result.txt
type result.txt

@REM Made with Bob
