#include "plugin_loader.h"

#include <string.h>
#include <stddef.h>

#include <plugin_api.h>


#define MAX_LIB_SEARCH_PATHS 64
#define MAX_SEARCH_PATH_LEN 512

size_t lib_search_paths_len = 0;
const char lib_search_paths[MAX_SEARCH_PATH_LEN][MAX_LIB_SEARCH_PATHS] = {0};

int32_t add_search_path(const char *search_path)
{
    // TODO: Add logging or something
    if (lib_search_paths_len == MAX_LIB_SEARCH_PATHS)
    {
        return -1;
    }
    if (strlen(search_path) > MAX_SEARCH_PATH_LEN)
    {
        return -2;
    }
    memcpy(
        (char *)lib_search_paths[lib_search_paths_len], 
        search_path, 
        strlen(search_path));
    
        return 0;
}

int32_t plugin_api_get(const char *plugin_name, const void **out_plugin_interface)
{
    (void *)plugin_name;
    out_plugin_interface = NULL;
    return 0;
}

int32_t plugin_api_add(const char *plugin_name, const void **out_plugin_interface)
{
    (void *)plugin_name;
    out_plugin_interface = NULL;
    return 0;
};


PluginApi g_plugin_api = {
    .add_search_path = add_search_path,
    .add = plugin_api_add,
    .get = plugin_api_get,
};

PluginApi *get_plugin_api() {
    return &g_plugin_api;
}

int32_t load_plugin(const char * const plugin_name)
{
    (void*) plugin_name;

    return 0;
}