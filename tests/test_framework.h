#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_TEST_NAME 256
#define MAX_TESTS 1000

typedef struct {
    char name[MAX_TEST_NAME];
    int passed;
    char error_msg[512];
} TestResult;

typedef struct {
    TestResult results[MAX_TESTS];
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestSuite;

extern TestSuite global_test_suite;

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s at %s:%d\n", message, __FILE__, __LINE__); \
            snprintf(global_test_suite.results[global_test_suite.total_tests].error_msg, \
                    sizeof(global_test_suite.results[global_test_suite.total_tests].error_msg), \
                    "ASSERTION FAILED: %s at %s:%d", message, __FILE__, __LINE__); \
            global_test_suite.results[global_test_suite.total_tests].passed = 0; \
            global_test_suite.failed_tests++; \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            snprintf(global_test_suite.results[global_test_suite.total_tests].error_msg, \
                    sizeof(global_test_suite.results[global_test_suite.total_tests].error_msg), \
                    "ASSERTION FAILED: %s - Expected: %d, Actual: %d at %s:%d", \
                    message, (int)(expected), (int)(actual), __FILE__, __LINE__); \
            global_test_suite.results[global_test_suite.total_tests].passed = 0; \
            global_test_suite.failed_tests++; \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQ(expected, actual, message) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            snprintf(global_test_suite.results[global_test_suite.total_tests].error_msg, \
                    sizeof(global_test_suite.results[global_test_suite.total_tests].error_msg), \
                    "ASSERTION FAILED: %s - Expected: '%s', Actual: '%s' at %s:%d", \
                    message, expected, actual, __FILE__, __LINE__); \
            global_test_suite.results[global_test_suite.total_tests].passed = 0; \
            global_test_suite.failed_tests++; \
            return 0; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running test: %s... ", #test_func); \
        fflush(stdout); \
        strncpy(global_test_suite.results[global_test_suite.total_tests].name, \
                #test_func, MAX_TEST_NAME - 1); \
        global_test_suite.results[global_test_suite.total_tests].passed = 1; \
        global_test_suite.results[global_test_suite.total_tests].error_msg[0] = '\0'; \
        if (test_func()) { \
            printf("PASS\n"); \
            global_test_suite.passed_tests++; \
        } else { \
            printf("FAIL\n"); \
            printf("  Error: %s\n", global_test_suite.results[global_test_suite.total_tests].error_msg); \
        } \
        global_test_suite.total_tests++; \
    } while(0)

#define PRINT_TEST_SUMMARY() \
    do { \
        printf("\n=== TEST SUMMARY ===\n"); \
        printf("Total tests: %d\n", global_test_suite.total_tests); \
        printf("Passed: %d\n", global_test_suite.passed_tests); \
        printf("Failed: %d\n", global_test_suite.failed_tests); \
        printf("Success rate: %.2f%%\n", \
               global_test_suite.total_tests > 0 ? \
               (100.0 * global_test_suite.passed_tests / global_test_suite.total_tests) : 0.0); \
        if (global_test_suite.failed_tests > 0) { \
            printf("\nFailed tests:\n"); \
            for (int i = 0; i < global_test_suite.total_tests; i++) { \
                if (!global_test_suite.results[i].passed) { \
                    printf("  - %s: %s\n", global_test_suite.results[i].name, \
                           global_test_suite.results[i].error_msg); \
                } \
            } \
        } \
        printf("==================\n"); \
    } while(0)

void create_test_file(const char *filename, const char *content);
int file_exists(const char *filename);
void cleanup_test_file(const char *filename);
void load_fixture_file(const char *fixture_name, const char *temp_filename);

#endif