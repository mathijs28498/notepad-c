#pragma once
#ifndef PLUGIN_MANAGER_LOADER_H
#define PLUGIN_MANAGER_LOADER_H

#include <stdint.h>
#include <stddef.h>

struct RequestedPlugin;
struct PluginRegistry;
struct PluginModule;

int32_t resolve_requested_plugins(
    const struct RequestedPlugin *requested_plugins,
    size_t requested_plugins_len,
    const struct PluginRegistry *plugin_registry,
    struct PluginModule *plugin_modules,
    size_t *plugin_modules_len);
int32_t load_plugin_modules(struct PluginModule *plugin_modules, size_t plugin_modules_len);
int32_t resolve_plugin_dependencies(struct PluginModule *plugin_modules, size_t plugin_modules_len);
int32_t initialize_plugins(struct PluginModule *plugin_modules, size_t plugin_modules_len);

#endif // #ifndef PLUGIN_MANAGER_LOADER_H