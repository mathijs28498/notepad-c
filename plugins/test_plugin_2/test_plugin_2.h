#pragma once
#ifndef TEST_PLUGIN_2_H
#define TEST_PLUGIN_2_H

#include <stdbool.h>

#pragma pack(push, 8)

typedef struct TestApi2Context
{
    bool is_initialized;
} TestApi2Context;

#pragma pack(pop)

#endif // #ifndef TEST_PLUGIN_2_H