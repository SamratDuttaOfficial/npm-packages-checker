# Test Fixtures

This directory contains test fixture files that provide consistent, reusable test data for the NPM vulnerability checker test suite.

## Structure

All fixtures are organized into three categories with matching names:

### CSV Files (`csv/`)
Vulnerability database files in various formats:
- `scenario_safe_packages.csv` - Safe packages only
- `scenario_mixed_vulnerabilities.csv` - Mixed safe/vulnerable packages
- `scenario_partial_matches.csv` - Packages for partial matching tests
- `edge_case_empty_file.csv` - Empty file test case
- `edge_case_complex_format.csv` - Complex CSV format with multiple columns
- `edge_case_malformed_data.csv` - Malformed/inconsistent CSV data

### NPM Output Files (`npm/`)
NPM dependency tree JSON files:
- `scenario_safe_dependencies.json` - Safe dependency versions
- `scenario_mixed_dependencies.json` - Mix of safe/vulnerable dependencies
- `scenario_partial_dependencies.json` - Dependencies for partial matching
- `edge_case_empty_file.json` - Empty dependencies
- `edge_case_complex_format.json` - Complex dependency tree
- `edge_case_malformed_data.json` - Malformed dependency data
- `basic_simple_output.json` - Basic 3-package output (lodash, express, moment)
- `basic_vulnerable_output.json` - Basic vulnerable packages output

### Expected Reports (`expected_reports/`)
Expected vulnerability report outputs:
- `scenario_safe_status_ok.txt` - Clean project report
- `scenario_mixed_status_vulnerable.txt` - Mixed vulnerabilities report
- `scenario_partial_status_warning.txt` - Partial matches with warnings
- `edge_case_empty_file_status_ok.txt` - Empty project report
- `edge_case_complex_format_status_vulnerable.txt` - Complex vulnerabilities report
- `edge_case_malformed_data_status_ok.txt` - Malformed data handling report

## Usage

### In Tests
Use the `load_fixture_file()` helper function to load fixture files:

```c
// Load NPM dependency data
load_fixture_file("npm/scenario_safe_dependencies.json", "temp_npm.json");

// Load CSV vulnerability data
load_fixture_file("csv/scenario_mixed_vulnerabilities.csv", "temp_vulns.csv");

// Load expected report for comparison
load_fixture_file("expected_reports/scenario_safe_status_ok.txt", "expected.txt");
```

### Fixture Matching
Each test scenario has matching fixtures across all three directories:
- `scenario_*` files test complete workflows
- `edge_case_*` files test error handling and edge cases
- `basic_*` files provide simple test data

## Benefits

1. **Consistency** - All tests use the same reference data
2. **Maintainability** - Test data changes in one place
3. **Readability** - Large test data doesn't clutter test code
4. **Reusability** - Fixtures can be shared across test suites
5. **Validation** - Expected outputs can be version controlled

## Backward Compatibility

Existing tests continue to use hardcoded mock data (`MOCK_NPM_*` constants) for backward compatibility. New tests should use fixture files.