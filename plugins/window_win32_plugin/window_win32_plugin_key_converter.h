#pragma once
#ifndef WINDOW_WIN32_PLUGIN_KEY_CONVERTER_H
#define WINDOW_WIN32_PLUGIN_KEY_CONVERTER_H

#include <stdint.h>

enum WindowEventKey;

enum WindowEventKey win32_key_to_window_event_key(uintptr_t wparam);

#endif // #ifndef WINDOW_WIN32_PLUGIN_KEY_CONVERTER_H