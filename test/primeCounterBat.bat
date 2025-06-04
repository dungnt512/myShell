@echo off
echo Compiling primeCounter.c...
gcc test/primeCounter.c -o primeCounter.exe
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo Running primeCounter.exe...
primeCounter.exe
pause
