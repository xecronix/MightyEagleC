/*
FILE: test/main.c
*/

#include "../src/include/mighty_eagle_lib.h"
#include "src/include/test_support.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TestStats
{
    int total;
    int passed;
    int failed;
} TestStats;

static void append_text(char **target, const char *text)
{
    size_t current_length;
    size_t add_length;
    char *next;

    current_length = (*target == NULL) ? 0U : strlen(*target);
    add_length = strlen(text);
    next = (char *)realloc(*target, current_length + add_length + 1U);
    if (next == NULL)
    {
        return;
    }
    *target = next;
    memcpy(*target + current_length, text, add_length + 1U);
}

static ME_Error action_cb(
    MightyEagle *me,
    const char *tag,
    const char *sub_template,
    const ME_Context *context,
    void *user_data,
    char **out_text)
{
    char *combined;
    char *rendered;
    ME_Context *loop_context;
    ME_Error error;
    int count;
    char count_text[16];

    (void)tag;
    (void)context;
    (void)user_data;

    combined = (char *)malloc(1U);
    if (combined == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }
    combined[0] = '\0';

    for (count = 1; count <= 2; ++count)
    {
        loop_context = me_context_create();
        if (loop_context == NULL)
        {
            free(combined);
            return ME_OUT_OF_MEMORY;
        }

        snprintf(count_text, sizeof(count_text), "%d", count);
        error = me_context_set(loop_context, "cnt", count_text);
        if (error != ME_OK)
        {
            me_context_destroy(loop_context);
            free(combined);
            return error;
        }

        error = me_parse(me, sub_template, loop_context, &rendered);
        me_context_destroy(loop_context);
        if (error != ME_OK)
        {
            free(combined);
            return error;
        }

        append_text(&combined, rendered);
        me_free_string(rendered);
    }

    *out_text = combined;
    return ME_OK;
}

static ME_Error tag_cb(
    MightyEagle *me,
    const char *tag,
    const char *tag_value,
    const ME_Context *context,
    void *user_data,
    char **out_text)
{
    char year[5];
    char month[3];
    char day[3];
    char *formatted;

    (void)me;
    (void)tag;
    (void)context;
    (void)user_data;

    *out_text = NULL;
    if (strcmp(tag_value, "1970-09-28") != 0)
    {
        return ME_OK;
    }

    memcpy(year, tag_value + 0, 4U);
    year[4] = '\0';
    memcpy(month, tag_value + 5, 2U);
    month[2] = '\0';
    memcpy(day, tag_value + 8, 2U);
    day[2] = '\0';

    formatted = (char *)malloc(11U);
    if (formatted == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }
    snprintf(formatted, 11U, "%s/%s/%s", month, day, year);
    *out_text = formatted;
    return ME_OK;
}

static ME_Error invalid_return_tag_cb(
    MightyEagle *me,
    const char *tag,
    const char *tag_value,
    const ME_Context *context,
    void *user_data,
    char **out_text)
{
    (void)me;
    (void)tag;
    (void)tag_value;
    (void)context;
    (void)user_data;
    (void)out_text;
    return ME_TAG_CALLBACK_ERROR;
}

static ME_Error apply_scenario_setup(MightyEagle *me, const char *scenario_name)
{
    if (strcmp(scenario_name, "parse_tag_cb") == 0)
    {
        return me_add_tag_cb(me, "birthday", tag_cb, NULL);
    }
    if (strcmp(scenario_name, "parse_tag_cb_error") == 0)
    {
        return me_add_tag_cb(me, "birthday", invalid_return_tag_cb, NULL);
    }
    if (strcmp(scenario_name, "parse_action_cb") == 0)
    {
        return me_add_action_cb(me, "sayit2x", action_cb, NULL);
    }
    return ME_OK;
}

static int run_scenario(const char *scenario_name, TestStats *stats)
{
    TestScenario scenario;
    MightyEagle *me;
    ME_Error error;
    char *actual_text;
    int passed;

    printf("RUN  %s\n", scenario_name);
    stats->total += 1;
    passed = 0;
    actual_text = NULL;
    me = NULL;

    error = test_load_scenario(scenario_name, &scenario);
    if (error != ME_OK)
    {
        printf("FAIL %s - setup error: %s\n", scenario_name, me_error_name(error));
        stats->failed += 1;
        return 0;
    }

    me = me_create();
    if (me == NULL)
    {
        printf("FAIL %s - could not create engine\n", scenario_name);
        test_free_scenario(&scenario);
        stats->failed += 1;
        return 0;
    }

    error = apply_scenario_setup(me, scenario_name);
    if (error == ME_OK)
    {
        error = me_parse(me, scenario.template_text, scenario.data, &actual_text);
    }

    if (scenario.expectation_kind == TEST_EXPECT_TEXT)
    {
        if ((error == ME_OK) && (actual_text != NULL) && (strcmp(actual_text, scenario.expected_text) == 0))
        {
            passed = 1;
        }
        else
        {
            printf("FAIL %s\n", scenario_name);
            printf("  expected text:\n%s\n", scenario.expected_text ? scenario.expected_text : "<null>");
            if (error != ME_OK)
            {
                printf("  actual error: %s\n", me_error_name(error));
            }
            else
            {
                printf("  actual text:\n%s\n", actual_text ? actual_text : "<null>");
            }
        }
    }
    else
    {
        if (error == scenario.expected_error)
        {
            passed = 1;
        }
        else
        {
            printf("FAIL %s\n", scenario_name);
            printf("  expected error: %s\n", me_error_name(scenario.expected_error));
            printf("  actual: %s\n", me_error_name(error));
        }
    }

    if (passed)
    {
        printf("PASS %s\n", scenario_name);
        stats->passed += 1;
    }
    else
    {
        stats->failed += 1;
    }

    me_free_string(actual_text);
    me_destroy(me);
    test_free_scenario(&scenario);
    return passed;
}

int main(void)
{
    /*
    FILE: test/main.c
    */

    static const char *SCENARIOS[] = {
        "parse_no_tags",
        "parse_tags",
        "parse_tag_cb",
        "parse_tag_cb_error",
        "parse_missing_tag_data",
        "parse_invalid_substitution_tag",
        "parse_missing_closing_substitution_curly",
        "parse_action_cb",
        "parse_unknown_action_cb",
        "parse_invalid_empty_action_tag",
        "parse_internal_space_substitution_tag",
        "parse_unclosed_substitution_tag",
        "parse_unclosed_action_tag"
    };

    TestStats stats;
    size_t index;

    stats.total = 0;
    stats.passed = 0;
    stats.failed = 0;

    for (index = 0U; index < (sizeof(SCENARIOS) / sizeof(SCENARIOS[0])); ++index)
    {
        run_scenario(SCENARIOS[index], &stats);
    }

    printf("\nSUMMARY\n");
    printf("  total  : %d\n", stats.total);
    printf("  passed : %d\n", stats.passed);
    printf("  failed : %d\n", stats.failed);

    return (stats.failed == 0) ? 0 : 1;
}
