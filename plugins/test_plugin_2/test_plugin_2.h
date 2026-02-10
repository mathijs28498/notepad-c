#pragma once
#ifndef TEST_PLUGIN_2_H
#define TEST_PLUGIN_2_H

#include <stdbool.h>

#pragma pack(push, 8)

struct LoggerApi;

typedef struct TestApi2Context
{
    struct LoggerApi *logger_api;

    bool is_initialized;
} TestApi2Context;

#pragma pack(pop)

#endif // #ifndef TEST_PLUGIN_2_H