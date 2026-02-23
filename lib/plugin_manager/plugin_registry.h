#pragma once

#include <stdint.h>

struct LoggerInterface;
struct PluginModuleRegistry;

int32_t plugin_registry_deserialize_json(struct LoggerInterface *logger, const char *json_str, struct PluginModuleRegistry *plugin_registry);