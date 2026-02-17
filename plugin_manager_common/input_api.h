#pragma once
#ifndef INPUT_API_H
#define INPUT_API_H

#include <stdint.h>
#include <stdbool.h>

#pragma pack(push, 8)

struct InputApiContext;
struct WindowEvent;
enum WindowEventKey;

typedef struct InputApi
{
    struct InputApiContext *context;

    int32_t (*prepare_processing)(struct InputApiContext *context);
    int32_t (*process_window_event)(struct InputApiContext *context, struct WindowEvent *window_event);
    bool (*input_plugin_key_just_pressed)(struct InputApiContext *context, enum WindowEventKey key);
    bool (*input_plugin_key_held)(struct InputApiContext *context, enum WindowEventKey key);
    bool (*input_plugin_key_just_released)(struct InputApiContext *context, enum WindowEventKey key);
} InputApi;

#pragma pack(pop)

#endif // #ifndef INPUT_API_H