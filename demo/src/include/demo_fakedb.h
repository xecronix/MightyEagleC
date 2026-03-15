/*
FILE: demo/src/include/demo_fakedb.h
*/

#ifndef DEMO_FAKEDB_H
#define DEMO_FAKEDB_H

#include <stddef.h>

typedef struct DemoCustomer
{
    const char *name;
    const char *email;
    const char *phone;
    const char *address;
} DemoCustomer;

typedef struct DemoOrder
{
    const char *customer_name;
    const char *product;
    int quantity;
    double cost;
} DemoOrder;

const DemoCustomer *demo_get_customers(size_t *out_count);
const DemoOrder *demo_get_orders(size_t *out_count);

#endif
