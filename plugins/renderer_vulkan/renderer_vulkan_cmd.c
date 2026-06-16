#include "renderer_vulkan_cmd.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <assert.h>
#include <bump_arena.h>

#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_cmd, LOG_LEVEL_DEBUG)

#include "renderer_vulkan.h"
#include "renderer_vulkan_register.h"
#include "renderer_vulkan_utils.h"
#include "renderer_vulkan_conversion.h"

void renderer_vulkan_cmd_bind_resource_sets(RendererContext *context, RendererCommandList *command_list, RendererPipelineType renderer_pipeline_type, RendererPipelineLayoutHandle pipeline_layout_handle, uint32_t first_set, uint32_t resource_set_len, const RendererResourceSetHandle *resource_set_handle, uint32_t dynamic_offset_len, const uint32_t *dynamic_offsets)
{
    assert(context != NULL);
    assert(command_list != NULL);

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->pipeline_layout_generations_a, context->pipeline_layouts_a, pipeline_layout_handle, pipeline_layout);

    TODO("Allow for multiple descriptor sets")
    VkDescriptorSet descriptor_set = context->active_frame_state.frame->transient_descriptor_sets_a[(size_t)resource_set_handle[0]];
    vkCmdBindDescriptorSets(command_list->command_buffer, rv_pipeline_type_to_vk_pipeline_bind_point(renderer_pipeline_type), pipeline_layout, first_set, resource_set_len, &descriptor_set, dynamic_offset_len, dynamic_offsets);
}

TODO("Find a backend agnostic thing for this as push constants are vulkan, maybe a feature flag or something")
void renderer_vulkan_cmd_push_constants(RendererContext *context, RendererCommandList *command_list, RendererPipelineLayoutHandle pipeline_layout_handle, RendererShaderStageFlags renderer_shader_stage_flags, uint32_t offset, uint32_t push_constants_size, void *push_constants)
{
    assert(context != NULL);
    assert(command_list != NULL);

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->pipeline_layout_generations_a, context->pipeline_layouts_a, pipeline_layout_handle, pipeline_layout);

    vkCmdPushConstants(command_list->command_buffer, pipeline_layout, rv_shader_stage_to_vk_shader_stage(renderer_shader_stage_flags), offset, push_constants_size, push_constants);
}

void renderer_vulkan_cmd_dispatch(RendererContext *context, RendererCommandList *command_list, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
    assert(context != NULL);
    assert(command_list != NULL);

    vkCmdDispatch(command_list->command_buffer, group_count_x, group_count_y, group_count_z);
}

void renderer_vulkan_cmd_bind_graphics_pipeline(RendererContext *context, RendererCommandList *command_list, RendererGraphicsPipelineHandle pipeline_handle)
{
    assert(context != NULL);
    assert(command_list != NULL);
    VkPipeline pipeline = VK_NULL_HANDLE;
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->pipeline_generations_a, context->pipelines_a, pipeline_handle, pipeline);
    vkCmdBindPipeline(command_list->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void renderer_vulkan_cmd_bind_compute_pipeline(RendererContext *context, RendererCommandList *command_list, RendererComputePipelineHandle pipeline_handle)
{
    assert(context != NULL);
    assert(command_list != NULL);
    VkPipeline pipeline = VK_NULL_HANDLE;
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->pipeline_generations_a, context->pipelines_a, pipeline_handle, pipeline);

    vkCmdBindPipeline(command_list->command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void renderer_vulkan_cmd_bind_index_buffer(RendererContext *context, RendererCommandList *command_list, RendererBufferHandle buffer_handle)
{
    assert(context != NULL);
    assert(command_list != NULL);

    RV_AllocatedBuffer buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                              buffer_handle, buffer);
    TODO("Figure out if other 2 should be arguments");
    vkCmdBindIndexBuffer(command_list->command_buffer, buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}

void renderer_vulkan_cmd_draw_indexed(RendererContext *context, RendererCommandList *command_list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    assert(context != NULL);
    assert(command_list != NULL);

    vkCmdDrawIndexed(command_list->command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void renderer_vulkan_cmd_draw(RendererContext *context, RendererCommandList *command_list, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    assert(context != NULL);
    assert(command_list != NULL);
    vkCmdDraw(command_list->command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

// ways to improve this efficiency: https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
void renderer_vulkan_cmd_transition_image(RendererContext *context, RendererCommandList *command_list, RendererImageHandle image_handle, RendererImageLayout renderer_current_layout, RendererImageLayout renderer_new_layout)
{
    assert(context != NULL);

    RV_AllocatedImage allocated_image = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_image_generations_a, context->allocated_images_a, image_handle, allocated_image);

    VkImageLayout current_layout = rv_image_layout_to_vk_image_layout(renderer_current_layout);
    VkImageLayout new_layout = rv_image_layout_to_vk_image_layout(renderer_new_layout);

    TODO("Make this better, this is too fragile")
    VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageMemoryBarrier2 image_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
        .oldLayout = current_layout,
        .newLayout = new_layout,

        .subresourceRange = rv_image_subresource_range(aspect_mask),
        .image = allocated_image.image,
    };

    VkDependencyInfo dependency_info = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &image_barrier,
    };

    vkCmdPipelineBarrier2(command_list->command_buffer, &dependency_info);
}

void renderer_vulkan_cmd_blit_image_to_image(RendererContext *context, RendererCommandList *command_list, RendererImageHandle image_handle_source, RendererImageHandle image_handle_destination, RendererExtent2D extent_source, RendererExtent2D extent_destination)
{
    assert(context != NULL);

    RV_AllocatedImage allocated_image_source = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_image_generations_a, context->allocated_images_a, image_handle_source, allocated_image_source);

    RV_AllocatedImage allocated_image_destination = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_image_generations_a, context->allocated_images_a, image_handle_destination, allocated_image_destination);

    VkOffset3D src_offset_max = {
        .x = extent_source.width,
        .y = extent_source.height,
        .z = 1,
    };

    VkOffset3D dst_offset_max = {
        .x = extent_destination.width,
        .y = extent_destination.height,
        .z = 1,
    };

    VkImageSubresourceLayers subresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseArrayLayer = 0,
        .layerCount = 1,
        .mipLevel = 0,
    };

    VkImageBlit2 blit_region = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .srcOffsets = {
            {.x = 0, .y = 0, .z = 0},
            src_offset_max,
        },
        .dstOffsets = {
            {.x = 0, .y = 0, .z = 0},
            dst_offset_max,
        },
        .srcSubresource = subresource,
        .dstSubresource = subresource,
    };

    VkBlitImageInfo2 blit_image_info = {
        .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .srcImage = allocated_image_source.image,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = allocated_image_destination.image,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .filter = VK_FILTER_LINEAR,
        .regionCount = 1,
        .pRegions = &blit_region,
    };

    vkCmdBlitImage2(command_list->command_buffer, &blit_image_info);
}

void renderer_vulkan_cmd_begin_rendering(RendererContext *context, RendererCommandList *command_list, const RendererBeginRenderingInfo *renderer_begin_rendering_info)
{
    assert(context != NULL);
    assert(command_list != NULL);
    assert(renderer_begin_rendering_info != NULL);

    TODO("Allow for more options for attachments");

    RV_AllocatedImage color_attachment_image;
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_image_generations_a, context->allocated_images_a,
                                              renderer_begin_rendering_info->color_attachment_info.image_handle, color_attachment_image);

    VkRenderingAttachmentInfo color_attachment_info = rv_attachment_info_to_vk_attachment_info(&renderer_begin_rendering_info->color_attachment_info, &color_attachment_image, RENDERER_ATTACHMENT_TYPE_COLOR);

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .extent = {
                .width = color_attachment_image.image_extent.width,
                .height = color_attachment_image.image_extent.height,
            }},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_info,
    };

    VkRenderingAttachmentInfo depth_attachment_info = {0};
    if (renderer_begin_rendering_info->depth_attachment_info != NULL)
    {
        RV_AllocatedImage depth_attachment_image;
        RV_RES_RENDERER_HANDLE_GET_OR_RETURN_VOID(context->deps.logger, context->allocated_image_generations_a, context->allocated_images_a,
                                                  renderer_begin_rendering_info->depth_attachment_info->image_handle, depth_attachment_image);

        depth_attachment_info = rv_attachment_info_to_vk_attachment_info(renderer_begin_rendering_info->depth_attachment_info, &depth_attachment_image, RENDERER_ATTACHMENT_TYPE_DEPTH);

        rendering_info.pDepthAttachment = &depth_attachment_info;
    }

    vkCmdBeginRendering(command_list->command_buffer, &rendering_info);
}

void renderer_vulkan_cmd_end_rendering(RendererContext *context, RendererCommandList *command_list)
{
    assert(context != NULL);
    assert(command_list != NULL);

    vkCmdEndRendering(command_list->command_buffer);
}

void renderer_vulkan_cmd_set_viewport(RendererContext *context, RendererCommandList *command_list, RendererExtent2D extent)
{
    assert(context != NULL);
    assert(command_list != NULL);

    VkViewport viewport = {
        .x = 0,
        .y = 0,
        .width = (float)extent.width,
        .height = (float)extent.height,
        .minDepth = 0,
        .maxDepth = 1,
    };

    vkCmdSetViewport(command_list->command_buffer, 0, 1, &viewport);
}

void renderer_vulkan_cmd_set_scissor(RendererContext *context, RendererCommandList *command_list, RendererExtent2D extent)
{
    assert(context != NULL);
    assert(command_list != NULL);

    VkRect2D scissor = {
        .offset = {
            .x = 0,
            .y = 0,
        },
        .extent = rv_renderer_extent_2d_to_vk_extent_2d(&extent),
    };

    vkCmdSetScissor(command_list->command_buffer, 0, 1, &scissor);
}

VkPipelineStageFlags rv_pipeline_stage_flags_to_vk_pipeline_stage_flags(RendererPipelineStageFlags flags)
{
    VkPipelineStageFlags vk_flags = 0;
    if (flags & RENDERER_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
        vk_flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    if (flags & RENDERER_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
        vk_flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    if (flags & RENDERER_PIPELINE_STAGE_TRANSFER_BIT)
        vk_flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    if (flags & RENDERER_PIPELINE_STAGE_HOST_BIT)
        vk_flags |= VK_PIPELINE_STAGE_HOST_BIT;
    if (flags & RENDERER_PIPELINE_STAGE_ALL_COMMANDS_BIT)
        vk_flags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    return vk_flags;
}

VkAccessFlags rv_access_flags_to_vk_access_flags(RendererAccessFlags flags)
{
    VkAccessFlags vk_flags = 0;
    if (flags & RENDERER_ACCESS_SHADER_READ_BIT)
        vk_flags |= VK_ACCESS_SHADER_READ_BIT;
    if (flags & RENDERER_ACCESS_SHADER_WRITE_BIT)
        vk_flags |= VK_ACCESS_SHADER_WRITE_BIT;
    if (flags & RENDERER_ACCESS_TRANSFER_READ_BIT)
        vk_flags |= VK_ACCESS_TRANSFER_READ_BIT;
    if (flags & RENDERER_ACCESS_TRANSFER_WRITE_BIT)
        vk_flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    if (flags & RENDERER_ACCESS_HOST_READ_BIT)
        vk_flags |= VK_ACCESS_HOST_READ_BIT;
    if (flags & RENDERER_ACCESS_HOST_WRITE_BIT)
        vk_flags |= VK_ACCESS_HOST_WRITE_BIT;
    return vk_flags;
}

TODO("Turn this into barrier2")
static inline int32_t inl_cmd_barrier(RendererContext *context, RendererCommandList *command_list, const RendererBarrierInfo *renderer_pipeline_barrier_info)
{
    int32_t ret;

    VkBufferMemoryBarrier *vk_buffer_barriers = NULL;
    if (renderer_pipeline_barrier_info->buffer_memory_barrier_len > 0)
    {
        RETURN_IF_ERROR(context->deps.logger, ret,
                        BUMP_ARENA_ALLOC_TYPED(context->bump_arena_a, VkBufferMemoryBarrier, renderer_pipeline_barrier_info->buffer_memory_barrier_len, &vk_buffer_barriers),
                        "Failed to allocate from bump arena: %d", ret);
    }

    for (uint32_t i = 0; i < renderer_pipeline_barrier_info->buffer_memory_barrier_len; ++i)
    {
        const RendererBufferMemoryBarrier *rb = &renderer_pipeline_barrier_info->buffer_memory_barriers[i];

        RV_AllocatedBuffer allocated_buffer = {0};
        RV_RES_RENDERER_HANDLE_GET_OR_RETURN(
            context->deps.logger,
            context->allocated_buffer_generations_a,
            context->allocated_buffers_a,
            rb->buffer,
            allocated_buffer);

        vk_buffer_barriers[i] = (VkBufferMemoryBarrier){
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = rv_access_flags_to_vk_access_flags(rb->src_access_mask),
            .dstAccessMask = rv_access_flags_to_vk_access_flags(rb->dst_access_mask),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = allocated_buffer.buffer,
            .offset = rb->offset,
            .size = (rb->size == RENDERER_WHOLE_SIZE) ? VK_WHOLE_SIZE : rb->size,
        };
    }

    vkCmdPipelineBarrier(
        command_list->command_buffer,
        rv_pipeline_stage_flags_to_vk_pipeline_stage_flags(renderer_pipeline_barrier_info->src_stage_mask),
        rv_pipeline_stage_flags_to_vk_pipeline_stage_flags(renderer_pipeline_barrier_info->dst_stage_mask),
        0,       // Dependency flags (e.g., VK_DEPENDENCY_BY_REGION_BIT)
        0, NULL, // Global memory barriers
        renderer_pipeline_barrier_info->buffer_memory_barrier_len,
        renderer_pipeline_barrier_info->buffer_memory_barrier_len > 0 ? vk_buffer_barriers : NULL,
        0, NULL // Image memory barriers
    );

    return 0;
}

int32_t renderer_vulkan_cmd_barrier(RendererContext *context, RendererCommandList *command_list, const RendererBarrierInfo *barrier_info)
{
    assert(context != NULL);
    assert(command_list != NULL);
    assert(barrier_info != NULL);

    BumpArenaCheckpoint bump_arena_checkpoint = bump_arena_create_checkpoint(context->bump_arena_a);
    int32_t ret = inl_cmd_barrier(context, command_list, barrier_info);
    bump_arena_restore_checkpoint(context->bump_arena_a, bump_arena_checkpoint, true);

    return ret;
}