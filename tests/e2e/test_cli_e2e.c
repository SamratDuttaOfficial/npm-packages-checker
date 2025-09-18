#include "../test_framework.h"
#include "../../src/cli.h"
#include "../mock_data/npm_mock.h"

int test_cli_e2e_help_display() {
    printf("\n--- Testing CLI help display ---\n");

    char *argv[] = {"npm-vuln-checker", "--cli", "--help"};
    int argc = 3;

    // Test help mode - we can't easily redirect stdout in a portable way
    // So we just test that help parsing works correctly
    CLIArgs args;
    int parse_result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(1, parse_result, "Should parse help request");
    TEST_ASSERT_EQ(1, args.show_help, "Should set help flag");

    printf("CLI help display test completed successfully\n");
    return 1;
}

int test_cli_e2e_argument_validation() {
    printf("\n--- Testing CLI argument validation ---\n");

    // Test missing required arguments
    char *argv1[] = {"npm-vuln-checker", "--cli", "--csv", "test.csv"};
    int argc1 = 4;

    int result1 = run_cli_mode(argc1, argv1);
    TEST_ASSERT_EQ(1, result1, "Should fail with missing required arguments");

    // Test invalid column numbers
    char *argv2[] = {"npm-vuln-checker", "--cli", "--csv", "test.csv",
                    "--project", ".", "--output", "report.txt",
                    "--name-col", "0"};
    int argc2 = 10;

    int result2 = run_cli_mode(argc2, argv2);
    TEST_ASSERT_EQ(1, result2, "Should fail with invalid column number");

    printf("CLI argument validation test completed\n");
    return 1;
}

int test_cli_e2e_non_cli_mode() {
    printf("\n--- Testing non-CLI mode detection ---\n");

    char *argv[] = {"npm-vuln-checker", "some", "random", "args"};
    int argc = 4;

    int result = run_cli_mode(argc, argv);
    TEST_ASSERT_EQ(-1, result, "Should return -1 for non-CLI mode");

    printf("Non-CLI mode detection test completed\n");
    return 1;
}

int test_cli_e2e_unknown_arguments() {
    printf("\n--- Testing unknown argument handling ---\n");

    char *argv[] = {"npm-vuln-checker", "--cli", "--unknown-flag", "value"};
    int argc = 4;

    int result = run_cli_mode(argc, argv);
    TEST_ASSERT_EQ(1, result, "Should fail with unknown arguments");

    printf("Unknown argument handling test completed\n");
    return 1;
}

int test_cli_e2e_missing_argument_values() {
    printf("\n--- Testing missing argument values ---\n");

    // Missing value for --csv
    char *argv1[] = {"npm-vuln-checker", "--cli", "--csv"};
    int argc1 = 3;

    int result1 = run_cli_mode(argc1, argv1);
    TEST_ASSERT_EQ(1, result1, "Should fail with missing CSV value");

    // Missing value for --project
    char *argv2[] = {"npm-vuln-checker", "--cli", "--project"};
    int argc2 = 3;

    int result2 = run_cli_mode(argc2, argv2);
    TEST_ASSERT_EQ(1, result2, "Should fail with missing project value");

    printf("Missing argument values test completed\n");
    return 1;
}

void run_cli_e2e_tests(void) {
    printf("\n=== CLI End-to-End Tests ===\n");
    RUN_TEST(test_cli_e2e_help_display);
    RUN_TEST(test_cli_e2e_argument_validation);
    RUN_TEST(test_cli_e2e_non_cli_mode);
    RUN_TEST(test_cli_e2e_unknown_arguments);
    RUN_TEST(test_cli_e2e_missing_argument_values);
}