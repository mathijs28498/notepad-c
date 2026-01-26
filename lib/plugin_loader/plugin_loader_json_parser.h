#pragma once
#ifndef PLUGIN_LOADER_JSON_PARSER_H

#include <stdint.h>

// TODO: Make this into cmake variables
#define PLUGIN_CONFIG_MAX_PLUGIN_COUNT 64
#define PLUGIN_CONFIG_MAX_PLUGIN_NAME_COUNT 64
#define PLUGIN_CONFIG_MAX_PLUGIN_PATH_COUNT 512
#define PLUGIN_CONFIG_MAX_PLUGIN_API_NAME_COUNT 64


typedef struct {
    char name[PLUGIN_CONFIG_MAX_PLUGIN_NAME_COUNT];
    char path[PLUGIN_CONFIG_MAX_PLUGIN_PATH_COUNT];
    char implements[PLUGIN_CONFIG_MAX_PLUGIN_API_NAME_COUNT];
} PluginInfo;

typedef struct {
    uint32_t plugins_count;
    PluginInfo plugins[PLUGIN_CONFIG_MAX_PLUGIN_COUNT];
} PluginConfig;

int plugin_loader_parse_config(const char *json_str, PluginConfig *config);

#endif // #ifndef PLUGIN_LOADER_JSON_PARSER_H