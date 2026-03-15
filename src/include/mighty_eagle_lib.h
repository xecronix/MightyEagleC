/*
FILE: src/include/mighty_eagle_lib.h
*/

#ifndef MIGHTY_EAGLE_LIB_H
#define MIGHTY_EAGLE_LIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MightyEagle MightyEagle;
typedef struct ME_Context ME_Context;

typedef enum ME_Error
{
    ME_OK = 0,
    ME_INVALID_TAG,
    ME_MISSING_CLOSING_CURLY,
    ME_MISSING_CLOSING_COLON,
    ME_TAG_CALLBACK_ERROR,
    ME_UNREACHABLE_NOT_TRUE,
    ME_OUT_OF_MEMORY
} ME_Error;

typedef ME_Error (*ME_TagCallback)(
    MightyEagle *me,
    const char *tag,
    const char *tag_value,
    const ME_Context *context,
    void *user_data,
    char **out_text
);

typedef ME_Error (*ME_ActionCallback)(
    MightyEagle *me,
    const char *tag,
    const char *sub_template,
    const ME_Context *context,
    void *user_data,
    char **out_text
);

MightyEagle *me_create(void);
void me_destroy(MightyEagle *me);

ME_Context *me_context_create(void);
void me_context_destroy(ME_Context *context);
ME_Error me_context_set(ME_Context *context, const char *key, const char *value);
const char *me_context_get(const ME_Context *context, const char *key);
ME_Error me_context_clone(const ME_Context *source, ME_Context **out_clone);

ME_Error me_add_tag_cb(
    MightyEagle *me,
    const char *tag,
    ME_TagCallback callback,
    void *user_data
);

ME_Error me_add_action_cb(
    MightyEagle *me,
    const char *tag,
    ME_ActionCallback callback,
    void *user_data
);

ME_Error me_parse(
    MightyEagle *me,
    const char *template_str,
    const ME_Context *context,
    char **out_text
);

void me_free_string(char *text);
const char *me_error_name(ME_Error error_code);

#ifdef __cplusplus
}
#endif

#endif
