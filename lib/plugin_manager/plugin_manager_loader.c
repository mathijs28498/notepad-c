#include "plugin_manager_loader.h"

#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>
#include <stdio.h>

#include "plugin_manager_types.h"

// TODO: Check if this algorithm can/should be made better/faster
int32_t resolve_requested_plugins(
    const RequestedPlugin *requested_plugins,
    size_t requested_plugins_len,
    const PluginRegistry *plugin_registry,
    PluginModule *plugin_modules,
    size_t *plugin_modules_len)
{
    for (size_t i = 0; i < requested_plugins_len; i++)
    {
        const RequestedPlugin *requested_plugin = &requested_plugins[i];

        bool use_default = strlen(requested_plugin->plugin_name) == 0;

        int32_t plugin_info_index = -1;
        for (uint32_t j = 0; j < plugin_registry->plugin_definitions_len; j++)
        {
            const PluginDefinition *plugin_definition = &plugin_registry->plugin_definitions[j];

            const char *plugin_info_name = use_default
                                               ? plugin_definition->api
                                               : plugin_definition->name;

            const char *plugin_to_add_name = use_default
                                                 ? requested_plugin->api_name
                                                 : requested_plugin->plugin_name;

            if (strcmp(plugin_info_name, plugin_to_add_name) == 0)
            {
                plugin_info_index = (int32_t)j;
                break;
            }
        }

        if (plugin_info_index < 0)
        {
            printf("Error: couldn't add plugin api \"%s\"\n", requested_plugin->api_name);
            return -1;
        }
        plugin_modules[*plugin_modules_len].plugin_definition = &plugin_registry->plugin_definitions[plugin_info_index];
        plugin_modules[*plugin_modules_len].dependencies_len = 0;
        (*plugin_modules_len)++;
        // TODO: Add max index check here
    }

    return 0;
}

int32_t load_plugin_modules(PluginModule *plugin_modules, size_t plugin_modules_len)
{
    for (size_t i = 0; i < plugin_modules_len; i++)
    {
        PluginModule *plugin_module = &plugin_modules[i];

        plugin_module->handle = LoadLibrary(plugin_module->plugin_definition->path);
        if (!plugin_module->handle)
        {
            printf("Error: Failed to load plugin \"%s\" at \"%s\"\n", plugin_module->plugin_definition->name, plugin_module->plugin_definition->path);
            continue;
        }

        static const char get_dependencies_postfix[] = "_get_dependencies";
        char get_dependencies_function_name[PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN + sizeof(get_dependencies_postfix)];

        snprintf(get_dependencies_function_name, sizeof(get_dependencies_function_name),
                 "%s%s", plugin_module->plugin_definition->api, get_dependencies_postfix);

        FARPROC get_dependencies_proc = GetProcAddress(plugin_module->handle, get_dependencies_function_name);
        if (get_dependencies_proc)
        {
            // TODO: Make these functions casted to a function signature typedef
            get_dependencies_proc(&plugin_module->dependencies, &plugin_module->dependencies_len);
        }

        static const char get_api_postfix[] = "_get_api";
        char get_api_function_name[PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN + sizeof(get_api_postfix)];

        snprintf(get_api_function_name, sizeof(get_api_function_name),
                 "%s%s", plugin_module->plugin_definition->api, get_api_postfix);

        FARPROC get_api_proc = GetProcAddress(plugin_module->handle, get_api_function_name);
        if (get_api_proc)
        {
            // TODO: Make these functions casted to a function signature typedef
            plugin_module->api = (PluginManagerBaseApi *)get_api_proc();
        }
        else
        {
            printf("Error: no api found for plugin: %s\n", plugin_module->plugin_definition->name);
        }
    }

    return 0;
}

int32_t resolve_plugin_dependencies(PluginModule *plugin_modules, size_t plugin_modules_len)
{
    for (size_t i = 0; i < plugin_modules_len; i++)
    {
        PluginModule *plugin_module = &plugin_modules[i];

        for (uint32_t j = 0; j < plugin_module->dependencies_len; j++)
        {
            // TODO: Handle when dependency is not found (add it from the registry if it is available)
            char *dependency = plugin_module->dependencies[j];
            for (uint32_t k = 0; k < plugin_modules_len; k++)
            {
                PluginModule *dep_plugin_module = &plugin_modules[k];
                if (strcmp(dep_plugin_module->plugin_definition->api, dependency) == 0)
                {

                    static const char set_dependency_midfix[] = "_set_";
                    // TODO: Fix the sizing here
                    char set_dependency_function_name[PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN + sizeof(set_dependency_midfix)];

                    snprintf(set_dependency_function_name, sizeof(set_dependency_function_name),
                             "%s%s%s", plugin_module->plugin_definition->api, set_dependency_midfix, dependency);

                    FARPROC set_dependency_proc = GetProcAddress(plugin_module->handle, set_dependency_function_name);
                    if (set_dependency_proc)
                    {
                        // TODO: Make these functions casted to a function signature typedef
                        set_dependency_proc(plugin_module->api->context, dep_plugin_module->api);
                    }
                    else
                    {
                        printf("Error: %s() not found\n", set_dependency_function_name);
                    }
                }
            }
        }
    }

    return 0;
}

int32_t initialize_plugins(PluginModule *plugin_modules, size_t plugin_modules_len)
{
    for (size_t i = 0; i < plugin_modules_len; i++)
    {
        PluginModule *plugin_module = &plugin_modules[i];
        static const char init_postfix[] = "_init";
        // TODO: Fix the sizing here
        char init_function_name[PLUGIN_REGISTRY_MAX_PLUGIN_API_NAME_LEN + sizeof(init_postfix)];
        snprintf(init_function_name, sizeof(init_function_name),
                 "%s%s", plugin_module->plugin_definition->api, init_postfix);

        FARPROC init_proc = GetProcAddress(plugin_module->handle, init_function_name);
        if (init_proc)
        {
            // TODO: Make these functions casted to a function signature typedef
            int32_t init_ret = (int32_t)init_proc(plugin_module->api->context);
            // TODO: Do something with the return value;
            (void)init_ret;
        }
        else
        {
            printf("Error: %s() not found\n", init_function_name);
        }
    }

    return 0;
}