#include "environment_plugin.h"

#include <environment_api.h>

#include <stdint.h>

#define PLUGIN_API_NAME environment_api

int32_t get_args(EnvironmentApiContext *context, int *argc, char ***argv)
{
    *argc = context->argc;
    *argv = context->argv;
    return 0;
}

int32_t environment_plugin_set_args(EnvironmentApiContext *context, int argc, char **argv)
{
    context->argc = argc;
    context->argv = argv;
    return 0;
}

EnvironmentApi *environment_api_get_api(void)
{
    static EnvironmentApiContext context = {0};

    static EnvironmentApi api = {
        .context = &context,
        .get_args = get_args,
    };

    return &api;
}
