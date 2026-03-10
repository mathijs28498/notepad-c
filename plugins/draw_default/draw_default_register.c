#include "draw_default_register.h"

#include <stdint.h>

#include <logger_interface.h>
#include <draw_interface.h>

#include "draw_default.h"

static DrawInterface *get_interface()
{
    static DrawInterfaceContext context = {0};

    static DrawInterface iface = {
        .context = &context,

        .present = draw_default_present,
    };

    return &iface;
}

#include "plugin_register.c.inc"