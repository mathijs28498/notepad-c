#pragma once
#ifndef PLUGIN_IMPL_H
#define PLUGIN_IMPL_H

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)

#define PLUGIN_DEPENDENCY_STRING(api_type, api_var_name, api_name) #api_name,
#define PLUGIN_DEPENDENCY_SETTER(api_type, api_var_name, api_name)                      \
    __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _set_##api_name)(api_type * api) \
    {                                                                                   \
        api_var_name = api;                                                             \
    }

#define PLUGIN_REGISTER_DEPENDENCIES(DEPENDENCY_LIST)                                                                        \
    __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _get_dependencies)(const char *const **dependencies, uint32_t *count) \
    {                                                                                                                        \
        static const char *const plugin_dependencies[] = {                                                                   \
            DEPENDENCY_LIST(PLUGIN_DEPENDENCY_STRING)};                                                                      \
                                                                                                                             \
        assert(dependencies != NULL);                                                                                        \
        assert(count != NULL);                                                                                               \
                                                                                                                             \
        *dependencies = plugin_dependencies;                                                                                 \
        *count = (uint32_t)(sizeof(plugin_dependencies) / sizeof(plugin_dependencies[0]));                                   \
    }                                                                                                                        \
    DEPENDENCY_LIST(PLUGIN_DEPENDENCY_SETTER)

#define PLUGIN_REGISTER_API(api_type, api_var_name)                         \
    __declspec(dllexport) api_type *CONCAT(PLUGIN_API_NAME, _get_api)(void) \
    {                                                                       \
        return &api_var_name;                                               \
    }

#define PLUGIN_REGISTER_INIT(init_fn)                                  \
    __declspec(dllexport) int32_t CONCAT(PLUGIN_API_NAME, _init)(void) \
    {                                                                  \
        return init_fn();                                              \
    }

#endif // #ifndef PLUGIN_IMPL_H