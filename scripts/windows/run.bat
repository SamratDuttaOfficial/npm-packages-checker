@echo off
REM NPM Vulnerability Checker - Windows Runtime Script
REM This script runs the pre-compiled application on Windows

echo Starting NPM Vulnerability Checker...

REM Change to project root directory
cd /d "%~dp0..\.."

REM Check if Visual Studio Build Tools or MinGW is available
where cl >nul 2>&1
if %errorlevel% equ 0 (
    set COMPILER_FOUND=1
) else (
    where gcc >nul 2>&1
    if %errorlevel% equ 0 (
        set COMPILER_FOUND=1
    ) else (
        echo Error: No C compiler found.
        echo Please install one of the following:
        echo   - Visual Studio Build Tools
        echo   - MinGW-w64
        echo   - Visual Studio Community
        pause
        exit /b 1
    )
)

REM Check if pkg-config is available (for MinGW builds)
where gcc >nul 2>&1
if %errorlevel% equ 0 (
    where pkg-config >nul 2>&1
    if %errorlevel% neq 0 (
        echo Warning: pkg-config not found.
        echo For MinGW builds, install MSYS2 and pkg-config
    )
)

REM Check if the executable exists
if not exist "build\windows\npm-vuln-checker.exe" (
    echo Error: npm-vuln-checker.exe not found in build\windows\ directory.
    echo Please build the project first using: scripts\windows\build.bat
    pause
    exit /b 1
)

REM Check if npm is available
where npm >nul 2>&1
if %errorlevel% neq 0 (
    echo Warning: npm is not installed or not in PATH.
    echo The application requires npm to analyze projects.
    echo Please install Node.js and npm from: https://nodejs.org/
    echo.
)

REM Run the application
echo Launching application...
build\windows\npm-vuln-checker.exe

if %errorlevel% neq 0 (
    echo Application exited with an error.
    echo Please check the above messages for details.
    pause
)