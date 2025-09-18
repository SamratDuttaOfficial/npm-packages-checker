#ifndef NPM_ANALYZER_H
#define NPM_ANALYZER_H

#include "common.h"

int get_npm_dependencies(const char *project_path, const char *output_dir, DependencyTree *tree);
int parse_npm_json_output(const char *json_file, DependencyTree *tree);

#endif