#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "window_interface_window_event.h"

#define WINDOW_INTERFACE_MAX_WINDOW_NAME_LEN 256

#pragma pack(push, 8)
struct WindowInterfaceContext;
typedef struct WindowInterfaceCreateWindowOptions
{
    const char window_name[WINDOW_INTERFACE_MAX_WINDOW_NAME_LEN];
} WindowInterfaceCreateWindowOptions;

typedef struct WindowInterface
{
    struct WindowInterfaceContext *context;

    int32_t (*create_window)(struct WindowInterfaceContext *context, WindowInterfaceCreateWindowOptions *options);
    int32_t (*close_window)(struct WindowInterfaceContext *context);
    int32_t (*poll_os_events)(struct WindowInterfaceContext *context);
    int32_t (*wait_for_os_events)(struct WindowInterfaceContext *context);
    bool (*pop_window_event)(struct WindowInterfaceContext *context, WindowEvent *window_event);
} WindowInterface;

#pragma pack(pop)

static inline int32_t window_create_window(WindowInterface *iface, WindowInterfaceCreateWindowOptions *options)
{
    return iface->create_window(iface->context, options);
}

static inline int32_t window_close_window(WindowInterface *iface)
{
    return iface->close_window(iface->context);
}

static inline int32_t window_poll_os_events(WindowInterface *iface)
{
    return iface->poll_os_events(iface->context);
}

static inline int32_t window_wait_for_os_events(WindowInterface *iface)
{
    return iface->wait_for_os_events(iface->context);
}

static inline bool window_pop_window_event(WindowInterface *iface, WindowEvent *window_event)
{
    return iface->pop_window_event(iface->context, window_event);
}
