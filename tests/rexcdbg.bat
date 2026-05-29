@echo off
REM Run excline.jav with debug mode in DOSBox-X
echo Compiling excline.jav...
djc.exe excline.jav
if errorlevel 1 goto error

echo.
echo Running with debug mode...
djvm.exe -d excline.djc
goto end

:error
echo Compilation failed!

:end
