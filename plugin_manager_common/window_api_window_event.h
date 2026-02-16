#pragma once
#ifndef WINDOW_API_WINDOW_EVENT_H
#define WINDOW_API_WINDOW_EVENT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum WindowEventType
{
    WINDOW_API_EVENT_QUIT,
    WINDOW_API_EVENT_KEY_PRESS,
    WINDOW_API_EVENT_MOUSE_PRESS,
    WINDOW_API_EVENT_MOUSE_MOVE,
    WINDOW_API_EVENT_MOUSE_SCROLL,
} WindowEventType;

typedef struct WindowEvent
{
    WindowEventType type;
    union
    {
        struct
        {
            uint32_t key;
            bool is_pressed;
        } key_press;
    } data;
} WindowEvent;

#endif // #ifndef WINDOW_API_WINDOW_EVENT_H