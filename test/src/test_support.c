/*
FILE: test/src/test_support.c
*/

#include "include/test_support.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ME_Error test_load_text_file(const char *file_path, char **out_text)
{
    FILE *file;
    long file_size;
    size_t read_count;
    char *buffer;

    if ((file_path == NULL) || (out_text == NULL))
    {
        return ME_INVALID_TAG;
    }

    *out_text = NULL;
    file = fopen(file_path, "rb");
    if (file == NULL)
    {
        return ME_INVALID_TAG;
    }

    if (fseek(file, 0L, SEEK_END) != 0)
    {
        fclose(file);
        return ME_INVALID_TAG;
    }

    file_size = ftell(file);
    if (file_size < 0L)
    {
        fclose(file);
        return ME_INVALID_TAG;
    }

    if (fseek(file, 0L, SEEK_SET) != 0)
    {
        fclose(file);
        return ME_INVALID_TAG;
    }

    buffer = (char *)malloc((size_t)file_size + 1U);
    if (buffer == NULL)
    {
        fclose(file);
        return ME_OUT_OF_MEMORY;
    }

    read_count = fread(buffer, 1U, (size_t)file_size, file);
    fclose(file);
    buffer[read_count] = '\0';
    *out_text = buffer;
    return ME_OK;
}

ME_Error test_load_data_file(const char *file_path, ME_Context **out_context)
{
    char *text;
    char *cursor;
    char *line_start;
    ME_Context *context;
    ME_Error error;

    if ((file_path == NULL) || (out_context == NULL))
    {
        return ME_INVALID_TAG;
    }

    error = test_load_text_file(file_path, &text);
    if (error != ME_OK)
    {
        return error;
    }

    context = me_context_create();
    if (context == NULL)
    {
        free(text);
        return ME_OUT_OF_MEMORY;
    }

    cursor = text;
    while (*cursor != '\0')
    {
        char *line_end;
        char *separator;
        char saved_end;
        char saved_separator;

        line_start = cursor;
        while ((*cursor != '\0') && (*cursor != '\n'))
        {
            ++cursor;
        }
        line_end = cursor;
        if (*cursor == '\n')
        {
            ++cursor;
        }

        while ((line_end > line_start) && ((line_end[-1] == '\r') || (line_end[-1] == '\n')))
        {
            --line_end;
        }

        if (line_end == line_start)
        {
            continue;
        }

        saved_end = *line_end;
        *line_end = '\0';
        separator = strstr(line_start, "||");
        if (separator != NULL)
        {
            saved_separator = separator[0];
            separator[0] = '\0';
            separator[1] = '\0';
            error = me_context_set(context, line_start, separator + 2);
            separator[0] = saved_separator;
            separator[1] = '|';
            if (error != ME_OK)
            {
                *line_end = saved_end;
                me_context_destroy(context);
                free(text);
                return error;
            }
        }
        *line_end = saved_end;
    }

    free(text);
    *out_context = context;
    return ME_OK;
}

ME_Error test_load_scenario(const char *scenario_name, TestScenario *out_scenario)
{
    char path_buffer[512];
    char *expected_text;
    ME_Error error;

    if ((scenario_name == NULL) || (out_scenario == NULL))
    {
        return ME_INVALID_TAG;
    }

    memset(out_scenario, 0, sizeof(*out_scenario));
    out_scenario->name = scenario_name;

    snprintf(path_buffer, sizeof(path_buffer), "test/data/%s.tpl", scenario_name);
    error = test_load_text_file(path_buffer, (char **)&out_scenario->template_text);
    if (error != ME_OK)
    {
        return error;
    }

    snprintf(path_buffer, sizeof(path_buffer), "test/data/%s.dat", scenario_name);
    error = test_load_data_file(path_buffer, &out_scenario->data);
    if (error != ME_OK)
    {
        test_free_scenario(out_scenario);
        return error;
    }

    snprintf(path_buffer, sizeof(path_buffer), "test/data/%s.res", scenario_name);
    error = test_load_text_file(path_buffer, &expected_text);
    if (error != ME_OK)
    {
        test_free_scenario(out_scenario);
        return error;
    }

    if (strncmp(expected_text, "ERROR:", 6U) == 0)
    {
        const char *name = expected_text + 6;
        size_t length = strlen(name);
        while ((length > 0U) && ((name[length - 1U] == '\n') || (name[length - 1U] == '\r')))
        {
            --length;
        }

        out_scenario->expectation_kind = TEST_EXPECT_ERROR;
        out_scenario->expected_text = NULL;
        if ((length == 11U) && (strncmp(name, "INVALID_TAG", length) == 0))
        {
            out_scenario->expected_error = ME_INVALID_TAG;
        }
        else if ((length == 21U) && (strncmp(name, "MISSING_CLOSING_CURLY", length) == 0))
        {
            out_scenario->expected_error = ME_MISSING_CLOSING_CURLY;
        }
        else if ((length == 21U) && (strncmp(name, "MISSING_CLOSING_COLON", length) == 0))
        {
            out_scenario->expected_error = ME_MISSING_CLOSING_COLON;
        }
        else if ((length == 18U) && (strncmp(name, "TAG_CALLBACK_ERROR", length) == 0))
        {
            out_scenario->expected_error = ME_TAG_CALLBACK_ERROR;
        }
        else
        {
            free(expected_text);
            test_free_scenario(out_scenario);
            return ME_INVALID_TAG;
        }
        free(expected_text);
    }
    else
    {
        out_scenario->expectation_kind = TEST_EXPECT_TEXT;
        out_scenario->expected_text = expected_text;
    }

    return ME_OK;
}

void test_free_scenario(TestScenario *scenario)
{
    if (scenario == NULL)
    {
        return;
    }

    free((char *)scenario->template_text);
    free((char *)scenario->expected_text);
    me_context_destroy(scenario->data);
    memset(scenario, 0, sizeof(*scenario));
}
