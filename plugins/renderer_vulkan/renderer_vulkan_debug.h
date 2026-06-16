#pragma once

#include <stdint.h>

#include <plugin_sdk/renderer/v1/renderer_types.h>

struct RendererContext;

int32_t renderer_vulkan_debug_init(struct RendererContext *context);
void renderer_vulkan_debug_start_capture(struct RendererContext *context);
void renderer_vulkan_debug_end_capture(struct RendererContext *context);

int32_t renderer_vulkan_debug_rename_buffer(struct RendererContext *context, RendererBufferHandle buffer_handle, const char *name);