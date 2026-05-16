@echo off
REM Check if obj3.djc is being updated
echo Checking obj3.djc file...
echo.
dir tests\obj3.djc
echo.
echo Deleting old obj3.djc...
del tests\obj3.djc
echo.
echo Recompiling obj3.jav...
djc.exe tests\obj3.jav
echo.
echo New obj3.djc:
dir tests\obj3.djc
echo.
echo Running djvm.exe...
djvm.exe tests\obj3.djc
