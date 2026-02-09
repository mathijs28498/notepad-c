#include "plugin_manager.h"

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cjson.h>
#include <Windows.h>
#include <assert.h>

// TODO: Change the name of this file
#include <plugin_manager_api.h>

#include <environment_api.h>
#include <environment_plugin.h>
#include <logger_api.h>
#include <logger_plugin.h>

#include "file_io.h"
#include "plugin_registry.h"
#include "plugin_manager_types.h"
#include "plugin_manager_loader.h"

PluginManagerRuntimeContext *get_plugin_manager_runtime_context()
{
    static PluginManagerRuntimeContext context = {0};
    return &context;
}

int32_t plugin_manager_init(PluginManagerSetupContext **setup_context, int argc, char **argv)
{
    assert(setup_context != NULL);

    PluginManagerSetupContext *new_setup_context = calloc(1, sizeof(**setup_context));
    if (new_setup_context == NULL)
    {
        return -1;
    }

    {
        assert(new_setup_context->internal_plugins_len < sizeof(new_setup_context->internal_plugins) / sizeof(new_setup_context->internal_plugins[0]));

        EnvironmentApi *environment_api = environment_api_get_api();
        environment_plugin_set_args(environment_api->context, argc, argv);

        PluginStatic environment_plugin = {
            .api_name = "environment_api",
            .plugin_name = "internal_environment_plugin",
            .dependencies_len = 0,
            .api = (PluginManagerBaseApi *)environment_api,
        };

        memcpy(&new_setup_context->internal_plugins[new_setup_context->internal_plugins_len],
               &environment_plugin,
               sizeof(new_setup_context->internal_plugins[new_setup_context->internal_plugins_len]));
        new_setup_context->internal_plugins_len++;
    }

    {
        assert(new_setup_context->internal_plugins_len < sizeof(new_setup_context->internal_plugins) / sizeof(new_setup_context->internal_plugins[0]));

        LoggerApi *logger_api = logger_api_get_api();
        new_setup_context->logger_api = logger_api;
        new_setup_context->logger_api->log(new_setup_context->logger_api->context, LOG_LEVEL_INFO, "plugin_manager", "This works yes? %d", 1000);

        PluginStatic logger_plugin = {
            .api_name = "logger_api",
            .plugin_name = "internal_logger_plugin",
            .dependencies_len = 0,
            .api = (PluginManagerBaseApi *)logger_api,
        };

        memcpy(&new_setup_context->internal_plugins[new_setup_context->internal_plugins_len],
               &logger_plugin,
               sizeof(new_setup_context->internal_plugins[new_setup_context->internal_plugins_len]));
        new_setup_context->internal_plugins_len++;
    }
    *setup_context = new_setup_context;
    return 0;
}

int32_t plugin_manager_add(PluginManagerSetupContext *setup_context, const char *api_name, const char *plugin_name)
{
    if (api_name == NULL)
    {
        setup_context->logger_api->log(setup_context->logger_api->context, LOG_LEVEL_ERROR, "plugin_manager", "Api name is NULL");
        return -1;
    }

    if (setup_context->requested_plugins_len >= PLUGIN_MANAGER_MAX_PLUGINS_LEN)
    {
        setup_context->logger_api->log(setup_context->logger_api->context, LOG_LEVEL_ERROR, "plugin_manager", "Cannot add plugin as max plugin_definitions is reached. Max plugin count \"%d\"", PLUGIN_MANAGER_MAX_PLUGINS_LEN);
        return -1;
    }

    snprintf(setup_context->requested_plugins[setup_context->requested_plugins_len].api_name, PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN,
             "%s", api_name);

    if (plugin_name == NULL)
    {
        setup_context->requested_plugins[setup_context->requested_plugins_len].plugin_name[0] = '\0';
    }
    else
    {
        snprintf(setup_context->requested_plugins[setup_context->requested_plugins_len].plugin_name, PLUGIN_REGISTRY_MAX_PLUGIN_NAME_LEN,
                 "%s", plugin_name);
    }

    setup_context->requested_plugins_len++;
    return 0;
};

// TODO: plugin add functionality
//          [X] - Load in plugin_definitions to add
//          [X] - Call dependency functions on plugin_definitions to add
//          [ ] - Check if dependencies are or are not included
//          [ ] - Add dependencies where not already included
//          [ ] - Repeat this until all dependencies loaded in
//          [ ] - Create dependency graph
//              [ ] - Do error checking on these dependencies (eg. cyclic dependencies)
//          [/] - Walk dependency graph layer by layer calling the init functions

int32_t plugin_manager_load(PluginManagerSetupContext *setup_context, PluginManagerRuntimeContext *runtime_context)
{
    int ret;
    char *buffer;
    PluginRegistry plugin_registry;
    (void)runtime_context;

    // TODO: Make buffer not use malloc
    ret = file_io_read("../plugin_registry.json", &buffer);
    ret = plugin_registry_deserialize_json(buffer, &plugin_registry);
    free(buffer);

    if (ret < 0)
    {
        setup_context->logger_api->log(setup_context->logger_api->context, LOG_LEVEL_ERROR, "plugin_manager", "unable to parse plugin config: %d", ret);
        return ret;
    }

    size_t plugin_modules_len = 0;
    PluginModule plugin_modules[PLUGIN_MANAGER_MAX_PLUGINS_LEN];
    // TODO: Add a way to differentiate between system and module plugins
    ret = resolve_requested_plugins(setup_context->requested_plugins, setup_context->requested_plugins_len, &plugin_registry, plugin_modules, &plugin_modules_len);

    ret = load_plugin_modules(plugin_modules, plugin_modules_len);

    // TODO: Add resolve unresolved dependencies recursively

    ret = resolve_plugin_dependencies(plugin_modules, plugin_modules_len);

    // TODO: Make own method - resolve dependencies
    // TODO: Make sure the plugin_definitions get initialized in order with their dependencies

    ret = initialize_plugins(plugin_modules, plugin_modules_len);

    // TODO: Remove this
    EnvironmentApi *env_api = (EnvironmentApi *)setup_context->internal_plugins[0].api;
    int argc;
    char **argv;
    env_api->get_args(env_api->context, &argc, &argv);

    setup_context->logger_api->log(setup_context->logger_api->context, LOG_LEVEL_INFO, "plugin_manager", "argc: %d, argv[0]: %s", argc, argv[0]);

    free(setup_context);

    return 0;
}

int32_t plugin_manager_get(const PluginManagerRuntimeContext *runtime_context, const char *api_name, void **api_interface)
{
    (void)runtime_context;
    (void)api_name;
    printf("Doing api get: %s\n", api_name);
    api_interface = NULL;
    return 0;
}
