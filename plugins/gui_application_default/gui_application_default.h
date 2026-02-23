#pragma once

#include <stdint.h>

struct GuiApplicationInterfaceContext;
struct WindowInterfaceCreateWindowOptions;

int32_t gui_application_default_setup(struct GuiApplicationInterfaceContext *context, struct WindowInterfaceCreateWindowOptions *create_window_options);
int32_t gui_application_default_run(struct GuiApplicationInterfaceContext *context);

