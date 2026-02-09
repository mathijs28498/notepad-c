#pragma once
#ifndef LOGGER_PLUGIN_H
#define LOGGER_PLUGIN_H

enum LoggerApiLogLevel;

typedef struct LoggerApiContext
{
    enum LoggerApiLogLevel log_level;
}
LoggerApiContext;

struct LoggerApi;
struct LoggerApi *logger_api_get_api(void);

#endif // #ifndef LOGGER_PLUGIN_H