@echo off
echo === String Edge Cases Test ===
echo.
echo Compiling stredge.jav...
djc.exe stredge.jav
if errorlevel 1 goto error
echo.
echo Compilation successful!
echo.
echo Running stredge.djc...
echo.
djvm.exe stredge.djc
echo.
echo Test completed.
goto end

:error
echo.
echo Compilation failed!

:end