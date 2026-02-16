#include <plugin_manager_api.h>
#include <logger_api.h>
LOGGER_API_REGISTER(main, LOG_LEVEL_DEBUG)
#include <environment_api.h>
#include <window_api.h>
#include <app_api.h>

#include <stdio.h>
#include <stdbool.h>

PLUGIN_MANAGER_API_MAIN()
{
    int32_t ret;

    // ret = PLUGIN_MANAGER_API_ADD("window_api", NULL);
    ret = PLUGIN_MANAGER_API_ADD("app_api", NULL);
    ret = PLUGIN_MANAGER_API_LOAD();
    if (ret < 0)
    {
        return -1;
    }

    WindowApi *window_api;
    AppApi *app_api;
    PLUGIN_MANAGER_API_GET("window_api", &window_api);
    PLUGIN_MANAGER_API_GET("app_api", &app_api);

    WindowApiCreateWindowOptions create_window_options = {
        .window_name = "My app",
    };
    ret = window_api->create_window(window_api->context, &create_window_options);

    ret = app_api->run(app_api->context);

    CoUninitialize();
    return 0;
}