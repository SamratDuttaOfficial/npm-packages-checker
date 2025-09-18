#include "csv_parser.h"

int parse_versions(const char *version_str, char versions[][MAX_VERSION_LEN], int max_versions) {
    if (version_str == NULL || versions == NULL || max_versions <= 0) {
        return 0;
    }

    char temp[512];
    strncpy(temp, version_str, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    int count = 0;
    char *token = strtok(temp, " \t\n\r");

    while (token != NULL && count < max_versions) {
        trim_whitespace(token);
        if (strlen(token) > 0 && strlen(token) < MAX_VERSION_LEN) {
            strncpy(versions[count], token, MAX_VERSION_LEN - 1);
            versions[count][MAX_VERSION_LEN - 1] = '\0';
            count++;
        }
        token = strtok(NULL, " \t\n\r");
    }

    return count;
}

static int parse_csv_line(char *line, char **columns, int max_columns) {
    if (line == NULL || columns == NULL || max_columns <= 0) {
        return 0;
    }

    int col_count = 0;
    char *token = strtok(line, ",");

    while (token != NULL && col_count < max_columns) {
        columns[col_count++] = token;
        token = strtok(NULL, ",");
    }

    return col_count;
}

int load_csv_packages(const char *csv_path, int name_col, int version_col, PackageList *packages) {
    if (csv_path == NULL || packages == NULL || name_col < 0 || version_col < 0) {
        printf("Error: Invalid parameters for CSV loading\n");
        return 0;
    }

    FILE *file = fopen(csv_path, "r");
    if (file == NULL) {
        printf("Error: Cannot open CSV file: %s\n", csv_path);
        return 0;
    }

    char line[MAX_LINE_LEN];
    int line_number = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;

        if (strlen(line) <= 1) {
            continue;
        }

        if (line_number == 1) {
            continue;
        }

        char *columns[100];
        int col_count = parse_csv_line(line, columns, 100);

        if (col_count <= name_col || col_count <= version_col) {
            printf("Warning: Line %d has insufficient columns (found %d, need %d)\n",
                   line_number, col_count, (name_col > version_col ? name_col + 1 : version_col + 1));
            continue;
        }

        Package pkg = {0};

        strncpy(pkg.name, columns[name_col], sizeof(pkg.name) - 1);
        pkg.name[sizeof(pkg.name) - 1] = '\0';
        clean_package_name(pkg.name);

        // Remove version suffix if accidentally included in package name
        char *version_separator = strrchr(pkg.name, '@');
        if (version_separator != NULL && version_separator != pkg.name) {
            char *check = version_separator + 1;
            int looks_like_version = 0;
            while (*check) {
                if (isdigit(*check) || *check == '.' || *check == '-' || *check == 'v') {
                    looks_like_version = 1;
                    break;
                }
                check++;
            }
            if (looks_like_version) {
                *version_separator = '\0'; // Remove version part
            }
        }

        if (strlen(pkg.name) == 0) {
            printf("Warning: Empty package name on line %d\n", line_number);
            continue;
        }

        pkg.version_count = parse_versions(columns[version_col], pkg.versions, MAX_VERSIONS);
        if (pkg.version_count > 0) {
            if (!add_package(packages, &pkg)) {
                printf("Error: Failed to add package '%s' - out of memory\n", pkg.name);
                fclose(file);
                return 0;
            }
        } else {
            printf("Warning: No valid versions found for package '%s' on line %d\n",
                   pkg.name, line_number);
        }
    }

    fclose(file);
    printf("Successfully loaded %d packages from CSV file\n", packages->count);
    return 1;
}