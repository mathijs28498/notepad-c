#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #include <test_plugin.h>
#include <Windows.h>

#include <jsmn.h>

#include <plugin_loader.h>
#include <plugin_api.h>

void test_jsmn()
{
    FILE *system_json_file;
    errno_t ret = fopen_s(&system_json_file, "../system.json", "rb");
    (void)ret;

    // TODO: Do this without malloc (get a define with the size of the file)
    fseek(system_json_file, 0, SEEK_END);
    size_t length = ftell(system_json_file);
    fseek(system_json_file, 0, SEEK_SET);
    // Reserve 1 byte for the null terminator
    char *buffer = malloc(length + 1);
    if (buffer)
    {
        fread(buffer, 1, length, system_json_file);
        buffer[length] = '\0';
    }
    fclose(system_json_file);

    printf("%s\n", buffer);

    jsmn_parser p;
    jsmntok_t t[128];

    jsmn_init(&p);

    int r = jsmn_parse(&p, buffer, strlen(buffer), t, 128);
    (void)r;

    for (int i = 0; i < 10; i++)
    {
        printf("type: %d\n", (int)t[i].type);
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    test_jsmn();

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