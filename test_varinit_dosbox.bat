@echo off
REM Test variable initialization on DOSBox-X
echo Testing variable initialization...
echo.

REM Build compiler and VM
call build_djc.bat
if errorlevel 1 goto error

call build_vm.bat
if errorlevel 1 goto error

REM Copy to DOSBox directory
call copy_to_dosbox.bat
if errorlevel 1 goto error

echo.
echo Files copied to DOSBox directory
echo.
echo Please run the following commands in DOSBox-X:
echo.
echo   cd C:\DOSJAVA
echo   djc.exe tests\varinit.jav
echo   djvm.exe tests\varinit.djc
echo.
echo Expected output:
echo   42
echo   123456
echo   3.14
echo   true
echo   Hello
echo   30
echo   42
echo   72
echo   All tests passed!
echo.
pause
goto end

:error
echo Build failed!
pause

:end
