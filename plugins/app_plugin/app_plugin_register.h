#pragma once
#ifndef APP_PLUGIN_REGISTER_H
#define APP_PLUGIN_REGISTER_H

#pragma pack(push, 8)

struct LoggerApi;
struct WindowApi;

typedef struct AppApiContext
{
    struct LoggerApi *logger_api;
    struct WindowApi *window_api;
} AppApiContext;

#pragma pack(pop)

#endif // #ifndef APP_PLUGIN_REGISTER_H