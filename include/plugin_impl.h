#pragma once
#ifndef PLUGIN_IMPL_H
#define PLUGIN_IMPL_H

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)

#define PLUGIN_DEPENDENCY_STRING(api_type, api_var_name, api_name) #api_name,
#define PLUGIN_DEPENDENCY_SETTER(api_type, api_var_name, api_name)                      \
    __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _set_##api_name)(api_type * arg) \
    {                                                                                   \
        (void)arg                                                                       \
            printf("This works %s\n", #api_name);                                       \
    }

// TODO: Add a compile error if the PLUGIN_API_NAME is not defined
// #define PLUGIN_REGISTER_DEPENDENCIES(...)                                                                                   \
//     __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _get_dependencies)(const char *const **dependencies, int32_t *count) \
//     {                                                                                                                       \
//         static const char *const plugin_dependencies[] = {                                                                  \
//             "test_api_4",                                                                                                   \
//             "test_api_3",                                                                                                   \
//         }                                                                                                                   \
//                                                                                                                             \
//         assert(dependencies != NULL);                                                                                       \
//         assert(count != NULL);                                                                                              \
//                                                                                                                             \
//         *dependencies = plugin_dependencies;                                                                                \
//         *count = sizeof(plugin_dependencies) / sizeof(plugin_dependencies[0]);                                              \
//     }

// __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _get_dependencies)(const char *const **dependencies, int32_t *count) \

// TODO: Add the PLUGIN_API_NAME

#define PLUGIN_REGISTER_DEPENDENCIES(DEPENDENCY_LIST)                                                                       \
    __declspec(dllexport) void CONCAT(PLUGIN_API_NAME, _get_dependencies)(const char *const **dependencies, int32_t *count) \
    {                                                                                                                       \
        static const char *const plugin_dependencies[] = {                                                                  \
            DEPENDENCY_LIST(PLUGIN_DEPENDENCY_STRING)};                                                                     \
                                                                                                                            \
        assert(dependencies != NULL);                                                                                       \
        assert(count != NULL);                                                                                              \
                                                                                                                            \
        *dependencies = plugin_dependencies;                                                                                \
        *count = sizeof(plugin_dependencies) / sizeof(plugin_dependencies[0]);                                              \
        printf("%s - %s()\n", __FILE__, __func__);                                                                          \
    }

#endif // #ifndef PLUGIN_IMPL_H