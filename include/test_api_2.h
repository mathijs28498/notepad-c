#pragma once
#ifndef TEST_API_2_H
#define TEST_API_2_H

#include <stdint.h>

typedef struct {
    int32_t (*add)(int32_t, int32_t);
} TestApi2;

#endif // #ifndef TEST_API_2_H