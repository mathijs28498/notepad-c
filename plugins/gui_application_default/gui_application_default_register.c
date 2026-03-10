#include "gui_application_default_register.h"

#include <gui_application_interface.h>
#include <logger_interface.h>
#include <window_interface.h>
#include <input_interface.h>
#include <draw_interface.h>

#include "gui_application_default.h"

static GuiApplicationInterface *get_interface(void)
{
    static GuiApplicationInterfaceContext context = {0};

    static GuiApplicationInterface iface = {
        .context = &context,

        .setup = gui_application_default_setup,
        .run = gui_application_default_run,
    };

    return &iface;
}

#include "plugin_register.c.inc"