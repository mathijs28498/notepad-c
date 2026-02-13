#include "test_plugin_2.h"

#include <stdbool.h>

#include <logger_api.h>
LOGGER_API_REGISTER(test_api_2, LOG_LEVEL_DEBUG)

#include <plugin_impl.h>
#include <test_api_2.h>
#include <plugin_manager_common.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define PLUGIN_API_NAME test_api2

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
    TODO("Need to make this actually work");
    const char *new_colors[] = {ANSI_COLOR_RED, ANSI_COLOR_RED, ANSI_COLOR_RED, ANSI_COLOR_RED};
    context->logger_api->set_colors(context->logger_api->context, new_colors);
    context->is_initialized = true;
    LOG_DBG(context->logger_api, "test_plugin_2 init");
    LOG_INF(context->logger_api, "test_plugin_2 init");
    LOG_WRN(context->logger_api, "test_plugin_2 init");
    LOG_ERR(context->logger_api, "test_plugin_2 init");
    return 0;
}

PLUGIN_REGISTER_INIT(init, TestApi2Context);
