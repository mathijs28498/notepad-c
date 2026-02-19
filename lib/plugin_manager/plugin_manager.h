#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct LoggerApi;
struct RequestedPlugin;

int32_t  plugin_manager_add_internal(
    const struct LoggerApi *logger_api,
    const char *api_name,
    const char *plugin_name,
    bool is_explicit,
    struct RequestedPlugin *requested_plugins,
    size_t *requested_plugins_len);