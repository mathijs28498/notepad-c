#pragma once
#ifndef WINDOW_API_H
#define WINDOW_API_H

#include <stdint.h>

#pragma pack(push, 8)
struct WindowApiContext;

typedef struct WindowApi
{
    struct WindowApiContext *context;

    int32_t (*create_window)();
} WindowApi;

#pragma pack(pop)

#endif // #ifndef WINDOW_API_H