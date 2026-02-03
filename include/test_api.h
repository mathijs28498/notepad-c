#pragma once
#ifndef TEST_API_H

#include <stdint.h>

typedef struct
{
    int32_t (*do_something)(void);
} TestApi;

#endif // #ifndef TEST_API_H