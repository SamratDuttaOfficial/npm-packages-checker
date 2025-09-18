#include "../test_framework.h"
#include "../../src/cli.h"
#include <string.h>

int test_parse_cli_arguments_basic() {
    char *argv[] = {"program", "--cli", "--csv", "test.csv", "--project", ".", "--output", "report.txt"};
    int argc = 8;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(1, result, "Should detect CLI mode");
    TEST_ASSERT_STR_EQ("test.csv", args.csv_file_path, "Should parse CSV path");
    TEST_ASSERT_STR_EQ(".", args.project_path, "Should parse project path");
    TEST_ASSERT_STR_EQ("report.txt", args.output_file_path, "Should parse output path");
    TEST_ASSERT_EQ(1, args.name_column, "Should use default name column");
    TEST_ASSERT_EQ(2, args.version_column, "Should use default version column");
    TEST_ASSERT_EQ(0, args.show_help, "Should not show help by default");

    return 1;
}

int test_parse_cli_arguments_with_columns() {
    char *argv[] = {"program", "--cli", "--csv", "vulns.csv", "--project", "/path/to/project",
                   "--output", "/tmp/report.txt", "--name-col", "3", "--version-col", "4"};
    int argc = 12;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(1, result, "Should detect CLI mode");
    TEST_ASSERT_STR_EQ("vulns.csv", args.csv_file_path, "Should parse CSV path");
    TEST_ASSERT_STR_EQ("/path/to/project", args.project_path, "Should parse project path");
    TEST_ASSERT_STR_EQ("/tmp/report.txt", args.output_file_path, "Should parse output path");
    TEST_ASSERT_EQ(3, args.name_column, "Should parse custom name column");
    TEST_ASSERT_EQ(4, args.version_column, "Should parse custom version column");

    return 1;
}

int test_parse_cli_arguments_help() {
    char *argv[] = {"program", "--cli", "--help"};
    int argc = 3;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(1, result, "Should detect CLI mode");
    TEST_ASSERT_EQ(1, args.show_help, "Should show help");

    return 1;
}

int test_parse_cli_arguments_not_cli_mode() {
    char *argv[] = {"program", "some", "other", "args"};
    int argc = 4;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(0, result, "Should not detect CLI mode");

    return 1;
}

int test_parse_cli_arguments_missing_required() {
    char *argv[] = {"program", "--cli", "--csv", "test.csv"};
    int argc = 4;
    CLIArgs args;

    int result = parse_cli_arguments(argc, argv, &args);

    TEST_ASSERT_EQ(1, result, "Should parse successfully");
    TEST_ASSERT_EQ(0, validate_cli_args(&args), "Should fail validation due to missing required args");

    return 1;
}

int test_validate_cli_args_valid() {
    CLIArgs args = {0};
    strcpy(args.csv_file_path, "test.csv");
    strcpy(args.project_path, ".");
    strcpy(args.output_file_path, "report.txt");
    args.name_column = 1;
    args.version_column = 2;
    args.show_help = 0;

    int result = validate_cli_args(&args);

    TEST_ASSERT_EQ(1, result, "Should validate successfully");

    return 1;
}

int test_validate_cli_args_invalid_columns() {
    CLIArgs args = {0};
    strcpy(args.csv_file_path, "test.csv");
    strcpy(args.project_path, ".");
    strcpy(args.output_file_path, "report.txt");
    args.name_column = 0;  // Invalid
    args.version_column = 2;

    int result = validate_cli_args(&args);

    TEST_ASSERT_EQ(0, result, "Should fail validation due to invalid column");

    return 1;
}

int test_validate_cli_args_help_always_valid() {
    CLIArgs args = {0};  // Empty args
    args.show_help = 1;

    int result = validate_cli_args(&args);

    TEST_ASSERT_EQ(1, result, "Help should always be valid");

    return 1;
}

void run_cli_tests(void) {
    printf("\n=== CLI Unit Tests ===\n");
    RUN_TEST(test_parse_cli_arguments_basic);
    RUN_TEST(test_parse_cli_arguments_with_columns);
    RUN_TEST(test_parse_cli_arguments_help);
    RUN_TEST(test_parse_cli_arguments_not_cli_mode);
    RUN_TEST(test_parse_cli_arguments_missing_required);
    RUN_TEST(test_validate_cli_args_valid);
    RUN_TEST(test_validate_cli_args_invalid_columns);
    RUN_TEST(test_validate_cli_args_help_always_valid);
}