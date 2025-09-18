@echo off
REM NPM Vulnerability Checker - Windows Build Script
REM This script compiles the application for Windows

echo Building NPM Vulnerability Checker for Windows...

REM Change to project root directory
cd /d "%~dp0..\.."

REM Check if MinGW or Visual Studio is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    where cl >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No C compiler found.
        echo Please install one of the following:
        echo   - MinGW-w64: https://www.mingw-w64.org/
        echo   - Visual Studio Build Tools
        echo   - Code::Blocks with MinGW
        pause
        exit /b 1
    ) else (
        goto :build_msvc
    )
)

:build_gcc
echo Using GCC compiler...

REM Create build directory
if not exist "build\windows" mkdir "build\windows"

REM Source files
set SOURCES=src\main.c src\common.c src\csv_parser.c src\npm_analyzer.c src\vulnerability_checker.c src\report_generator.c src\gui_windows.c

REM Compile with GCC (MinGW)
echo Compiling...
gcc -o build\windows\npm-vuln-checker.exe %SOURCES% ^
    -Isrc ^
    -luser32 -lkernel32 -lgdi32 -lshell32 -lole32 -lcomdlg32 ^
    -Wall -Wextra -Wno-unused-parameter -O2 -std=c99 -mwindows

if %errorlevel% equ 0 (
    echo Build successful!
    echo Executable created: build\windows\npm-vuln-checker.exe
    echo.
    echo To run the application:
    echo   build\windows\npm-vuln-checker.exe
) else (
    echo Build failed!
    pause
    exit /b 1
)
goto :end

:build_msvc
echo Using Visual Studio compiler...

REM Create build directory
if not exist "build\windows" mkdir "build\windows"

REM Source files
set SOURCES=src\main.c src\common.c src\csv_parser.c src\npm_analyzer.c src\vulnerability_checker.c src\report_generator.c src\gui_windows.c

REM Compile with MSVC
echo Compiling...
cl /Fe:build\windows\npm-vuln-checker.exe %SOURCES% ^
   /Isrc ^
   user32.lib kernel32.lib gdi32.lib shell32.lib ole32.lib comdlg32.lib ^
   /O2 /MT /TC

if %errorlevel% equ 0 (
    echo Build successful!
    echo Executable created: build\windows\npm-vuln-checker.exe
    echo.
    echo To run the application:
    echo   build\windows\npm-vuln-checker.exe

    REM Clean up MSVC temporary files
    del *.obj >nul 2>&1
) else (
    echo Build failed!
    pause
    exit /b 1
)

:end
echo.
echo Build process completed.
pause