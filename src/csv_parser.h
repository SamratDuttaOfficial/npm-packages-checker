#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "common.h"

int parse_versions(const char *version_str, char versions[][MAX_VERSION_LEN], int max_versions);
int load_csv_packages(const char *csv_path, int name_col, int version_col, PackageList *packages);

#endif