#include "application_plugin.h"

#include <stdint.h>

#include <window_api.h>
#include <logger_api.h>
LOGGER_API_REGISTER(application_plugin, LOG_LEVEL_DEBUG)
#include <input_api.h>
#include <plugin_manager_common.h>

#include "application_plugin_register.h"

int32_t application_plugin_run(AppApiContext *context)
{
    WindowApi *window_api = context->window_api;
    LoggerApi *logger_api = context->logger_api;
    InputApi *input_api = context->input_api;
    LOG_INF(logger_api, "Starting main loop");

    bool application_running = true;
    while (application_running)
    {
        window_api->poll_os_events(window_api->context);
        WindowEvent window_event;

        input_api->prepare_processing(input_api->context);
        while (window_api->pop_window_event(window_api->context, &window_event))
        {
            switch (window_event.type)
            {
            case WINDOW_EVENT_TYPE_QUIT:
                application_running = false;
                break;
            case WINDOW_EVENT_TYPE_KEY_PRESS:
            case WINDOW_EVENT_TYPE_MOUSE_MOVE:
            case WINDOW_EVENT_TYPE_MOUSE_PRESS:
            case WINDOW_EVENT_TYPE_MOUSE_SCROLL:
                input_api->process_window_event(input_api->context, &window_event);
                break;
            }
        }
        if (application_running)
        {
            if (input_api->input_plugin_key_just_pressed(input_api->context, WINDOW_EVENT_KEY_A))
            {
                LOG_ERR(logger_api, "A just pressed");
            }
            if (input_api->input_plugin_key_held(input_api->context, WINDOW_EVENT_KEY_A))
            {
                LOG_WRN(logger_api, "A held");
            }
            if (input_api->input_plugin_key_just_pressed(input_api->context, WINDOW_EVENT_KEY_B))
            {
                LOG_ERR(logger_api, "A just pressed");
            }
            if (input_api->input_plugin_key_held(input_api->context, WINDOW_EVENT_KEY_B))
            {
                LOG_WRN(logger_api, "A held");
            }
            TODO("Do rendering stuff")
        }
    }

    return 0;
}
