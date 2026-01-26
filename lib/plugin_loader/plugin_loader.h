#pragma once
#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <stdint.h>

typedef struct PluginApi PluginApi;
PluginApi *get_plugin_api();

#endif // #ifndef PLUGIN_LOADER_H