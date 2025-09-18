#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PATH_LEN 1024
#define MAX_LINE_LEN 4096
#define MAX_PACKAGES 20
#define MAX_VERSIONS 100
#define MAX_PACKAGE_NAME_LEN 256
#define MAX_VERSION_LEN 32
#define MAX_DEPENDENCY_PATH 512

#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

typedef struct {
    char name[MAX_PACKAGE_NAME_LEN];
    char versions[MAX_VERSIONS][MAX_VERSION_LEN];
    int version_count;
} Package;

typedef struct {
    char name[MAX_PACKAGE_NAME_LEN];
    char version[MAX_VERSION_LEN];
    char path[MAX_DEPENDENCY_PATH];
    int is_direct;
} DependencyNode;

typedef struct {
    Package *packages;
    int count;
    int capacity;
} PackageList;

typedef struct {
    DependencyNode *nodes;
    int count;
    int capacity;
} DependencyTree;

typedef struct {
    char npm_project_path[MAX_PATH_LEN];
    char csv_file_path[MAX_PATH_LEN];
    int package_name_column;
    int version_column;
    char output_directory[MAX_PATH_LEN];
} AppState;

void trim_whitespace(char *str);
void clean_package_name(char *name);

// Dynamic memory management functions
PackageList* create_package_list(void);
void free_package_list(PackageList *list);
int add_package(PackageList *list, const Package *package);

DependencyTree* create_dependency_tree(void);
void free_dependency_tree(DependencyTree *tree);
int add_dependency(DependencyTree *tree, const DependencyNode *node);

// Safe string copy function to avoid truncation warnings
void safe_string_copy(char *dest, const char *src, size_t dest_size);

#endif