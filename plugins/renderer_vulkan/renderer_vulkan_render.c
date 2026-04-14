#include "renderer_vulkan_render.h"

#include <vk_mem_alloc.h>

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <assert.h>
#include <math.h>


#include <logger_interface.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_render, LOG_LEVEL_DEBUG)

#include "renderer_vulkan_utils.h"
#include "renderer_vulkan_register.h"

#define SECOND_IN_NS 1000000000

void draw_background(RendererContext *context, VkCommandBuffer cmd)
{
    float color_val = fabsf(sinf(context->frame_number / 120.f));
    float color_val_1 = fabsf(cosf(context->frame_number / 120.f));
    float color_val_2 = 1.f - fabsf(cosf(context->frame_number / 120.f));
    VkClearColorValue clear_color = {{color_val_1, color_val_2, color_val, 1.}};
    VkImageSubresourceRange clear_range = rv_image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, context->draw_image.image, VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_range);
}

int32_t renderer_vulkan_render(RendererContext *context)
{
    assert(context != NULL);

    VkResult result;
    RendererFrameData *frame = &context->frames[context->frame_number % ARRAY_SIZE(context->frames)];

    VK_RETURN_IF_ERROR(context->logger, result, vkWaitForFences(context->device, 1, &frame->render_fence, VK_TRUE, SECOND_IN_NS),
                       -1, "Failed to wait for render fence: %d", result);

    rv_call_queue_flush(&frame->destroy_queue);

    VK_RETURN_IF_ERROR(context->logger, result, vkResetFences(context->device, 1, &frame->render_fence),
                       -1, "Failed to reset render fence: %d", result);

    uint32_t swapchain_image_index;
    VK_RETURN_IF_ERROR(
        context->logger, result,
        vkAcquireNextImageKHR(context->device, context->swapchain, SECOND_IN_NS, frame->swapchain_semaphore, VK_NULL_HANDLE, &swapchain_image_index),
        -1, "Failed to acquire next image: %d", result);
    assert(swapchain_image_index < GET_ARRAY_LENGTH(context->swapchain_images));
    VkImage swapchain_image = context->swapchain_images[swapchain_image_index];

    VkCommandBuffer cmd = frame->main_command_buffer;

    VK_RETURN_IF_ERROR(context->logger, result, vkResetCommandBuffer(cmd, 0),
                       -1, "Failed to reset command buffer: %d", result);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_RETURN_IF_ERROR(context->logger, result, vkBeginCommandBuffer(cmd, &begin_info),
                       -1, "Failed to begin command buffer: %d", result);

    rv_transition_image(cmd, swapchain_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // draw_background(context, cmd);

    float color_val = fabsf(sinf(context->frame_number / 120.f));
    float color_val_1 = fabsf(cosf(context->frame_number / 120.f));
    float color_val_2 = 1.f - fabsf(cosf(context->frame_number / 120.f));
    VkClearColorValue clear_color = {{color_val_1, color_val_2, color_val, 1.}};
    VkImageSubresourceRange clear_range = rv_image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, swapchain_image, VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_range);


    rv_transition_image(cmd, swapchain_image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VK_RETURN_IF_ERROR(context->logger, result, vkEndCommandBuffer(cmd),
                       -1, "Failed to end buffer: %d", result);

    VkCommandBufferSubmitInfo cmd_info = rv_create_command_buffer_submit_info(cmd);

    VkSemaphoreSubmitInfo wait_info = rv_create_semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, frame->swapchain_semaphore);
    VkSemaphoreSubmitInfo signal_info = rv_create_semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, frame->render_semaphore);

    VkSubmitInfo2 submit = rv_create_submit_info(&cmd_info, &signal_info, &wait_info);

    VK_RETURN_IF_ERROR(context->logger, result, vkQueueSubmit2(context->graphics_queue, 1, &submit, frame->render_fence),
                       -1, "Failed to submit cmd to queue: %d", result);

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pSwapchains = &context->swapchain,
        .swapchainCount = 1,

        .pWaitSemaphores = &frame->render_semaphore,
        .waitSemaphoreCount = 1,

        .pImageIndices = &swapchain_image_index,
    };

    VK_RETURN_IF_ERROR(context->logger, result, vkQueuePresentKHR(context->present_queue, &present_info),
                       -1, "Failed to present queue: %d", result);

    context->frame_number++;

    return 0;
}