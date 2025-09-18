#ifndef NPM_MOCK_H
#define NPM_MOCK_H

#include "../test_framework.h"
#include "../../src/common.h"

void create_mock_npm_project(const char *project_path);
void create_mock_package_json(const char *project_path);
void create_mock_npm_dependencies_output(const char *output_file);
void cleanup_mock_npm_project(const char *project_path);

#define MOCK_NPM_SIMPLE_OUTPUT \
"{\n" \
"  \"name\": \"test-project\",\n" \
"  \"version\": \"1.0.0\",\n" \
"  \"dependencies\": {\n" \
"    \"lodash\": {\n" \
"      \"version\": \"4.17.20\"\n" \
"    },\n" \
"    \"express\": {\n" \
"      \"version\": \"4.17.1\"\n" \
"    },\n" \
"    \"moment\": {\n" \
"      \"version\": \"2.24.0\"\n" \
"    }\n" \
"  }\n" \
"}"

#define MOCK_NPM_COMPLEX_OUTPUT \
"{\n" \
"  \"name\": \"complex-project\",\n" \
"  \"version\": \"2.0.0\",\n" \
"  \"dependencies\": {\n" \
"    \"lodash\": {\n" \
"      \"version\": \"4.17.20\",\n" \
"      \"dependencies\": {}\n" \
"    },\n" \
"    \"express\": {\n" \
"      \"version\": \"4.17.1\",\n" \
"      \"dependencies\": {\n" \
"        \"debug\": {\n" \
"          \"version\": \"2.6.9\"\n" \
"        }\n" \
"      }\n" \
"    },\n" \
"    \"axios\": {\n" \
"      \"version\": \"0.21.1\"\n" \
"    },\n" \
"    \"underscore\": {\n" \
"      \"version\": \"1.9.1\"\n" \
"    }\n" \
"  }\n" \
"}"

#define MOCK_NPM_VULNERABLE_OUTPUT \
"{\n" \
"  \"name\": \"vulnerable-project\",\n" \
"  \"version\": \"1.0.0\",\n" \
"  \"dependencies\": {\n" \
"    \"lodash\": {\n" \
"      \"version\": \"4.17.19\"\n" \
"    },\n" \
"    \"express\": {\n" \
"      \"version\": \"4.16.4\"\n" \
"    },\n" \
"    \"moment\": {\n" \
"      \"version\": \"2.29.1\"\n" \
"    },\n" \
"    \"axios\": {\n" \
"      \"version\": \"0.18.0\"\n" \
"    }\n" \
"  }\n" \
"}"

#define MOCK_NPM_EMPTY_OUTPUT \
"{\n" \
"  \"name\": \"empty-project\",\n" \
"  \"version\": \"1.0.0\",\n" \
"  \"dependencies\": {}\n" \
"}"

#endif