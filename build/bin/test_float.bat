@echo off
echo Testing Float Support
echo.

echo Compiling ftst1.jav...
djc ..\..\tests\ftst1.jav
if errorlevel 1 goto error

echo Compiling ftst2.jav...
djc ..\..\tests\ftst2.jav
if errorlevel 1 goto error

echo.
echo Running ftst1.jav...
djvm ftst1.jav
if errorlevel 1 goto error

echo.
echo Running ftst2.jav...
djvm ftst2.jav
if errorlevel 1 goto error

echo.
echo All float tests passed!
goto end

:error
echo.
echo Test failed!

:end
