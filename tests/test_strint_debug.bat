@echo off
echo Testing String + int with debug mode...
echo.

REM Compile strint2.jav
echo Compiling strint2.jav...
..\build\bin\djc.exe strint2.jav
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

REM Run with debug mode
echo.
echo Running with debug mode (-d):
echo ================================
..\build\bin\djvm.exe -d strint2.djc
echo ================================
echo.

echo Test complete.
