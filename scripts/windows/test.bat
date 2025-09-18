@echo off
REM NPM Vulnerability Checker - Test Build and Run Script
REM This script compiles and runs all tests

echo.
echo ==========================================
echo NPM Vulnerability Checker - Test Runner
echo ==========================================
echo.

REM Change to project root directory
cd /d "%~dp0..\.."

REM Always rebuild tests to ensure they're current
echo Building tests...
goto :build_tests

:build_tests
echo.
echo Building and running NPM Vulnerability Checker tests...

REM Check if compiler is available
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    where cl >nul 2>&1
    if %errorlevel% neq 0 (
        echo Error: No C compiler found.
        echo Please install MinGW-w64 or Visual Studio Build Tools
        pause
        exit /b 1
    ) else (
        goto :build_msvc
    )
)

:build_gcc
echo Using GCC compiler...

REM Create test build directory
if not exist "build\tests" mkdir "build\tests"

REM Source files (excluding GUI modules for tests)
set MAIN_SOURCES=src\common.c src\csv_parser.c src\npm_analyzer.c src\vulnerability_checker.c src\report_generator.c src\cli.c

REM Test source files
set TEST_SOURCES=tests\test_framework.c tests\mock_data\npm_mock.c
set UNIT_TEST_SOURCES=tests\unit\test_csv_parser.c tests\unit\test_vulnerability_checker.c tests\unit\test_report_generator.c tests\unit\test_cli.c
set INTEGRATION_TEST_SOURCES=tests\integration\test_npm_analyzer.c tests\integration\test_full_workflow.c tests\integration\test_cli_integration.c tests\integration\test_gui_integration.c
set E2E_TEST_SOURCES=tests\e2e\test_end_to_end.c tests\e2e\test_cli_e2e.c
set MAIN_TEST_SOURCE=tests\run_all_tests.c

REM Compile tests
echo Compiling tests...
gcc -o build\tests\run_all_tests.exe ^
    %MAIN_SOURCES% %TEST_SOURCES% %UNIT_TEST_SOURCES% %INTEGRATION_TEST_SOURCES% %E2E_TEST_SOURCES% %MAIN_TEST_SOURCE% ^
    -Isrc -Itests ^
    -Wall -Wextra -Wno-unused-parameter -std=c99 -g -DTEST_BUILD

if %errorlevel% equ 0 (
    goto :setup_test_env
) else (
    echo Test compilation failed!
    pause
    exit /b 1
)

:build_msvc
echo Using Visual Studio compiler...

REM Create test build directory
if not exist "build\tests" mkdir "build\tests"

REM Source files
set MAIN_SOURCES=src\common.c src\csv_parser.c src\npm_analyzer.c src\vulnerability_checker.c src\report_generator.c src\cli.c
set TEST_SOURCES=tests\test_framework.c tests\mock_data\npm_mock.c
set UNIT_TEST_SOURCES=tests\unit\test_csv_parser.c tests\unit\test_vulnerability_checker.c tests\unit\test_report_generator.c
set INTEGRATION_TEST_SOURCES=tests\integration\test_npm_analyzer.c tests\integration\test_full_workflow.c
set E2E_TEST_SOURCES=tests\e2e\test_end_to_end.c
set MAIN_TEST_SOURCE=tests\run_all_tests.c

REM Compile tests
echo Compiling tests...
cl /Fe:build\tests\run_all_tests.exe ^
   %MAIN_SOURCES% %TEST_SOURCES% %UNIT_TEST_SOURCES% %INTEGRATION_TEST_SOURCES% %E2E_TEST_SOURCES% %MAIN_TEST_SOURCE% ^
   /Isrc /Itests ^
   /O2 /MT /TC /DTEST_BUILD

if %errorlevel% equ 0 (
    REM Clean up MSVC temporary files
    del *.obj >nul 2>&1
    goto :setup_test_env
) else (
    echo Test compilation failed!
    pause
    exit /b 1
)

:setup_test_env
echo Test compilation successful!
echo.

REM Create test directories if they don't exist
if not exist "tests\fixtures\csv" mkdir "tests\fixtures\csv"
if not exist "tests\fixtures\npm" mkdir "tests\fixtures\npm"
if not exist "tests\fixtures\expected_reports" mkdir "tests\fixtures\expected_reports"
if not exist "build\test_output" mkdir "build\test_output"

echo Test fixtures directory structure ready.

:run_tests
echo.
echo ==========================================
echo Starting Test Execution
echo ==========================================

REM Run the tests
build\tests\run_all_tests.exe %*

set TEST_RESULT=%ERRORLEVEL%

echo.
echo ==========================================
if %TEST_RESULT% EQU 0 (
    echo PASS: All tests passed!
) else (
    echo FAIL: Some tests failed!
)
echo ==========================================
echo.

pause
exit /b %TEST_RESULT%