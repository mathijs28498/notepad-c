#include "renderer_vulkan_register.h"

#include <renderer_interface.h>

#include "renderer_vulkan.h"

static RendererInterface *get_interface(void)
{
    static RendererInterfaceContext context = {0};

    static RendererInterface iface = {
        .context = &context,

        .init = renderer_vulkan_init,
    };

    return &iface;
}

#include "plugin_register.c.inc"