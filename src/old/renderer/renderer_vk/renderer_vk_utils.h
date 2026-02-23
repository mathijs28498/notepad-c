#pragma once
#ifndef RENDERER_RENDERER_VK_UTILS_H
#define RENDERER_RENDERER_VK_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <system_api.h>

#ifndef VULKAN_H_
typedef int VkResult;
#define VK_SUCCESS 0
#endif // #ifndef VULKAN_H_

#define VK_CHECK(expr, logger, fmt, ...) do { \
    VkResult _res = (expr); \
    if (_res != VK_SUCCESS) \
    { \
        LOG_ERR(logger, "renderer_vk", "[%s:%d] VkResult %d: " fmt, __FILE__, __LINE__, _res, ##__VA_ARGS__); \
        return _res; \
    } \
} while (0)

#endif // #ifndef RENDERER_RENDERER_VK_UTILS_H