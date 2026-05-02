@echo off
REM Test script for method invocation
REM Run this in DOSBox or DOS-compatible environment

echo Testing Method Invocation...
echo.

echo Test 1: Simple method call (no return value)
..\..\build\bin\djc.exe test_method_call.java
if errorlevel 1 goto error1
..\..\build\bin\djvm.exe test_method_call.djc
if errorlevel 1 goto error1
echo Test 1: PASSED
echo.

echo Test 2: Method with return value
..\..\build\bin\djc.exe test_method_return.java
if errorlevel 1 goto error2
..\..\build\bin\djvm.exe test_method_return.djc
if errorlevel 1 goto error2
echo Test 2: PASSED
echo.

echo All tests completed successfully!
goto end

:error1
echo Test 1: FAILED
goto end

:error2
echo Test 2: FAILED
goto end

:end

@REM Made with Bob
