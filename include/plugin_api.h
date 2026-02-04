#pragma once
#ifndef PLUGIN_API_H
#define PLUGIN_API_H

// TODO: Add static linking functionality

#include <stdint.h>

typedef struct PluginLoaderContext PluginLoaderContext;

#ifndef PLUGIN_LOADER_STATIC_LINKING
// TODO: Review this name
typedef struct PluginApi
{
    PluginLoaderContext *context;

    int32_t (*add)(PluginLoaderContext *context, const char *api_name, const char *plugin_name);
    int32_t (*load)(PluginLoaderContext *context);
    int32_t (*get)(PluginLoaderContext *context, const char *api_name, const void **out_api_interface);
} PluginApi;

PluginApi *get_plugin_api();

#endif // #ifndef STATIC_LINKING

#ifdef PLUGIN_LOADER_STATIC_LINKING
#define PLUGIN_API_ADD(api_name, plugin_name)
#else
#define PLUGIN_API_ADD(api_name, plugin_name) (get_plugin_api()->add(get_plugin_api()->context, api_name, plugin_name))
#endif // #ifdef PLUGIN_LOADER_STATIC_LINKING

#ifdef PLUGIN_LOADER_STATIC_LINKING
#define PLUGIN_API_LOAD()
#else
#define PLUGIN_API_LOAD() (get_plugin_api()->load(get_plugin_api()->context))
#endif // #ifdef PLUGIN_LOADER_STATIC_LINKING

#ifdef PLUGIN_LOADER_STATIC_LINKING
#define PLUGIN_API_GET(api_name, out_api_interface)
#else
#define PLUGIN_API_GET(api_name, out_api_interface) (get_plugin_api()->get(get_plugin_api()->context, api_name, out_api_interface))
#endif // #ifdef PLUGIN_LOADER_STATIC_LINKING

#endif // #ifndef PLUGIN_API_H