#pragma once

#include <stdint.h>

struct DrawContext;
struct RendererCommandList;
struct DrawResource;

int32_t draw_default_render_frame(struct DrawContext *context, struct RendererCommandList *command_list);
int32_t draw_get_resource_handle_by_name(struct DrawContext *context, const char *name, struct DrawResource **out_draw_resource);