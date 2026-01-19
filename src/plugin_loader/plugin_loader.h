#pragma once
#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <stdint.h>

// int32_t load_plugin(const char * const plugin_name);

typedef struct PluginApi PluginApi;
PluginApi *get_plugin_api();

#endif // #ifndef PLUGIN_LOADER_H