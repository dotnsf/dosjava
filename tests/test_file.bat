@echo off
echo Compiling file1.jav...
..\build\bin\djc.exe file1.jav file1.djc
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

echo Running file1.djc...
..\build\bin\djvm.exe file1.djc
if errorlevel 1 (
    echo Execution failed!
    exit /b 1
)

echo Test completed successfully!

