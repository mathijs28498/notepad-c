#include "app_plugin.h"

#include <stdint.h>

#include <window_api.h>
#include <logger_api.h>
LOGGER_API_REGISTER(app_plugin, LOG_LEVEL_DEBUG)
#include <plugin_manager_common.h>

#include "app_plugin_register.h"

int32_t app_plugin_run(AppApiContext *context)
{
    WindowApi *window_api = context->window_api;
    LoggerApi *logger_api = context->logger_api;

    bool app_running = true;
    while (app_running)
    {
        window_api->poll_os_events(window_api->context);
        WindowEvent window_event;
        while (window_api->pop_window_event(window_api->context, &window_event))
        {
            switch (window_event.type)
            {
            case WINDOW_API_EVENT_QUIT:
                app_running = false;
                break;
            case WINDOW_API_EVENT_KEY_PRESS:
                break;
            case WINDOW_API_EVENT_MOUSE_MOVE:
                break;
            case WINDOW_API_EVENT_MOUSE_PRESS:
                break;
            case WINDOW_API_EVENT_MOUSE_SCROLL:
                break;
            }
        }
        if (app_running)
        {
            TODO("Do rendering stuff")
        }
    }

    return 0;
}
