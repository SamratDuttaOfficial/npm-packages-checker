#ifndef TEST_DATA_PATHS_H
#define TEST_DATA_PATHS_H

// Base paths for organized test data
#define TEST_DATA_BASE "tests/integration/data"
#define TEST_CSV_DIR TEST_DATA_BASE "/csv"
#define TEST_NPM_DIR TEST_DATA_BASE "/npm_output"
#define TEST_REPORTS_DIR TEST_DATA_BASE "/expected_reports"
#define TEST_PROJECTS_DIR TEST_DATA_BASE "/test_projects"

// CSV test data files
#define TEST_CSV_FORMAT TEST_CSV_DIR "/test_format.csv"
#define TEST_CSV_LARGE TEST_CSV_DIR "/test_large.csv"
#define TEST_CSV_PARTIAL TEST_CSV_DIR "/test_partial.csv"
#define TEST_CSV_WITH_VULN TEST_CSV_DIR "/test_with_vuln.csv"

// NPM output test data files
#define TEST_NPM_E2E_ERROR TEST_NPM_DIR "/e2e_error_npm.json"
#define TEST_NPM_E2E_OUTPUT TEST_NPM_DIR "/e2e_npm_output.json"
#define TEST_NPM_FORMAT TEST_NPM_DIR "/test_format_npm.json"
#define TEST_NPM_LARGE TEST_NPM_DIR "/test_large_npm.json"
#define TEST_NPM_PARTIAL TEST_NPM_DIR "/test_partial_npm.json"
#define TEST_NPM_WITH_VULN TEST_NPM_DIR "/test_with_vuln_npm.json"

// Expected reports test data files
#define TEST_REPORT_LARGE_EXPECTED TEST_REPORTS_DIR "/test_large_report.txt"

// Test projects
#define TEST_PROJECT_E2E TEST_PROJECTS_DIR "/e2e_test_project"

// Temporary test output directory (for generated files during tests)
#define TEST_OUTPUT_DIR "build/test_output"

#endif // TEST_DATA_PATHS_H