#include "app_plugin_register.h"

#include <app_api.h>
#include <plugin_manager_impl.h>
#include <logger_api.h>
#include <window_api.h>

#include "app_plugin.h"

// REG
#define PLUGIN_API_NAME app_api

#define PLUGIN_DEPENDENCIES(X)           \
    X(LoggerApi, logger_api, logger_api) \
    X(WindowApi, window_api, window_api)

PLUGIN_REGISTER_DEPENDENCIES(AppApiContext, PLUGIN_DEPENDENCIES);

AppApi *get_api(void)
{
    static AppApiContext context = {0};

    static AppApi api = {
        .context = &context,

        .run = app_plugin_run,
    };

    return &api;
}

PLUGIN_REGISTER_API(get_api, AppApi);