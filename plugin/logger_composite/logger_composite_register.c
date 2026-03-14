#include "logger_composite_register.h"

#include <logger_interface.h>

#include "logger_composite.h"

static LoggerInterface *get_interface(void)
{
    static LoggerInterfaceContext context = {0};

    static LoggerInterface iface = {
        .context = &context,
    };

    return &iface;
}

#include "plugin_register.c.inc"