@echo off
REM NPM Vulnerability Checker - Distribution Builder for Windows
REM Author: Samrat Dutta

echo ============================================
echo NPM Vulnerability Checker - Distribution Builder
echo ============================================

REM Change to project root directory
cd /d "%~dp0..\.."

REM Clean previous distributions
echo Cleaning previous distribution files...
if exist dist rmdir /s /q dist
mkdir dist
mkdir dist\windows
mkdir dist\linux
mkdir dist\macos

REM Build for Windows
echo.
echo [1/3] Building Windows executable...
call scripts\windows\build.bat
if %ERRORLEVEL% neq 0 (
    echo ERROR: Windows build failed!
    exit /b 1
)

REM Package Windows distribution
echo Packaging Windows distribution...
copy build\windows\npm-vuln-checker.exe dist\windows\
copy README.md dist\windows\
copy scripts\windows\run.bat dist\windows\run_windows.bat 2>nul || echo Creating default run script...

REM Create run script for Windows if it doesn't exist
if not exist dist\windows\run_windows.bat (
    echo @echo off > dist\windows\run_windows.bat
    echo echo Starting NPM Vulnerability Checker... >> dist\windows\run_windows.bat
    echo npm-vuln-checker.exe >> dist\windows\run_windows.bat
    echo pause >> dist\windows\run_windows.bat
)

REM Create archive for Windows
echo Creating Windows archive...
cd dist\windows
tar -czf ..\npm-vuln-checker-windows.tar.gz *
cd ..\..

echo.
echo ============================================
echo Distribution build completed!
echo ============================================
echo.
echo Generated files:
echo   dist\windows\                   - Windows standalone package
echo   dist\npm-vuln-checker-windows.tar.gz - Windows archive for release
echo.
echo To distribute:
echo   1. Upload dist\npm-vuln-checker-windows.tar.gz to GitHub releases
echo   2. Or share the entire dist\windows\ folder
echo.
echo Usage on target machine:
echo   1. Extract archive
echo   2. Run: npm-vuln-checker.exe (or run_windows.bat)
echo.