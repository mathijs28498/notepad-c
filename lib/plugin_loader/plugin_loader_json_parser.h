#pragma once
#ifndef PLUGIN_LOADER_JSON_PARSER_H

#include <stdint.h>

// TODO: Make this into cmake variables
#define PLUGIN_REGISTRY_MAX_PLUGIN_LEN 64
#define PLUGIN_REGISTRY_MAX_PLUGIN_NAME_LEN 64
#define PLUGIN_REGISTRY_MAX_PLUGIN_PATH_LEN 512
#define PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN 64


typedef struct {
    char name[PLUGIN_REGISTRY_MAX_PLUGIN_NAME_LEN];
    char path[PLUGIN_REGISTRY_MAX_PLUGIN_PATH_LEN];
    char api[PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN];
} PluginDefinition;

typedef struct {
    uint32_t plugin_definitions_len;
    PluginDefinition plugin_definitions[PLUGIN_REGISTRY_MAX_PLUGIN_LEN];
} PluginRegistry;

int plugin_loader_parse_config(const char *json_str, PluginRegistry *plugin_registry);

#endif // #ifndef PLUGIN_LOADER_JSON_PARSER_H