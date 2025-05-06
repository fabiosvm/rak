@echo off

if not exist "tests\" (
    echo Error: 'tests' directory not found. >&2
    exit /b 1
)

where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Python is not installed or not in PATH. >&2
    exit /b 1
)

python tests\run.py -e "build\Release\rak.exe" %*
