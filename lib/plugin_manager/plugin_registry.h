#pragma once
#ifndef PLUGIN_MANAGER_CONFIG_READER_H
#define PLUGIN_MANAGER_CONFIG_READER_H

#include <stdint.h>

struct PluginRegistry;

int32_t plugin_registry_deserialize_json(const char *json_str, struct PluginRegistry *plugin_registry);

#endif // #ifndef PLUGIN_MANAGER_CONFIG_READER_H