#include "test_plugin_2.h"

#include <stdbool.h>

#include <logger_api.h>
#include <plugin_impl.h>
#include <test_api_2.h>

#define PLUGIN_API_NAME test_api2
#define LOGGER_API_TAG "test_api_2"

#define PLUGIN_DEPENDENCIES(X) \
    X(LoggerApi, logger_api, logger_api)

PLUGIN_REGISTER_DEPENDENCIES(TestApi2Context, PLUGIN_DEPENDENCIES);

int32_t add(TestApi2Context *context, int32_t a, int32_t b)
{
    if (!context->is_initialized)
    {
        LOG_ERR(context->logger_api, "not initialized yet: %d", context->is_initialized);
        return -1;
    }
    LOG_INF(context->logger_api, "adding: %d, %d", a, b);
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
    LOG_INF(context->logger_api, "test_plugin_2 init");
    return 0;
}

PLUGIN_REGISTER_INIT(init, TestApi2Context);
