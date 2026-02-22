#pragma once

#include <plugin_manager_common.h>
#include <stdint.h>

TODO("Add static linking functionality")

#ifndef PLUGIN_MANAGER_STATIC_LINKING

struct PluginManagerRuntimeContext;
struct PluginManagerSetupContext;

struct PluginManagerRuntimeContext *__get_plugin_manager_runtime_context();

int32_t __plugin_manager_init(struct PluginManagerSetupContext **setup_context, int argc, char **argv, void *platform_context);
int32_t __plugin_manager_add(struct PluginManagerSetupContext *setup_context, const char *api_name, const char *plugin_name);
int32_t __plugin_manager_load(struct PluginManagerSetupContext *setup_context, struct PluginManagerRuntimeContext *runtime_context);
int32_t __plugin_manager_get(struct PluginManagerRuntimeContext *runtime_context, const char *api_name, void **api_interface);
int32_t __plugin_manager_shutdown(struct PluginManagerRuntimeContext *runtime_context);

#endif // #ifndef PLUGIN_MANAGER_STATIC_LINKING

#if WINDOWS_GUI
#include <Windows.h>
#define __PLUGIN_MANAGER_ENTRY_IMPL()                                                                    \
    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,                                    \
                        LPWSTR lpCmdLine, int nCmdShow)                                                  \
    {                                                                                                    \
        (void)hPrevInstance;                                                                             \
        (void)lpCmdLine;                                                                                 \
                                                                                                         \
        HRESULT hr = {0};                                                                                \
        TODO("Check if this belongs here or somewhere else (also replace CoUninitialize if replacing)"); \
        hr = CoInitialize(NULL);                                                                         \
                                                                                                         \
        struct PluginManagerSetupContext *__context;                                                     \
        static struct                                                                                    \
        {                                                                                                \
            void *hInstance;                                                                             \
            int nCmdShow;                                                                                \
        } platform_context;                                                                              \
        platform_context.hInstance = hInstance;                                                          \
        platform_context.nCmdShow = nCmdShow;                                                            \
                                                                                                         \
        (void)__plugin_manager_init(&__context, __argc, __argv, &platform_context);                      \
        int ret = plugin_api_main(__context);                                                            \
        (void)__plugin_manager_shutdown(__get_plugin_manager_runtime_context());                           \
        return ret;                                                                                      \
    }
#else // #if WINDOWS_GUI
#define __PLUGIN_MANAGER_ENTRY_IMPL()                              \
    int main(int argc, char *argv[])                               \
    {                                                              \
        struct PluginManagerSetupContext *__context;               \
        (void)__plugin_manager_init(&__context, argc, argv, NULL); \
        return plugin_api_main(__context);                         \
    }
#endif // #if WINDOWS_GUI

#define PLUGIN_MANAGER_API_MAIN()                                     \
    int plugin_api_main(struct PluginManagerSetupContext *__context); \
    __PLUGIN_MANAGER_ENTRY_IMPL()                                     \
    int plugin_api_main(struct PluginManagerSetupContext *__context)

#ifdef PLUGIN_MANAGER_STATIC_LINKING
#define PLUGIN_MANAGER_API_ADD(api_name, plugin_name)
#else
#define PLUGIN_MANAGER_API_ADD(api_name, plugin_name) (__plugin_manager_add(__context, api_name, plugin_name))
#endif // #ifdef PLUGIN_MANAGER_STATIC_LINKING

#ifdef PLUGIN_MANAGER_STATIC_LINKING
#define PLUGIN_MANAGER_API_LOAD()
#else
#define PLUGIN_MANAGER_API_LOAD() (__plugin_manager_load(__context, __get_plugin_manager_runtime_context()))
#endif // #ifdef PLUGIN_MANAGER_STATIC_LINKING

#ifdef PLUGIN_MANAGER_STATIC_LINKING
#define PLUGIN_MANAGER_API_GET(api_name, out_api_interface)
#else
#define PLUGIN_MANAGER_API_GET(api_name, api_interface) (__plugin_manager_get(__get_plugin_manager_runtime_context(), api_name, (void **)api_interface))
#endif // #ifdef PLUGIN_MANAGER_STATIC_LINKING
