#include "input_plugin_register.h"

#include <plugin_manager_impl.h>
#include <input_api.h>

#include "input_plugin.h"

#define PLUGIN_API_NAME input_api

InputApi *get_api(void)
{
    static InputApiContext context = {0};

    static InputApi api = {
        .context = &context,
        .prepare_processing = input_plugin_prepare_processing,
        .process_window_event = input_plugin_process_window_event,
        .key_pressed = input_plugin_key_pressed,
        .key_held = input_plugin_key_held,
        .key_released = input_plugin_key_released,
    };

    return &api;
}

PLUGIN_REGISTER_API(get_api, InputApi)