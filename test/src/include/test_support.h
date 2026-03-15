/*
FILE: test/src/include/test_support.h
*/

#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include "../../../src/include/mighty_eagle_lib.h"

#include <stddef.h>

typedef enum TestExpectationKind
{
    TEST_EXPECT_TEXT = 0,
    TEST_EXPECT_ERROR
} TestExpectationKind;

typedef struct TestScenario
{
    const char *name;
    const char *template_text;
    const char *expected_text;
    ME_Error expected_error;
    ME_Context *data;
    TestExpectationKind expectation_kind;
} TestScenario;

ME_Error test_load_text_file(const char *file_path, char **out_text);
ME_Error test_load_data_file(const char *file_path, ME_Context **out_context);
ME_Error test_load_scenario(const char *scenario_name, TestScenario *out_scenario);
void test_free_scenario(TestScenario *scenario);

#endif
