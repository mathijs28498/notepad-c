#include "test_plugin_2.h"

#include <plugin_impl.h>
#include <test_api_2.h>
#include <stdio.h>
#include <stdbool.h>

#define PLUGIN_API_NAME test_api2

bool g_is_initialized = false;

int32_t add(int32_t a, int32_t b)
{
    if (!g_is_initialized)
    {
        printf("Error: not initialized yet: %d\n", g_is_initialized);
        return -1;
    }
    printf("adding: %d, %d\n", a, b);
    return a + b;
}

TestApi2 g_test_api2 = {
    .add = add,
};

PLUGIN_REGISTER_API(TestApi2, g_test_api2);

int32_t init(void)
{
    printf("test_plugin_2 init\n");
    g_is_initialized = true;
    return 0;
}

PLUGIN_REGISTER_INIT(init);
