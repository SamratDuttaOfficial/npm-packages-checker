#include "../test_framework.h"
#include "../../src/cli.h"
#include "../mock_data/npm_mock.h"
#include "test_data_paths.h"
#include "integration_test_setup.h"

int test_cli_full_workflow_no_vulnerabilities() {
    setup_test_output_dir();

    const char *csv_content =
        "Package,Versions,Severity\n"
        "nonexistent-package,1.0.0,High\n"
        "another-fake-package,2.0.0,Medium\n";

    const char *csv_file = TEST_OUTPUT_DIR "/cli_test_no_vuln.csv";
    const char *npm_file = TEST_OUTPUT_DIR "/cli_test_no_vuln_npm.json";
    const char *report_file = TEST_OUTPUT_DIR "/cli_test_no_vuln_report.txt";

    create_test_file(csv_file, csv_content);
    create_test_file(npm_file, MOCK_NPM_SIMPLE_OUTPUT);

    char *argv[] = {"npm-vuln-checker", "--cli", "--csv", (char*)csv_file,
                   "--project", ".", "--output", (char*)report_file};
    int argc = 8;

    // Note: We can't easily test run_cli_mode directly without npm dependencies
    // So we test the argument parsing and validation instead
    CLIArgs args;
    int parse_result = parse_cli_arguments(argc, argv, &args);
    TEST_ASSERT_EQ(1, parse_result, "Should parse CLI arguments");

    int validate_result = validate_cli_args(&args);
    TEST_ASSERT_EQ(1, validate_result, "Should validate CLI arguments");

    TEST_ASSERT_STR_EQ(csv_file, args.csv_file_path, "Should have correct CSV path");
    TEST_ASSERT_STR_EQ(".", args.project_path, "Should have correct project path");
    TEST_ASSERT_STR_EQ(report_file, args.output_file_path, "Should have correct output path");

    return 1;
}

int test_cli_argument_parsing_edge_cases() {
    setup_test_output_dir();

    // Test with spaces in paths
    char *argv[] = {"program", "--cli", "--csv", "path with spaces.csv",
                   "--project", "/path/with spaces/project", "--output", "report file.txt"};
    int argc = 8;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);
    TEST_ASSERT_EQ(1, result, "Should handle paths with spaces");
    TEST_ASSERT_STR_EQ("path with spaces.csv", args.csv_file_path, "Should preserve spaces in CSV path");
    TEST_ASSERT_STR_EQ("/path/with spaces/project", args.project_path, "Should preserve spaces in project path");
    TEST_ASSERT_STR_EQ("report file.txt", args.output_file_path, "Should preserve spaces in output path");

    return 1;
}

int test_cli_custom_column_parsing() {
    char *argv[] = {"program", "--cli", "--csv", "test.csv", "--project", ".",
                   "--output", "report.txt", "--name-col", "5", "--version-col", "7"};
    int argc = 12;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);
    TEST_ASSERT_EQ(1, result, "Should parse custom columns");
    TEST_ASSERT_EQ(5, args.name_column, "Should parse name column correctly");
    TEST_ASSERT_EQ(7, args.version_column, "Should parse version column correctly");

    return 1;
}

int test_cli_invalid_arguments() {
    // Missing value for --csv
    char *argv1[] = {"program", "--cli", "--csv"};
    int argc1 = 3;
    CLIArgs args1;

    int result1 = parse_cli_arguments(argc1, argv1, &args1);
    TEST_ASSERT_EQ(-1, result1, "Should fail with missing CSV value");

    // Unknown argument
    char *argv2[] = {"program", "--cli", "--unknown-arg", "value"};
    int argc2 = 4;
    CLIArgs args2;

    int result2 = parse_cli_arguments(argc2, argv2, &args2);
    TEST_ASSERT_EQ(-1, result2, "Should fail with unknown argument");

    return 1;
}

int test_cli_help_functionality() {
    char *argv[] = {"program", "--cli", "--help"};
    int argc = 3;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);
    TEST_ASSERT_EQ(1, result, "Should parse help request");
    TEST_ASSERT_EQ(1, args.show_help, "Should set help flag");
    TEST_ASSERT_EQ(1, validate_cli_args(&args), "Help should always validate");

    return 1;
}

void run_cli_integration_tests(void) {
    printf("\n=== CLI Integration Tests ===\n");
    RUN_TEST(test_cli_full_workflow_no_vulnerabilities);
    RUN_TEST(test_cli_argument_parsing_edge_cases);
    RUN_TEST(test_cli_custom_column_parsing);
    RUN_TEST(test_cli_invalid_arguments);
    RUN_TEST(test_cli_help_functionality);
}