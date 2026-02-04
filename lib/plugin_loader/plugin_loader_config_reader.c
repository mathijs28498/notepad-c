#include "plugin_loader_config_reader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t plugin_loader_read_config(char **buffer_out)
{
    FILE *system_json_file;
    int ret;
    ret = fopen_s(&system_json_file, "../plugin_registry.json", "rb");

    // TODO: Do this without malloc (get a define with the size of the file)
    fseek(system_json_file, 0, SEEK_END);
    size_t length = ftell(system_json_file);
    fseek(system_json_file, 0, SEEK_SET);
    // Reserve 1 byte for the null terminator
    *buffer_out = malloc(length + 1);
    if (*buffer_out)
    {
        fread(*buffer_out, 1, length, system_json_file);
        (*buffer_out)[length] = '\0';
    }
    fclose(system_json_file);

    return 0;
}
