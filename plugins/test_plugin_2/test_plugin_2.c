#include "test_plugin_2.h"

#include <plugin_impl.h>
#include <test_api_2.h>
#include <stdio.h>
#include <stdbool.h>

#define PLUGIN_API_NAME test_api2

int32_t add(TestApi2Context *context, int32_t a, int32_t b)
{
    if (!context->is_initialized)
    {
        printf("Error: not initialized yet: %d\n", context->is_initialized);
        return -1;
    }
    printf("adding: %d, %d\n", a, b);
    return a + b;
}

TestApi2 *get_api()
{
    static TestApi2Context context = {0};
    static TestApi2 api = {
        .context = &context,

        .add = add,
    };

    return &api;
}

PLUGIN_REGISTER_API(get_api, TestApi2);

int32_t init(TestApi2Context *context)
{
    context->is_initialized = true;
    printf("test_plugin_2 init\n");
    return 0;
}

PLUGIN_REGISTER_INIT(init, TestApi2Context);
