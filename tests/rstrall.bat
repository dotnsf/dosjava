@echo off
echo ========================================
echo  Phase 13: String Extension Tests
echo ========================================
echo.

echo [1/5] Running basic tests part 1...
echo.
call rstrext1.bat
echo.

echo [2/5] Running basic tests part 2...
echo.
call rstrext2.bat
echo.

echo [3/5] Running basic tests part 3...
echo.
call rstrext3.bat
echo.

echo [4/5] Running exception tests...
echo.
call rstrexc.bat
echo.

echo [5/5] Running edge case tests...
echo.
call rstredge.bat
echo.

echo ========================================
echo  All tests completed!
echo ========================================