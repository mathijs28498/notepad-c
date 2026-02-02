#include "plugin_loader_json_parser.h"

#include <string.h>
#include <cJSON.h>
#include <stdio.h>

// 1. Define a static memory arena (e.g., 4KB)
#define STATIC_BUFFER_SIZE 4096
static unsigned char static_buffer[STATIC_BUFFER_SIZE];
static size_t buffer_offset = 0;

// 2. Custom Malloc: Allocates from the static buffer
void *static_malloc(size_t size)
{
    // Ensure memory alignment (8-byte alignment is standard for most architectures)
    size_t aligned_size = (size + 7) & ~7;

    if (buffer_offset + aligned_size > STATIC_BUFFER_SIZE)
    {
        return NULL; // Out of memory
    }

    void *ptr = (void *)(static_buffer + buffer_offset);
    buffer_offset += aligned_size;
    return ptr;
}

// 3. Custom Free: No-op (we reset the entire buffer at once later)
void static_free(void *ptr)
{
    // Individual frees are not supported in a simple bump allocator.
    // Memory is reclaimed by resetting 'buffer_offset' to 0.
    (void)ptr;
}

void safe_strcpy(char *dest, const char *src, size_t dest_size)
{
    if (src == NULL)
    {
        dest[0] = '\0';
        return;
    }
    strncpy_s(dest, dest_size, src, _TRUNCATE);
}

int plugin_loader_parse_config(const char *json_str, PluginConfig *config_out)
{
    memset(config_out, 0, sizeof(PluginConfig));

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return -1;
    }

    // Parse plugins array
    cJSON *plugins_array = cJSON_GetObjectItem(root, "plugins");
    if (!cJSON_IsArray(plugins_array))
    {
        printf("Plugins object is not an array");
        return -1;
    }

    int plugins_count = 0;
    cJSON *plugin_item = NULL;
    cJSON_ArrayForEach(plugin_item, plugins_array)
    {
        if (plugins_count >= PLUGIN_CONFIG_MAX_PLUGIN_COUNT)
            break;

        cJSON *name = cJSON_GetObjectItem(plugin_item, "name");
        cJSON *path = cJSON_GetObjectItem(plugin_item, "path");
        cJSON *implements = cJSON_GetObjectItem(plugin_item, "implements");

        if (cJSON_IsString(name))
        {
            safe_strcpy(config_out->plugins[plugins_count].name,
                        name->valuestring, PLUGIN_CONFIG_MAX_PLUGIN_NAME_COUNT);
        }

        if (cJSON_IsString(path))
        {
            safe_strcpy(config_out->plugins[plugins_count].path,
                        path->valuestring, PLUGIN_CONFIG_MAX_PLUGIN_PATH_COUNT);
        }

        if (cJSON_IsString(implements))
        {
            safe_strcpy(config_out->plugins[plugins_count].implements,
                        implements->valuestring, PLUGIN_CONFIG_MAX_PLUGIN_API_NAME_COUNT);
        }

        plugins_count++;
    }
    config_out->plugins_count = plugins_count;

    cJSON_Delete(root);
    return 0;
}

