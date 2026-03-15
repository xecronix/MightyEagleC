/*
FILE: mighty_eagle_lib.c
*/

#include "src/include/mighty_eagle_lib.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ME_StringBuilder
{
    char *data;
    size_t length;
    size_t capacity;
} ME_StringBuilder;

typedef struct ME_ContextEntry
{
    char *key;
    char *value;
} ME_ContextEntry;

struct ME_Context
{
    ME_ContextEntry *entries;
    size_t count;
    size_t capacity;
};

typedef struct ME_TagRegistration
{
    char *tag;
    ME_TagCallback callback;
    void *user_data;
} ME_TagRegistration;

typedef struct ME_ActionRegistration
{
    char *tag;
    ME_ActionCallback callback;
    void *user_data;
} ME_ActionRegistration;

struct MightyEagle
{
    ME_TagRegistration *tag_callbacks;
    size_t tag_callback_count;
    size_t tag_callback_capacity;

    ME_ActionRegistration *action_callbacks;
    size_t action_callback_count;
    size_t action_callback_capacity;
};

static int me_is_valid_tag_char(int ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
    {
        return 1;
    }
    if ((ch >= 'A') && (ch <= 'Z'))
    {
        return 1;
    }
    if ((ch >= '0') && (ch <= '9'))
    {
        return 1;
    }
    return (ch == '_') || (ch == '.');
}

static char *me_strdup(const char *text)
{
    size_t length;
    char *copy;

    if (text == NULL)
    {
        return NULL;
    }

    length = strlen(text);
    copy = (char *)malloc(length + 1U);
    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, text, length + 1U);
    return copy;
}

static char *me_substr(const char *text, size_t start, size_t length)
{
    char *copy;

    copy = (char *)malloc(length + 1U);
    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, text + start, length);
    copy[length] = '\0';
    return copy;
}

static void me_sb_init(ME_StringBuilder *builder)
{
    builder->data = NULL;
    builder->length = 0U;
    builder->capacity = 0U;
}

static void me_sb_free(ME_StringBuilder *builder)
{
    free(builder->data);
    builder->data = NULL;
    builder->length = 0U;
    builder->capacity = 0U;
}

static ME_Error me_sb_reserve(ME_StringBuilder *builder, size_t extra)
{
    size_t needed;
    size_t capacity;
    char *next;

    needed = builder->length + extra + 1U;
    if (needed <= builder->capacity)
    {
        return ME_OK;
    }

    capacity = (builder->capacity == 0U) ? 64U : builder->capacity;
    while (capacity < needed)
    {
        capacity *= 2U;
    }

    next = (char *)realloc(builder->data, capacity);
    if (next == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    builder->data = next;
    builder->capacity = capacity;
    return ME_OK;
}

static ME_Error me_sb_append_n(ME_StringBuilder *builder, const char *text, size_t length)
{
    ME_Error error;

    error = me_sb_reserve(builder, length);
    if (error != ME_OK)
    {
        return error;
    }

    memcpy(builder->data + builder->length, text, length);
    builder->length += length;
    builder->data[builder->length] = '\0';
    return ME_OK;
}

static ME_Error me_sb_append_cstr(ME_StringBuilder *builder, const char *text)
{
    if (text == NULL)
    {
        return me_sb_append_n(builder, "", 0U);
    }
    return me_sb_append_n(builder, text, strlen(text));
}

static ME_Error me_sb_append_char(ME_StringBuilder *builder, char ch)
{
    return me_sb_append_n(builder, &ch, 1U);
}

static char *me_sb_take(ME_StringBuilder *builder)
{
    char *result;

    if (builder->data == NULL)
    {
        result = me_strdup("");
        return result;
    }

    result = builder->data;
    builder->data = NULL;
    builder->length = 0U;
    builder->capacity = 0U;
    return result;
}

ME_Context *me_context_create(void)
{
    ME_Context *context;

    context = (ME_Context *)calloc(1U, sizeof(*context));
    return context;
}

void me_context_destroy(ME_Context *context)
{
    size_t index;

    if (context == NULL)
    {
        return;
    }

    for (index = 0U; index < context->count; ++index)
    {
        free(context->entries[index].key);
        free(context->entries[index].value);
    }
    free(context->entries);
    free(context);
}

static ME_Error me_context_reserve(ME_Context *context, size_t extra)
{
    size_t needed;
    size_t capacity;
    ME_ContextEntry *next;

    needed = context->count + extra;
    if (needed <= context->capacity)
    {
        return ME_OK;
    }

    capacity = (context->capacity == 0U) ? 8U : context->capacity;
    while (capacity < needed)
    {
        capacity *= 2U;
    }

    next = (ME_ContextEntry *)realloc(context->entries, capacity * sizeof(*next));
    if (next == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    context->entries = next;
    context->capacity = capacity;
    return ME_OK;
}

ME_Error me_context_set(ME_Context *context, const char *key, const char *value)
{
    size_t index;
    char *key_copy;
    char *value_copy;
    ME_Error error;

    if ((context == NULL) || (key == NULL) || (value == NULL))
    {
        return ME_INVALID_TAG;
    }

    for (index = 0U; index < context->count; ++index)
    {
        if (strcmp(context->entries[index].key, key) == 0)
        {
            value_copy = me_strdup(value);
            if (value_copy == NULL)
            {
                return ME_OUT_OF_MEMORY;
            }
            free(context->entries[index].value);
            context->entries[index].value = value_copy;
            return ME_OK;
        }
    }

    error = me_context_reserve(context, 1U);
    if (error != ME_OK)
    {
        return error;
    }

    key_copy = me_strdup(key);
    value_copy = me_strdup(value);
    if ((key_copy == NULL) || (value_copy == NULL))
    {
        free(key_copy);
        free(value_copy);
        return ME_OUT_OF_MEMORY;
    }

    context->entries[context->count].key = key_copy;
    context->entries[context->count].value = value_copy;
    context->count += 1U;
    return ME_OK;
}

const char *me_context_get(const ME_Context *context, const char *key)
{
    size_t index;

    if ((context == NULL) || (key == NULL))
    {
        return NULL;
    }

    for (index = 0U; index < context->count; ++index)
    {
        if (strcmp(context->entries[index].key, key) == 0)
        {
            return context->entries[index].value;
        }
    }

    return NULL;
}

ME_Error me_context_clone(const ME_Context *source, ME_Context **out_clone)
{
    ME_Context *clone;
    size_t index;
    ME_Error error;

    if (out_clone == NULL)
    {
        return ME_INVALID_TAG;
    }

    clone = me_context_create();
    if (clone == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    if (source != NULL)
    {
        for (index = 0U; index < source->count; ++index)
        {
            error = me_context_set(clone, source->entries[index].key, source->entries[index].value);
            if (error != ME_OK)
            {
                me_context_destroy(clone);
                return error;
            }
        }
    }

    *out_clone = clone;
    return ME_OK;
}

MightyEagle *me_create(void)
{
    MightyEagle *me;

    me = (MightyEagle *)calloc(1U, sizeof(*me));
    return me;
}

void me_destroy(MightyEagle *me)
{
    size_t index;

    if (me == NULL)
    {
        return;
    }

    for (index = 0U; index < me->tag_callback_count; ++index)
    {
        free(me->tag_callbacks[index].tag);
    }
    free(me->tag_callbacks);

    for (index = 0U; index < me->action_callback_count; ++index)
    {
        free(me->action_callbacks[index].tag);
    }
    free(me->action_callbacks);
    free(me);
}

static ME_Error me_tag_callbacks_reserve(MightyEagle *me, size_t extra)
{
    size_t needed;
    size_t capacity;
    ME_TagRegistration *next;

    needed = me->tag_callback_count + extra;
    if (needed <= me->tag_callback_capacity)
    {
        return ME_OK;
    }

    capacity = (me->tag_callback_capacity == 0U) ? 4U : me->tag_callback_capacity;
    while (capacity < needed)
    {
        capacity *= 2U;
    }

    next = (ME_TagRegistration *)realloc(me->tag_callbacks, capacity * sizeof(*next));
    if (next == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    me->tag_callbacks = next;
    me->tag_callback_capacity = capacity;
    return ME_OK;
}

static ME_Error me_action_callbacks_reserve(MightyEagle *me, size_t extra)
{
    size_t needed;
    size_t capacity;
    ME_ActionRegistration *next;

    needed = me->action_callback_count + extra;
    if (needed <= me->action_callback_capacity)
    {
        return ME_OK;
    }

    capacity = (me->action_callback_capacity == 0U) ? 4U : me->action_callback_capacity;
    while (capacity < needed)
    {
        capacity *= 2U;
    }

    next = (ME_ActionRegistration *)realloc(me->action_callbacks, capacity * sizeof(*next));
    if (next == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    me->action_callbacks = next;
    me->action_callback_capacity = capacity;
    return ME_OK;
}

ME_Error me_add_tag_cb(
    MightyEagle *me,
    const char *tag,
    ME_TagCallback callback,
    void *user_data)
{
    size_t index;
    char *tag_copy;
    ME_Error error;

    if ((me == NULL) || (tag == NULL) || (callback == NULL))
    {
        return ME_INVALID_TAG;
    }

    for (index = 0U; index < me->tag_callback_count; ++index)
    {
        if (strcmp(me->tag_callbacks[index].tag, tag) == 0)
        {
            me->tag_callbacks[index].callback = callback;
            me->tag_callbacks[index].user_data = user_data;
            return ME_OK;
        }
    }

    error = me_tag_callbacks_reserve(me, 1U);
    if (error != ME_OK)
    {
        return error;
    }

    tag_copy = me_strdup(tag);
    if (tag_copy == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    me->tag_callbacks[me->tag_callback_count].tag = tag_copy;
    me->tag_callbacks[me->tag_callback_count].callback = callback;
    me->tag_callbacks[me->tag_callback_count].user_data = user_data;
    me->tag_callback_count += 1U;
    return ME_OK;
}

ME_Error me_add_action_cb(
    MightyEagle *me,
    const char *tag,
    ME_ActionCallback callback,
    void *user_data)
{
    size_t index;
    char *tag_copy;
    ME_Error error;

    if ((me == NULL) || (tag == NULL) || (callback == NULL))
    {
        return ME_INVALID_TAG;
    }

    for (index = 0U; index < me->action_callback_count; ++index)
    {
        if (strcmp(me->action_callbacks[index].tag, tag) == 0)
        {
            me->action_callbacks[index].callback = callback;
            me->action_callbacks[index].user_data = user_data;
            return ME_OK;
        }
    }

    error = me_action_callbacks_reserve(me, 1U);
    if (error != ME_OK)
    {
        return error;
    }

    tag_copy = me_strdup(tag);
    if (tag_copy == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    me->action_callbacks[me->action_callback_count].tag = tag_copy;
    me->action_callbacks[me->action_callback_count].callback = callback;
    me->action_callbacks[me->action_callback_count].user_data = user_data;
    me->action_callback_count += 1U;
    return ME_OK;
}

static ME_TagRegistration *me_find_tag_registration(MightyEagle *me, const char *tag)
{
    size_t index;

    if ((me == NULL) || (tag == NULL))
    {
        return NULL;
    }

    for (index = 0U; index < me->tag_callback_count; ++index)
    {
        if (strcmp(me->tag_callbacks[index].tag, tag) == 0)
        {
            return &me->tag_callbacks[index];
        }
    }

    return NULL;
}

static ME_ActionRegistration *me_find_action_registration(MightyEagle *me, const char *tag)
{
    size_t index;

    if ((me == NULL) || (tag == NULL))
    {
        return NULL;
    }

    for (index = 0U; index < me->action_callback_count; ++index)
    {
        if (strcmp(me->action_callbacks[index].tag, tag) == 0)
        {
            return &me->action_callbacks[index];
        }
    }

    return NULL;
}

static ME_Error me_parse_internal(
    MightyEagle *me,
    const char *template_str,
    const ME_Context *context,
    ME_StringBuilder *output);

static ME_Error me_handle_substitution(
    MightyEagle *me,
    const char *template_str,
    size_t start,
    const ME_Context *context,
    ME_StringBuilder *output,
    size_t *out_next_index)
{
    size_t index;
    size_t name_start;
    size_t name_end;
    size_t scan;
    size_t close_end;
    char *tag_name;
    char *original_text;
    const char *tag_value;
    char *callback_text;
    ME_TagRegistration *registration;
    ME_Error error;

    index = start + 2U;
    while ((template_str[index] != '\0') && isspace((unsigned char)template_str[index]))
    {
        ++index;
    }

    if (!me_is_valid_tag_char((unsigned char)template_str[index]))
    {
        return ME_INVALID_TAG;
    }

    name_start = index;
    while (me_is_valid_tag_char((unsigned char)template_str[index]))
    {
        ++index;
    }
    name_end = index;

    scan = index;
    while ((template_str[scan] != '\0') && isspace((unsigned char)template_str[scan]))
    {
        ++scan;
    }

    if (me_is_valid_tag_char((unsigned char)template_str[scan]))
    {
        return ME_INVALID_TAG;
    }

    if (template_str[scan] != ':')
    {
        if (template_str[scan] == '}')
        {
            return ME_MISSING_CLOSING_COLON;
        }
        return ME_INVALID_TAG;
    }

    if (template_str[scan + 1U] != '}')
    {
        return ME_MISSING_CLOSING_CURLY;
    }

    close_end = scan + 2U;
    tag_name = me_substr(template_str, name_start, name_end - name_start);
    if (tag_name == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    callback_text = NULL;
    registration = me_find_tag_registration(me, tag_name);
    if (registration != NULL)
    {
        tag_value = me_context_get(context, tag_name);
        if (tag_value == NULL)
        {
            tag_value = "";
        }
        error = registration->callback(
            me,
            tag_name,
            tag_value,
            context,
            registration->user_data,
            &callback_text);
        if (error != ME_OK)
        {
            free(tag_name);
            return error;
        }
        if (callback_text != NULL)
        {
            error = me_sb_append_cstr(output, callback_text);
            free(callback_text);
            free(tag_name);
            *out_next_index = close_end;
            return error;
        }
    }

    tag_value = me_context_get(context, tag_name);
    if (tag_value != NULL)
    {
        error = me_sb_append_cstr(output, tag_value);
        free(tag_name);
        *out_next_index = close_end;
        return error;
    }

    original_text = me_substr(template_str, start, close_end - start);
    free(tag_name);
    if (original_text == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    error = me_sb_append_cstr(output, original_text);
    free(original_text);
    *out_next_index = close_end;
    return error;
}

static ME_Error me_handle_action(
    MightyEagle *me,
    const char *template_str,
    size_t start,
    const ME_Context *context,
    ME_StringBuilder *output,
    size_t *out_next_index)
{
    size_t index;
    size_t name_start;
    size_t name_end;
    size_t body_start;
    size_t close_start;
    size_t close_end;
    int level;
    char *tag_name;
    char *sub_template;
    char *original_text;
    char *callback_text;
    ME_ActionRegistration *registration;
    ME_Error error;

    index = start + 2U;
    if (!me_is_valid_tag_char((unsigned char)template_str[index]))
    {
        return ME_INVALID_TAG;
    }

    name_start = index;
    while (me_is_valid_tag_char((unsigned char)template_str[index]))
    {
        ++index;
    }
    name_end = index;
    body_start = index;
    while (template_str[body_start] == ' ')
    {
        ++body_start;
    }
    index = body_start;

    level = 1;
    close_start = 0U;
    close_end = 0U;

    while (template_str[index] != '\0')
    {
        if ((template_str[index] == '{') && ((template_str[index + 1U] == '=') || (template_str[index + 1U] == '@')))
        {
            level += 1;
            index += 2U;
            continue;
        }

        if ((template_str[index] == ':') && (template_str[index + 1U] == '}'))
        {
            level -= 1;
            if (level == 0)
            {
                close_start = index;
                close_end = index + 2U;
                break;
            }
            index += 2U;
            continue;
        }

        ++index;
    }

    if (close_end == 0U)
    {
        return ME_INVALID_TAG;
    }

    tag_name = me_substr(template_str, name_start, name_end - name_start);
    sub_template = me_substr(template_str, body_start, close_start - body_start);
    if ((tag_name == NULL) || (sub_template == NULL))
    {
        free(tag_name);
        free(sub_template);
        return ME_OUT_OF_MEMORY;
    }

    callback_text = NULL;
    registration = me_find_action_registration(me, tag_name);
    if (registration != NULL)
    {
        error = registration->callback(
            me,
            tag_name,
            sub_template,
            context,
            registration->user_data,
            &callback_text);
        if (error != ME_OK)
        {
            free(tag_name);
            free(sub_template);
            return error;
        }
        if (callback_text != NULL)
        {
            error = me_sb_append_cstr(output, callback_text);
            free(callback_text);
            free(tag_name);
            free(sub_template);
            *out_next_index = close_end;
            return error;
        }
    }

    original_text = me_substr(template_str, start, close_end - start);
    free(tag_name);
    free(sub_template);
    if (original_text == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    error = me_sb_append_cstr(output, original_text);
    free(original_text);
    *out_next_index = close_end;
    return error;
}

static ME_Error me_parse_internal(
    MightyEagle *me,
    const char *template_str,
    const ME_Context *context,
    ME_StringBuilder *output)
{
    size_t index;
    size_t next_index;
    ME_Error error;

    index = 0U;
    while (template_str[index] != '\0')
    {
        if ((template_str[index] == '{') && (template_str[index + 1U] == '='))
        {
            error = me_handle_substitution(me, template_str, index, context, output, &next_index);
            if (error != ME_OK)
            {
                return error;
            }
            index = next_index;
            continue;
        }

        if ((template_str[index] == '{') && (template_str[index + 1U] == '@'))
        {
            error = me_handle_action(me, template_str, index, context, output, &next_index);
            if (error != ME_OK)
            {
                return error;
            }
            index = next_index;
            continue;
        }

        error = me_sb_append_char(output, template_str[index]);
        if (error != ME_OK)
        {
            return error;
        }
        ++index;
    }

    return ME_OK;
}

ME_Error me_parse(
    MightyEagle *me,
    const char *template_str,
    const ME_Context *context,
    char **out_text)
{
    ME_StringBuilder builder;
    ME_Error error;
    char *text;

    if ((me == NULL) || (template_str == NULL) || (out_text == NULL))
    {
        return ME_INVALID_TAG;
    }

    *out_text = NULL;
    me_sb_init(&builder);

    error = me_parse_internal(me, template_str, context, &builder);
    if (error != ME_OK)
    {
        me_sb_free(&builder);
        return error;
    }

    text = me_sb_take(&builder);
    if (text == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }

    *out_text = text;
    return ME_OK;
}

void me_free_string(char *text)
{
    free(text);
}

const char *me_error_name(ME_Error error_code)
{
    switch (error_code)
    {
        case ME_OK:
            return "OK";
        case ME_INVALID_TAG:
            return "INVALID_TAG";
        case ME_MISSING_CLOSING_CURLY:
            return "MISSING_CLOSING_CURLY";
        case ME_MISSING_CLOSING_COLON:
            return "MISSING_CLOSING_COLON";
        case ME_TAG_CALLBACK_ERROR:
            return "TAG_CALLBACK_ERROR";
        case ME_UNREACHABLE_NOT_TRUE:
            return "UNREACHABLE_NOT_TRUE";
        case ME_OUT_OF_MEMORY:
            return "OUT_OF_MEMORY";
        default:
            return "UNKNOWN_ERROR";
    }
}
