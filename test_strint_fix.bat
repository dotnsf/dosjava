@echo off
echo Testing String + int fix...
echo.
echo Test 1: strint2.jav (System.out.println("Value: " + 42))
djc.exe tests\strint2.jav
if errorlevel 1 goto error
djvm.exe tests\strint2.djc
echo.
echo Test 2: strint1.jav (String + int variable)
djc.exe tests\strint1.jav
if errorlevel 1 goto error
djvm.exe tests\strint1.djc
echo.
echo Test 3: strint3.jav (Variable assignment)
djc.exe tests\strint3.jav
if errorlevel 1 goto error
djvm.exe tests\strint3.djc
echo.
echo All tests completed!
goto end
:error
echo Compilation failed!
:end
