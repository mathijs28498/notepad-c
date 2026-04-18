#include "draw_default_register.h"

#include <plugin_sdk/draw_interface.h>

#include "draw_default.h"

static const DrawVtable plugin_vtable = {
    .start = draw_default_start,
    .present = draw_default_present,
    .on_window_resize = draw_default_on_window_resize,
};

#include "plugin_register.c.inc"