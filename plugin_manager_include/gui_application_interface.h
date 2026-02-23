#pragma once

#include <stdint.h>

#pragma pack(push, 8)

struct GuiApplicationInterfaceContext;
struct WindowInterfaceCreateWindowOptions;

typedef struct GuiApplicationInterface {
    struct GuiApplicationInterfaceContext *context;

    int32_t (*setup)(struct GuiApplicationInterfaceContext *context, struct WindowInterfaceCreateWindowOptions *create_window_options);
    int32_t (*run)(struct GuiApplicationInterfaceContext *context);
} GuiApplicationInterface;

#pragma pack(pop)

