/*
FILE: demo/src/fakedb.c
*/

#include "include/demo_fakedb.h"

static const DemoCustomer DEMO_CUSTOMERS[] = {
    {"xecronix", "nospam_xecronix@sogetthis.com", "407-555-1212", "123 Main Street"},
    {"jsmith", "greatday88@sogetthis.com", "407-555-1212", "999 Other Street"}
};

static const DemoOrder DEMO_ORDERS[] = {
    {"xecronix", "Laser Pointer", 3, 1.25},
    {"jsmith", "Spiral Notebook", 12, 6.87},
    {"jsmith", "Computer Mouse", 1, 9.86},
    {"jsmith", "Mouse Pad", 1, 3.49}
};

const DemoCustomer *demo_get_customers(size_t *out_count)
{
    if (out_count != NULL)
    {
        *out_count = sizeof(DEMO_CUSTOMERS) / sizeof(DEMO_CUSTOMERS[0]);
    }
    return DEMO_CUSTOMERS;
}

const DemoOrder *demo_get_orders(size_t *out_count)
{
    if (out_count != NULL)
    {
        *out_count = sizeof(DEMO_ORDERS) / sizeof(DEMO_ORDERS[0]);
    }
    return DEMO_ORDERS;
}
