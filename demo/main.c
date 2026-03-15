/*
FILE: demo/main.c
*/

#include "../src/include/mighty_eagle_lib.h"
#include "src/include/demo_fakedb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DemoApp
{
    MightyEagle *me;
} DemoApp;

static ME_Error customer_cb(
    MightyEagle *me,
    const char *tag,
    const char *sub_template,
    const ME_Context *context,
    void *user_data,
    char **out_text)
{
    const DemoCustomer *customers;
    size_t customer_count;
    size_t index;
    ME_Context *tags;
    ME_Error error;
    char *rendered;
    char *combined;
    size_t total_length;
    size_t used;

    (void)tag;
    (void)context;
    (void)user_data;

    customers = demo_get_customers(&customer_count);
    total_length = 1U;
    combined = (char *)malloc(total_length);
    if (combined == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }
    combined[0] = '\0';

    for (index = 0U; index < customer_count; ++index)
    {
        tags = me_context_create();
        if (tags == NULL)
        {
            free(combined);
            return ME_OUT_OF_MEMORY;
        }

        error = me_context_set(tags, "name", customers[index].name);
        if (error == ME_OK) error = me_context_set(tags, "email", customers[index].email);
        if (error == ME_OK) error = me_context_set(tags, "phone", customers[index].phone);
        if (error == ME_OK) error = me_context_set(tags, "address", customers[index].address);
        if (error != ME_OK)
        {
            me_context_destroy(tags);
            free(combined);
            return error;
        }

        error = me_parse(me, sub_template, tags, &rendered);
        me_context_destroy(tags);
        if (error != ME_OK)
        {
            free(combined);
            return error;
        }

        used = strlen(combined);
        total_length = used + strlen(rendered) + 1U;
        {
            char *next = (char *)realloc(combined, total_length);
            if (next == NULL)
            {
                me_free_string(rendered);
                free(combined);
                return ME_OUT_OF_MEMORY;
            }
            combined = next;
        }
        memcpy(combined + used, rendered, strlen(rendered) + 1U);
        me_free_string(rendered);
    }

    *out_text = combined;
    return ME_OK;
}

static ME_Error order_cb(
    MightyEagle *me,
    const char *tag,
    const char *sub_template,
    const ME_Context *context,
    void *user_data,
    char **out_text)
{
    const DemoOrder *orders;
    size_t order_count;
    size_t index;
    const char *customer_name;
    ME_Context *tags;
    ME_Error error;
    char quantity_text[32];
    char cost_text[32];
    char *rendered;
    char *combined;
    size_t total_length;
    size_t used;

    (void)tag;
    (void)user_data;

    orders = demo_get_orders(&order_count);
    customer_name = me_context_get(context, "name");

    total_length = 1U;
    combined = (char *)malloc(total_length);
    if (combined == NULL)
    {
        return ME_OUT_OF_MEMORY;
    }
    combined[0] = '\0';

    for (index = 0U; index < order_count; ++index)
    {
        if ((customer_name == NULL) || (strcmp(orders[index].customer_name, customer_name) != 0))
        {
            continue;
        }

        tags = me_context_create();
        if (tags == NULL)
        {
            free(combined);
            return ME_OUT_OF_MEMORY;
        }

        snprintf(quantity_text, sizeof(quantity_text), "%d", orders[index].quantity);
        snprintf(cost_text, sizeof(cost_text), "%.2f", orders[index].cost);

        error = me_context_set(tags, "prod", orders[index].product);
        if (error == ME_OK) error = me_context_set(tags, "qty", quantity_text);
        if (error == ME_OK) error = me_context_set(tags, "cost", cost_text);
        if (error != ME_OK)
        {
            me_context_destroy(tags);
            free(combined);
            return error;
        }

        error = me_parse(me, sub_template, tags, &rendered);
        me_context_destroy(tags);
        if (error != ME_OK)
        {
            free(combined);
            return error;
        }

        used = strlen(combined);
        total_length = used + strlen(rendered) + 1U;
        {
            char *next = (char *)realloc(combined, total_length);
            if (next == NULL)
            {
                me_free_string(rendered);
                free(combined);
                return ME_OUT_OF_MEMORY;
            }
            combined = next;
        }
        memcpy(combined + used, rendered, strlen(rendered) + 1U);
        me_free_string(rendered);
    }

    *out_text = combined;
    return ME_OK;
}

int main(void)
{
    /*
    FILE: demo/main.c
    */

    static const char *template_text =
        "<html>\n"
        "<style>\n"
        "\ttd {border:1px solid}\n"
        "\ttable {border:1px solid}\n"
        "\t.center {text-align:center}\n"
        "\t.right_align {text-align:right}\n"
        "</style>\n"
        "<body>\n"
        "<table>\n"
        "{@customer_cb\n"
        "  <tr>\n"
        "    <th colspan='3'>\n"
        "      {=name :}<br />\n"
        "      {=address :}<br />\n"
        "      {=phone :}<br />\n"
        "      {=email :}<br />\n"
        "    </th>\n"
        "  </tr>\n"
        "  <tr>\n"
        "      <th>Product</th>\n"
        "      <th>Quantity</th>\n"
        "      <th>Cost</th>\n"
        "    </th>\n"
        "  </tr>\n"
        "{@order_cb\n"
        "  <tr>\n"
        "    <td class='center'>{=prod :}</td>\n"
        "    <td class='right_align'>{=qty :}</td>\n"
        "    <td class='right_align'>{=cost :}</td>\n"
        "  </tr>\n"
        ":}:}\n"
        "</table>\n"
        "</body>\n"
        "</html>\n";

    MightyEagle *me;
    ME_Context *context;
    ME_Error error;
    char *html;

    me = me_create();
    context = me_context_create();
    if ((me == NULL) || (context == NULL))
    {
        fprintf(stderr, "Failed to initialize demo.\n");
        me_context_destroy(context);
        me_destroy(me);
        return 1;
    }

    error = me_add_action_cb(me, "customer_cb", customer_cb, NULL);
    if (error == ME_OK)
    {
        error = me_add_action_cb(me, "order_cb", order_cb, NULL);
    }
    if (error == ME_OK)
    {
        error = me_parse(me, template_text, context, &html);
    }

    if (error != ME_OK)
    {
        fprintf(stderr, "Demo failed: %s\n", me_error_name(error));
        me_context_destroy(context);
        me_destroy(me);
        return 1;
    }

    printf("%s", html);

    me_free_string(html);
    me_context_destroy(context);
    me_destroy(me);
    return 0;
}
