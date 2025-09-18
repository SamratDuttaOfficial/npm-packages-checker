# Integration Tests Organization

This directory contains integration tests that test the complete workflow of the NPM Vulnerability Checker.

## Directory Structure

```
tests/integration/
├── README.md                           # This file
├── test_npm_analyzer.c                 # Tests for NPM output parsing
├── test_full_workflow.c                # End-to-end workflow tests
├── test_data_paths.h                   # Centralized test data path constants
├── integration_test_setup.h            # Test environment setup utilities
└── data/                               # Organized test data
    ├── csv/                            # CSV vulnerability database files
    │   ├── test_format.csv
    │   ├── test_large.csv
    │   ├── test_partial.csv
    │   └── test_with_vuln.csv
    ├── npm_output/                     # NPM command output JSON files
    │   ├── e2e_error_npm.json
    │   ├── e2e_npm_output.json
    │   ├── test_format_npm.json
    │   ├── test_large_npm.json
    │   ├── test_partial_npm.json
    │   └── test_with_vuln_npm.json
    ├── expected_reports/               # Expected output reports for validation
    │   └── test_large_report.txt
    └── test_projects/                  # Sample NPM projects for testing
        └── e2e_test_project/
            └── package.json
```

## Test Data Organization

### CSV Files (`data/csv/`)
Contains vulnerability database CSV files with different scenarios:
- `test_format.csv` - Tests various CSV formatting scenarios
- `test_large.csv` - Large dataset for performance testing
- `test_partial.csv` - Partial matches testing
- `test_with_vuln.csv` - Known vulnerable packages for positive testing

### NPM Output Files (`data/npm_output/`)
Contains JSON files that simulate `npm list --json` output:
- Corresponding files for each CSV test scenario
- Error scenarios for error handling testing
- Various dependency tree structures

### Expected Reports (`data/expected_reports/`)
Contains expected output report files for validation:
- Used to verify report generation accuracy
- Different scenarios for various vulnerability states

### Test Projects (`data/test_projects/`)
Contains sample NPM projects with real package.json files:
- Used for end-to-end testing
- Simulates real-world project structures

## Test Output

Tests generate temporary files in `build/test_output/` during execution:
- Generated CSV files for dynamic test scenarios
- Temporary NPM output files
- Generated reports for validation

This directory is automatically created by the test framework and cleaned up appropriately.

## Path Management

All test data paths are centrally managed in `test_data_paths.h`:
- Use constants instead of hardcoded paths
- Easy to update paths in one location
- Consistent path handling across all tests

## Test Environment Setup

The `integration_test_setup.h` provides utilities for:
- Creating required test directories
- Setting up test environment
- Cross-platform directory creation

## Running Integration Tests

Use the organized test scripts:

**Windows:**
```cmd
scripts\windows\test.bat
```

**Linux/macOS:**
```bash
./scripts/test.sh
```

Or use specific test targets in the Makefile:
```bash
make test-integration
```

## Adding New Tests

When adding new integration tests:

1. **Add test data files** to appropriate `data/` subdirectories
2. **Update `test_data_paths.h`** with new path constants
3. **Include required headers** in your test files:
   ```c
   #include "test_data_paths.h"
   #include "integration_test_setup.h"
   ```
4. **Use constants** for file paths instead of hardcoded strings
5. **Call setup functions** if your test needs special environment setup

This organization makes tests more maintainable, paths more consistent, and test data easier to manage.