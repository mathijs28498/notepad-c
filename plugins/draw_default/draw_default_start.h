#pragma once

#include <stdint.h>

 #include <plugin_sdk/renderer/v1/renderer_types.h>

struct DrawContext;

int32_t draw_default_create_draw_image(struct DrawContext *context, RendererImageHandle *out_image_handle);
int32_t draw_default_start(struct DrawContext *context);
void draw_default_cleanup(struct DrawContext *context);