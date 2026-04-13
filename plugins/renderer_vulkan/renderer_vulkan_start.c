#include "renderer_vulkan_start.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <assert.h>

#include <vk_mem_alloc.h>

#include <plugin_utils.h>
#include <logger_interface.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_start, LOG_LEVEL_WARNING)

#include "renderer_vulkan_bootstrap.h"
#include "renderer_vulkan_utils.h"
#include "renderer_vulkan_register.h"

int32_t init_commands(RendererContext *context)
{
    assert(context != NULL);
    VkResult result;
    int32_t ret;

    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = context->graphics_queue_family,
    };

    VkCommandBufferAllocateInfo command_buffer_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandBufferCount = 1,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    };

    RV_VkDestroyData destroy_data = {
        .context = context->device,
        .destroy_3 = (rv_vk_destroy_3)vkDestroyCommandPool,
    };

    for (size_t i = 0; i < ARRAY_SIZE(context->frames); i++)
    {
        RendererFrameData *frame = &context->frames[i];
        VK_RETURN_IF_ERROR(context->logger, result,
                           vkCreateCommandPool(context->device, &command_pool_create_info, NULL, &frame->command_pool),
                           -1, "Failed to create command pool for frame %d: %d", i, result);

        destroy_data.subject = &context->frames[i].command_pool;
        RETURN_IF_ERROR(context->logger, ret, rv_vk_destroy_queue_push(context->logger, &context->destroy_queue, &destroy_data),
                        "Failed to push command pool destroy data to destroy queue: %d", ret);

        command_buffer_alloc_info.commandPool = frame->command_pool;

        VK_RETURN_IF_ERROR(context->logger, result,
                           vkAllocateCommandBuffers(context->device, &command_buffer_alloc_info, &frame->main_command_buffer),
                           -1, "Failed to allocate command buffer for frame %d: %d", i, result);
    }

    return 0;
}

int32_t create_sync_structures(RendererContext *context)
{
    assert(context != NULL);

    VkResult result;
    int32_t ret;

    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    RV_VkDestroyData destroy_data = {
        .context = context->device,
    };

    for (size_t i = 0; i < ARRAY_SIZE(context->frames); i++)
    {
        RendererFrameData *frame = &context->frames[i];
        VK_RETURN_IF_ERROR(context->logger, result, vkCreateFence(context->device, &fence_create_info, NULL, &frame->render_fence),
                           -1, "Failed to create fence for frame %d: %d", i, result);

        destroy_data.subject = &context->frames[i].render_fence;
        destroy_data.destroy_3 = (rv_vk_destroy_3)vkDestroyFence;
        RETURN_IF_ERROR(context->logger, ret, rv_vk_destroy_queue_push(context->logger, &context->destroy_queue, &destroy_data),
                        "Failed to push render fence destroy data to destroy queue: %d", ret);

        VK_RETURN_IF_ERROR(context->logger, result, vkCreateSemaphore(context->device, &semaphore_create_info, NULL, &frame->render_semaphore),
                           -1, "Failed to create render semaphore for frame %d: %d", i, result);

        destroy_data.subject = &context->frames[i].render_semaphore;
        destroy_data.destroy_3 = (rv_vk_destroy_3)vkDestroySemaphore;
        RETURN_IF_ERROR(context->logger, ret, rv_vk_destroy_queue_push(context->logger, &context->destroy_queue, &destroy_data),
                        "Failed to push render semaphore destroy data to destroy queue: %d", ret);

        VK_RETURN_IF_ERROR(context->logger, result, vkCreateSemaphore(context->device, &semaphore_create_info, NULL, &frame->swapchain_semaphore),
                           -1, "Failed to create swacphain semaphore for frame %d: %d", i, result);

        destroy_data.subject = &context->frames[i].swapchain_semaphore;
        destroy_data.destroy_3 = (rv_vk_destroy_3)vkDestroySemaphore;
        RETURN_IF_ERROR(context->logger, ret, rv_vk_destroy_queue_push(context->logger, &context->destroy_queue, &destroy_data),
                        "Failed to push swapchain semaphore destroy data to destroy queue: %d", ret);
    }

    return 0;
}

int32_t create_vma_allocator(RendererContext *context)
{
    assert(context != NULL);

    VkResult result;
    int32_t ret;

    VmaAllocatorCreateInfo vma_alloc_create_info = {
        .physicalDevice = context->physical_device,
        .device = context->device,
        .instance = context->instance,
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
    };

    VK_RETURN_IF_ERROR(context->logger, result, vmaCreateAllocator(&vma_alloc_create_info, &context->vma_allocator),
                       -1, "Unable to create vma allocator: %d", result);

    RV_VkDestroyData destroy_data = {
        .allocator = context->vma_allocator,
        .destroy_1 = vmaDestroyAllocator,
    };

    RETURN_IF_ERROR(context->logger, ret, rv_vk_destroy_queue_push(context->logger, &context->destroy_queue, &destroy_data),
                    "Failed to push swapchain semaphore destroy data to destroy queue: %d", ret);

    return 0;
}

int32_t renderer_vulkan_start(RendererContext *context)
{
    assert(context != NULL);

    int32_t ret;
    VK_TRY_INIT(context->logger, ret, renderer_vulkan_bootstrap(context), &context->destroy_queue,
                "Failed to bootstrap vulkan: %d", ret);

    VK_TRY_INIT(context->logger, ret, init_commands(context), &context->destroy_queue,
                "Failed to init commands: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_sync_structures(context), &context->destroy_queue,
                "Failed to create sync structures: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_vma_allocator(context), &context->destroy_queue,
                "Failed to create vma allocator: %d", ret);

    return 0;
}