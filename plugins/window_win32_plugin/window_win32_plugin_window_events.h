#pragma once
#ifndef WINDOW_WIN32_PLUGIN_WINDOW_EVENTS_H
#define WINDOW_WIN32_PLUGIN_WINDOW_EVENTS_H

#include <stdbool.h>

struct WindowApiContext;
struct WindowEvent;

bool window_win32_plugin_window_events_pop(struct WindowApiContext *context, struct WindowEvent *window_event);
void window_win32_plugin_window_events_push(struct WindowApiContext *context, struct WindowEvent const *window_event);

#endif // #ifndef WINDOW_WIN32_PLUGIN_WINDOW_EVENTS_H