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

static inline int32_t gui_application_setup(GuiApplicationInterface *iface, struct WindowInterfaceCreateWindowOptions *create_window_options)
{
    return iface->setup(iface->context, create_window_options);
}

static inline int32_t gui_application_run(GuiApplicationInterface *iface)
{
    return iface->run(iface->context);
}

