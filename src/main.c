#include <plugin_manager_api.h>
#include <logger_api.h>
LOGGER_API_REGISTER(main, LOG_LEVEL_DEBUG)
#include <environment_api.h>
#include <window_api.h>
#include <application_api.h>

#include <stdio.h>
#include <stdbool.h>

PLUGIN_MANAGER_API_MAIN()
{
    int32_t ret;

    // ret = PLUGIN_MANAGER_API_ADD("window_api", NULL);
    ret = PLUGIN_MANAGER_API_ADD("application_api", NULL);
    ret = PLUGIN_MANAGER_API_LOAD();
    if (ret < 0)
    {
        return -1;
    }

    WindowApi *window_api;
    AppApi *application_api;
    PLUGIN_MANAGER_API_GET("window_api", &window_api);
    PLUGIN_MANAGER_API_GET("application_api", &application_api);

    WindowApiCreateWindowOptions create_window_options = {
        .window_name = "My app",
    };
    ret = window_api->create_window(window_api->context, &create_window_options);

    ret = application_api->run(application_api->context);

    CoUninitialize();
    return 0;
}