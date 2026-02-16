#pragma once
#ifndef APP_API_H
#define APP_API_H

#include <stdint.h>

#pragma pack(push, 8)

struct AppApiContext;

typedef struct AppApi {
    struct AppApiContext *context;

    int32_t (*run)(struct AppApiContext *context);
} AppApi;

#pragma pack(pop)

#endif // #ifndef APP_API_H