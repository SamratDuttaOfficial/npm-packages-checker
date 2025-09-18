#ifndef INTEGRATION_TEST_SETUP_H
#define INTEGRATION_TEST_SETUP_H

#include "test_data_paths.h"
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
    #include <direct.h>  // For _mkdir on Windows
#endif

// Function to create test output directory if it doesn't exist
static int setup_test_output_dir(void) {
#ifdef _WIN32
    // Windows version
    return _mkdir(TEST_OUTPUT_DIR) == 0 || errno == EEXIST;
#else
    // Unix/Linux/macOS version
    return mkdir(TEST_OUTPUT_DIR, 0755) == 0 || errno == EEXIST;
#endif
}

// Function to setup all required test directories
// Note: This function is available for future use but currently unused
#ifdef __GNUC__
__attribute__((unused))
#endif
static int setup_integration_test_environment(void) {
    // Create build directory first
#ifdef _WIN32
    _mkdir("build");
#else
    mkdir("build", 0755);
#endif

    // Create test output directory
    if (!setup_test_output_dir()) {
        printf("Failed to create test output directory: %s\n", TEST_OUTPUT_DIR);
        return 0;
    }

    return 1;
}

#endif // INTEGRATION_TEST_SETUP_H