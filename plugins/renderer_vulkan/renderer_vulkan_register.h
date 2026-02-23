#pragma once

#pragma pack(push, 8)

struct LoggerInterface;

typedef struct RendererVulkanContext
{
    struct LoggerInterface *logger;
} RendererVulkanContext;

#pragma pack(pop)