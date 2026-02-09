#pragma once
#ifndef LOGGER_API_H
#define LOGGER_API_H

struct LoggerApiContext;

typedef enum LoggerApiLogLevel
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} LoggerApiLogLevel;

#pragma pack(push, 8)

typedef struct LoggerApi
{
    struct LoggerApiContext *context;

    void (*log)(const struct LoggerApiContext *context, LoggerApiLogLevel log_level, const char *tag, const char *message, ...);
    void (*set_level)(struct LoggerApiContext *context, LoggerApiLogLevel log_level);

} LoggerApi;

#pragma pack(pop)

#endif // #ifndef LOGGER_API_H