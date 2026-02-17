#pragma once
#ifndef APPLICATION_PLUGIN_REGISTER_H
#define APPLICATION_PLUGIN_REGISTER_H

#pragma pack(push, 8)

struct LoggerApi;
struct WindowApi;
struct InputApi;

typedef struct AppApiContext
{
    struct LoggerApi *logger_api;
    struct WindowApi *window_api;
    struct InputApi *input_api;
} AppApiContext;

#pragma pack(pop)

#endif // #ifndef APPLICATION_PLUGIN_REGISTER_H