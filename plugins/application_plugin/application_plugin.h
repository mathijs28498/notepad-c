#pragma once
#ifndef APPLICATION_PLUGIN__H
#define APPLICATION_PLUGIN__H

#include <stdint.h>

struct AppApiContext;

int32_t application_plugin_run(struct AppApiContext *context);

#endif // #ifndef APPLICATION_PLUGIN__H