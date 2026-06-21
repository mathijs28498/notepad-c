#include "time_win32_register.h"

#include <plugin_sdk/time/v1/time_interface.h>
#include <plugin_sdk/plugin_utils.h>

#include "time_win32.h"

static const TimeVtable plugin_vtable = {
    .get_string = time_win32_get_string,
    .ms = time_win32_ms,
    .ns = time_win32_ns,
    .get_elapsed_ms = time_win32_get_elapsed_ms,
};

#include "plugin_register.c.inc"