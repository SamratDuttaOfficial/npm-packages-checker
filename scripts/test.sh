#!/bin/bash

# NPM Vulnerability Checker - Test Build and Run Script
# This script compiles and runs all tests

echo "NPM Vulnerability Checker - Test Runner"
echo "========================================"

# Change to project root directory
cd "$(dirname "$0")/.."

echo "Building tests..."
echo "Building and running NPM Vulnerability Checker tests..."

# Create test build directory
mkdir -p build/tests

# Source files (excluding GUI modules for tests)
MAIN_SOURCES="src/common.c src/csv_parser.c src/npm_analyzer.c src/vulnerability_checker.c src/report_generator.c"

# Test source files
TEST_SOURCES="tests/test_framework.c tests/mock_data/npm_mock.c"
UNIT_TEST_SOURCES="tests/unit/test_csv_parser.c tests/unit/test_vulnerability_checker.c tests/unit/test_report_generator.c"
INTEGRATION_TEST_SOURCES="tests/integration/test_npm_analyzer.c tests/integration/test_full_workflow.c"
E2E_TEST_SOURCES="tests/e2e/test_end_to_end.c"
MAIN_TEST_SOURCE="tests/run_all_tests.c"

ALL_SOURCES="$MAIN_SOURCES $TEST_SOURCES $UNIT_TEST_SOURCES $INTEGRATION_TEST_SOURCES $E2E_TEST_SOURCES $MAIN_TEST_SOURCE"

# Compile tests
echo "Compiling tests..."
gcc -o build/tests/run_all_tests $ALL_SOURCES \
    -Isrc -Itests \
    -Wall -Wextra -Wno-unused-parameter -std=c99 -g -DTEST_BUILD

if [ $? -eq 0 ]; then
    echo "Test compilation successful!"
    echo ""

    # Create test directories if they don't exist
    mkdir -p tests/fixtures/csv
    mkdir -p tests/fixtures/npm
    mkdir -p tests/fixtures/expected_reports
    mkdir -p build/test_output

    echo "Test fixtures directory structure ready."
else
    echo "ERROR: Test compilation failed!"
    exit 1
fi

# Run tests
echo ""
echo "Starting Test Execution"
echo "=============================================="
./build/tests/run_all_tests "$@"
TEST_EXIT_CODE=$?

echo ""
echo "=============================================="
if [ $TEST_EXIT_CODE -eq 0 ]; then
    echo "PASS: All tests passed!"
else
    echo "FAIL: Some tests failed!"
fi

exit $TEST_EXIT_CODE