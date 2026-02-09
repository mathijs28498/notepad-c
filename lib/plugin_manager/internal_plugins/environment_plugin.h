#pragma once
#ifndef ENVIRONMENT_PLUGIN_H
#define ENVIRONMENT_PLUGIN_H

#include <stdint.h>

typedef struct EnvironmentApiContext {
    int argc;
    char **argv;
} EnvironmentApiContext;

struct EnvironmentApi;

struct EnvironmentApi *environment_api_get_api(void);
int32_t environment_plugin_set_args(EnvironmentApiContext *context, int argc, char **argv);

#endif // #ifndef ENVIRONMENT_PLUGIN_H