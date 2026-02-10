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

#define LOG(logger_api, log_level, ...) (logger_api->log(logger_api->context, log_level, LOGGER_API_TAG, __VA_ARGS__))
#define LOG_ERR(logger_api, ...) LOG(logger_api, LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WRN(logger_api, ...) LOG(logger_api, LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_INF(logger_api, ...) LOG(logger_api, LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DBG(logger_api, ...) LOG(logger_api, LOG_LEVEL_DEBUG, __VA_ARGS__)

#endif // #ifndef LOGGER_API_H