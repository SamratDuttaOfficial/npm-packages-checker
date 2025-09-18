#include "test_framework.h"

// Unit test function declarations
void run_csv_parser_tests(void);
void run_vulnerability_checker_tests(void);
void run_report_generator_tests(void);

// Integration test function declarations
void run_npm_analyzer_integration_tests(void);
void run_full_workflow_integration_tests(void);

// End-to-end test function declarations
void run_end_to_end_tests(void);

int main(int argc, char *argv[]) {
    printf("==============================================\n");
    printf("NPM Vulnerability Checker - Test Suite\n");
    printf("==============================================\n");

    int run_unit = 1;
    int run_integration = 1;
    int run_e2e = 1;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--unit-only") == 0) {
            run_integration = 0;
            run_e2e = 0;
        } else if (strcmp(argv[i], "--integration-only") == 0) {
            run_unit = 0;
            run_e2e = 0;
        } else if (strcmp(argv[i], "--e2e-only") == 0) {
            run_unit = 0;
            run_integration = 0;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  --unit-only        Run only unit tests\n");
            printf("  --integration-only Run only integration tests\n");
            printf("  --e2e-only         Run only end-to-end tests\n");
            printf("  --help             Show this help message\n");
            return 0;
        }
    }

    // Run unit tests
    if (run_unit) {
        printf("\n▶ Running Unit Tests...\n");
        run_csv_parser_tests();
        run_vulnerability_checker_tests();
        run_report_generator_tests();
    }

    // Run integration tests
    if (run_integration) {
        printf("\n▶ Running Integration Tests...\n");
        run_npm_analyzer_integration_tests();
        run_full_workflow_integration_tests();
    }

    // Run end-to-end tests
    if (run_e2e) {
        printf("\n▶ Running End-to-End Tests...\n");
        run_end_to_end_tests();
    }

    // Print final summary
    PRINT_TEST_SUMMARY();

    // Return appropriate exit code
    return (global_test_suite.failed_tests > 0) ? 1 : 0;
}