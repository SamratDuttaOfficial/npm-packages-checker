#include "common.h"

void trim_whitespace(char *str) {
    if (str == NULL || *str == '\0') {
        return;
    }

    char *start = str;
    char *end;

    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void clean_package_name(char *name) {
    if (name == NULL) {
        return;
    }

    trim_whitespace(name);

    char *src = name;
    char *dst = name;

    while (*src) {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

// Dynamic memory management functions
PackageList* create_package_list(void) {
    PackageList *list = calloc(1, sizeof(PackageList));  // Use calloc for zero-init
    if (list == NULL) {
        return NULL;
    }

    list->packages = calloc(100, sizeof(Package)); // Use calloc for zero-init
    if (list->packages == NULL) {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = 100;
    return list;
}

void free_package_list(PackageList *list) {
    if (list == NULL) return;

    if (list->packages != NULL) {
        free(list->packages);
    }
    free(list);
}

int add_package(PackageList *list, const Package *package) {
    if (list == NULL || package == NULL) {
        return 0;
    }

    // Resize if needed
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Package *new_packages = realloc(list->packages, sizeof(Package) * new_capacity);
        if (new_packages == NULL) {
            return 0; // Out of memory
        }
        list->packages = new_packages;
        list->capacity = new_capacity;
    }

    // Add the package
    list->packages[list->count] = *package;
    list->count++;
    return 1;
}

DependencyTree* create_dependency_tree(void) {
    DependencyTree *tree = calloc(1, sizeof(DependencyTree));  // Use calloc for zero-init
    if (tree == NULL) {
        return NULL;
    }

    tree->nodes = calloc(100, sizeof(DependencyNode)); // Use calloc for zero-init
    if (tree->nodes == NULL) {
        free(tree);
        return NULL;
    }

    tree->count = 0;
    tree->capacity = 100;
    return tree;
}

void free_dependency_tree(DependencyTree *tree) {
    if (tree == NULL) return;

    if (tree->nodes != NULL) {
        free(tree->nodes);
    }
    free(tree);
}

int add_dependency(DependencyTree *tree, const DependencyNode *node) {
    if (tree == NULL || node == NULL) {
        return 0;
    }

    // Resize if needed
    if (tree->count >= tree->capacity) {
        int new_capacity = tree->capacity * 2;
        DependencyNode *new_nodes = realloc(tree->nodes, sizeof(DependencyNode) * new_capacity);
        if (new_nodes == NULL) {
            return 0; // Out of memory
        }
        tree->nodes = new_nodes;
        tree->capacity = new_capacity;
    }

    // Add the node
    tree->nodes[tree->count] = *node;
    tree->count++;
    return 1;
}

// Safe string copy function to avoid truncation warnings
void safe_string_copy(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }

    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;

    memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
}