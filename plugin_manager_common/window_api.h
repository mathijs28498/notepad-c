#pragma once
#ifndef WINDOW_API_H
#define WINDOW_API_H

#include <stdint.h>

#pragma pack(push, 8)
struct WindowApiContext;
typedef struct WindowApiCreateWindowOptions
{
    const char *window_name;
} WindowApiCreateWindowOptions;

enum WindowApiEvent
{
    WINDOW_API_EVENT_QUIT,
    WINDOW_API_EVENT_A,
};

typedef struct WindowApi
{
    struct WindowApiContext *context;

    int32_t (*create_window)(struct WindowApiContext *context, WindowApiCreateWindowOptions *options);
    int32_t (*poll_os_events)(struct WindowApiContext *context);
    int32_t (*wait_for_os_events)(struct WindowApiContext *context);
    int32_t (*pop_window_event)(struct WindowApiContext *context, enum WindowApiEvent *window_event);
} WindowApi;

#pragma pack(pop)

#endif // #ifndef WINDOW_API_H