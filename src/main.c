#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #include <test_plugin.h>
#include <Windows.h>

#include <plugin_loader.h>
#include <plugin_api.h>

int main(int argc, char* argv[])
{
    (void) argc;    
    (void) argv;    

    PluginApi *plugin_api = get_plugin_api();

    plugin_api->add_search_path("./");

    plugin_api->get("test", NULL);

    // HMODULE test_plugin = LoadLibrary("TestPlugin.dll");
    // if (!test_plugin)
    // {
    //     printf("Failed to load plugin\n");
    //     return -1;
    // }

    // FARPROC proc = GetProcAddress(test_plugin, "test_func");
    // if (!proc)
    // {
    //     printf("Failed to get proc\n");
    //     return -1;
    // }
    // proc(1300);

    // test_func(1300);
}