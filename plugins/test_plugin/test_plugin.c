#include "test_plugin.h"

#include <plugin_impl.h>
#include <test_api.h>
#include <test_api_2.h>
#include <logger_api.h>

#include <stdint.h>
#include <stdio.h>

#define PLUGIN_API_NAME test_api
#define LOGGER_API_TAG "test api"

#define PLUGIN_DEPENDENCIES(X)           \
    X(LoggerApi, logger_api, logger_api) \
    X(TestApi2, test_api_2, test_api2)

PLUGIN_REGISTER_DEPENDENCIES(TestApiContext, PLUGIN_DEPENDENCIES);

int32_t do_something(TestApiContext *context)
{
    TestApi2 *test_api_2 = context->test_api_2;
    return test_api_2->add(test_api_2->context, 123, 343);
}

TestApi *get_api()
{
    static TestApiContext context = {0};

    static TestApi api = {
        .context = &context,

        .do_something = do_something,
    };

    return &api;
}

PLUGIN_REGISTER_API(get_api, TestApi);

int32_t init(TestApiContext *context)
{
    TestApi2 *test_api_2 = context->test_api_2;
    LOG_INF(context->logger_api, "test_plugin init: \"%d\"", test_api_2->add(test_api_2->context, 231, 452));
    return 0;
}

PLUGIN_REGISTER_INIT(init, TestApiContext);
