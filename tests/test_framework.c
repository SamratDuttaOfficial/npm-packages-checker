#include "test_framework.h"

TestSuite global_test_suite = {0};

void create_test_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, "%s", content);
        fclose(file);
    }
}

int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}

void cleanup_test_file(const char *filename) {
    remove(filename);
}

void load_fixture_file(const char *fixture_name, const char *temp_filename) {
    char fixture_path[1024];
    snprintf(fixture_path, sizeof(fixture_path), "tests/fixtures/%s", fixture_name);

    FILE *source = fopen(fixture_path, "r");
    if (source == NULL) {
        printf("Warning: Could not load fixture file: %s\n", fixture_path);
        return;
    }

    FILE *dest = fopen(temp_filename, "w");
    if (dest == NULL) {
        printf("Warning: Could not create temp file: %s\n", temp_filename);
        fclose(source);
        return;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(source);
    fclose(dest);
}