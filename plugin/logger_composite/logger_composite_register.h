#pragma once

#include "plugin_dependencies.h"

#pragma pack(push, 8)

struct LoggerInterface;

typedef struct LoggerInterfaceContext
{
    PLUGIN_CONTEXT_DEPENDENCIES
} LoggerInterfaceContext;

#pragma pack(pop)

