#include "draw_default_render_frame.h"

#include <stdint.h>
#include <math.h>
#include <string.h>

#include <cglm/cglm.h>

#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(draw_default_render_frame, LOG_LEVEL_DEBUG);

#include "draw_default_register.h"
#include "draw_default_start.h"
#include "draw_default.h"

int32_t draw_get_resource_handle_by_name(DrawContext *context, const char *name, DrawResource **out_draw_resource)
{
    assert(context != NULL);
    assert(name != NULL);
    assert(out_draw_resource != NULL);

    for (uint32_t i = 0; i < GET_ARRAY_LENGTH(context->resources_a); i++)
    {
        DrawResource *resource = &context->resources_a[i];
        if (strcmp(resource->name, name) == 0)
        {
            *out_draw_resource = resource;
            return 0;
        }
    }

    return -1;
}

int32_t draw_background_pass(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    int32_t ret;

    DrawContext *context = (DrawContext *)user_data;

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    DrawResource *draw_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "draw_image", &draw_image_resource),
                    "Failed to get draw image resource: %d", ret);

    RendererImageProperties draw_image_properties;
    RETURN_IF_ERROR(logger, ret, renderer_get_image_properties(renderer, draw_image_resource->resource_handle, &draw_image_properties),
                    "Failed to get draw image properties: %d", ret);
    RendererExtent2D render_extent_2d = {
        .width = draw_image_properties.extent.width,
        .height = draw_image_properties.extent.height,
    };

    RendererResourceSetHandle draw_image_resource_set_handle;
    RETURN_IF_ERROR(logger, ret, renderer_allocate_transient_resource_set(renderer, context->draw_image_resource_set_layout_handle, &draw_image_resource_set_handle),
                    "Failed to allocate transient resource set: %d", ret);

    RendererResourceImageBinding draw_image_binding = {
        .image_handle = draw_image_resource->resource_handle,
        .image_layout = RENDERER_IMAGE_LAYOUT_GENERAL,
    };
    RendererResourceSetWrite draw_image_resource_set_write = {
        .binding = 0,
        .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_IMAGE,
        .resource_bindings_len = 1,
        .image_bindings = &draw_image_binding,
    };

    RendererResourceSetUpdateInfo resource_set_update_info = {
        .resource_set_handle = draw_image_resource_set_handle,
        .resource_set_writes_len = 1,
        .resource_set_writes = &draw_image_resource_set_write,
    };

    renderer_update_resource_set(renderer, &resource_set_update_info);

    
    BackgroundPushConstants push_constants = {
        .top_left = {1, 0, 0, 1},
        .top_right = {0, 0, 1, 1},
        .bottom_left = {0, 1, 1, 1},
        .bottom_right = {0, 1, 0, 1},
    };

    renderer_cmd_bind_compute_pipeline(renderer, command_list, context->background_pipeline_handle);
    renderer_cmd_bind_resource_sets(renderer, command_list, RENDERER_PIPELINE_TYPE_COMPUTE, context->background_pipeline_layout_handle, 0, 1, &draw_image_resource_set_handle, 0, NULL);
    renderer_cmd_push_constants(renderer, command_list, context->background_pipeline_layout_handle, RENDERER_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(BackgroundPushConstants), &push_constants);

    renderer_cmd_dispatch(renderer, command_list, (uint32_t)ceil(render_extent_2d.width / 16.0), (uint32_t)ceil(render_extent_2d.height / 16.0), 1);

    return 0;
}

int32_t draw_2d_pass(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    int32_t ret;

    DrawContext *context = (DrawContext *)user_data;

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    DrawResource *render_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "render_image", &render_image_resource),
                    "Failed to get render image resource: %d", ret);

    RendererImageProperties render_image_properties;
    RETURN_IF_ERROR(logger, ret, renderer_get_image_properties(renderer, render_image_resource->resource_handle, &render_image_properties),
                    "Failed to get render image properties: %d", ret);
    RendererExtent2D render_extent_2d = {
        .width = render_image_properties.extent.width,
        .height = render_image_properties.extent.height,
    };

    DrawResource *draw_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "draw_image", &draw_image_resource),
                    "Failed to get draw image resource: %d", ret);
    // Start graphics rendering
    RendererBeginRenderingInfo begin_rendering_info = {
        .color_attachment_info = {
            .image_handle = draw_image_resource->resource_handle,
            .load_op = RENDERER_ATTACHMENT_LOAD_OP_LOAD,
            .store_op = RENDERER_ATTACHMENT_STORE_OP_STORE,
        },
        .depth_attachment_info = NULL,
    };
    renderer_cmd_begin_rendering(renderer, command_list, &begin_rendering_info);
    renderer_cmd_set_viewport(renderer, command_list, render_extent_2d);
    renderer_cmd_set_scissor(renderer, command_list, render_extent_2d);

    renderer_cmd_bind_graphics_pipeline(renderer, command_list, context->triangle_pipeline_handle);
    renderer_cmd_draw(renderer, command_list, 3, 1, 0, 0);

    renderer_cmd_bind_graphics_pipeline(renderer, command_list, context->triangle_mesh_pipeline_handle);
    GPUDrawPushConstants mesh_push_constants = {
        .world_matrix = GLM_MAT4_IDENTITY_INIT,
        .vertex_buffer_address = context->rect_mesh_buffers.vertex_buffer_address,
    };
    renderer_cmd_push_constants(renderer, command_list, context->triangle_mesh_pipeline_layout_handle, RENDERER_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &mesh_push_constants);
    renderer_cmd_bind_index_buffer(renderer, command_list, context->rect_mesh_buffers.index_buffer_handle);
    renderer_cmd_draw_indexed(renderer, command_list, context->rect_mesh_buffers.indices_len, 1, 0, 0, 0);
    renderer_cmd_end_rendering(renderer, command_list);

    return 0;
}

int32_t draw_final_pass(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    int32_t ret;

    DrawContext *context = (DrawContext *)user_data;

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    DrawResource *render_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "render_image", &render_image_resource),
                    "Failed to get render image resource: %d", ret);
    RendererImageProperties render_image_properties;
    RETURN_IF_ERROR(logger, ret, renderer_get_image_properties(renderer, render_image_resource->resource_handle, &render_image_properties),
                    "Failed to get render image properties: %d", ret);
    RendererExtent2D render_extent_2d = {
        .width = render_image_properties.extent.width,
        .height = render_image_properties.extent.height,
    };

    DrawResource *draw_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "draw_image", &draw_image_resource),
                    "Failed to get draw image resource: %d", ret);
    renderer_cmd_blit_image_to_image(renderer, command_list, draw_image_resource->resource_handle, render_image_resource->resource_handle,
                                     context->draw_extent, render_extent_2d);

    return 0;
}

int32_t draw_execute_render_graph(DrawContext *context, RendererCommandList *command_list)
{
    assert(context != NULL);
    assert(command_list != NULL);

    int32_t ret;

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    DrawResource *render_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "render_image", &render_image_resource),
                    "Failed to get render image resource: %d", ret);
    DrawResource *draw_image_resource;
    RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "draw_image", &draw_image_resource),
                    "Failed to get draw image resource: %d", ret);

    render_image_resource->resource_handle = renderer_get_render_image_handle(renderer);

    // Transition to pass 0
    renderer_cmd_transition_image(renderer, command_list, draw_image_resource->resource_handle, RENDERER_IMAGE_LAYOUT_UNDEFINED, RENDERER_IMAGE_LAYOUT_GENERAL);

    // Execute pass 0

    draw_background_pass(command_list, context);

    // Transition to pass 1
    renderer_cmd_transition_image(renderer, command_list, draw_image_resource->resource_handle, RENDERER_IMAGE_LAYOUT_GENERAL, RENDERER_IMAGE_LAYOUT_COLOR_ATTACHMENT);

    // Execute pass 1
    draw_2d_pass(command_list, context);

    // Transition to final pass

    renderer_cmd_transition_image(renderer, command_list, draw_image_resource->resource_handle, RENDERER_IMAGE_LAYOUT_COLOR_ATTACHMENT, RENDERER_IMAGE_LAYOUT_TRANSFER_SRC);
    renderer_cmd_transition_image(renderer, command_list, render_image_resource->resource_handle, RENDERER_IMAGE_LAYOUT_UNDEFINED, RENDERER_IMAGE_LAYOUT_TRANSFER_DST);

    // Execute final pass
    draw_final_pass(command_list, context);

    // Transition to present
    renderer_cmd_transition_image(renderer, command_list, render_image_resource->resource_handle, RENDERER_IMAGE_LAYOUT_TRANSFER_DST, RENDERER_IMAGE_LAYOUT_PRESENT_SRC);
    return 0;
}

int32_t draw_default_render_frame(DrawContext *context, RendererCommandList *command_list)
{
    assert(context != NULL);
    assert(command_list != NULL);

    int32_t ret;
    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    if (renderer_consume_has_resized(renderer))
    {
        DrawResource *draw_image_resource;
        RETURN_IF_ERROR(logger, ret, draw_get_resource_handle_by_name(context, "draw_image", &draw_image_resource),
                        "Failed to get draw image resource: %d", ret);
        RETURN_IF_ERROR(logger, ret, renderer_destroy_image(renderer, draw_image_resource->resource_handle),
                        "Failed to destroy draw image: %d", ret);
        RETURN_IF_ERROR(logger, ret, draw_default_create_draw_image(context, &draw_image_resource->resource_handle),
                        "Failed to create draw image: %d", ret);
    }

    RETURN_IF_ERROR(logger, ret, draw_execute_render_graph(context, command_list),
                    "Failed to execute render graph: %d", ret);

    return 0;
}