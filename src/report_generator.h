#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "common.h"
#include "vulnerability_checker.h"

int generate_vulnerability_report(const VulnerabilityResults *results,
                                 const PackageList *vulnerable_packages,
                                 const DependencyTree *dependencies,
                                 char *report,
                                 size_t report_size);

int save_report_to_file(const char *report_path, const char *report_content);

#endif