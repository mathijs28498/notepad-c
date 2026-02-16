#pragma once
#ifndef APP_PLUGIN__H
#define APP_PLUGIN__H

#include <stdint.h>

struct AppApiContext;

int32_t app_plugin_run(struct AppApiContext *context);

#endif // #ifndef APP_PLUGIN__H