#pragma once

#include <stdint.h>

struct WindowInterfaceContext;
struct WindowInterfaceCreateWindowOptions;
struct WindowEvent;

int32_t window_win32_create_window(struct WindowInterfaceContext *context, struct WindowInterfaceCreateWindowOptions *options);
int32_t window_win32_close_window(struct WindowInterfaceContext *context);

int32_t window_win32_poll_os_events(struct WindowInterfaceContext *context);
int32_t window_win32_wait_for_os_events(struct WindowInterfaceContext *context);

