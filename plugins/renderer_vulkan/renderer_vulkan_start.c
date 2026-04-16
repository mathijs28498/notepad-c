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

int32_t create_command_buffers(RendererContext *context)
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

    for (size_t i = 0; i < ARRAY_SIZE(context->frames); i++)
    {
        RendererFrameData *frame = &context->frames[i];
        VK_RETURN_IF_ERROR(context->logger, result,
                           vkCreateCommandPool(context->device, &command_pool_create_info, NULL, &frame->command_pool),
                           -1, "Failed to create command pool for frame %d: %d", i, result);

        RETURN_IF_ERROR(context->logger, ret,
                        RV_CALL_QUEUE_PUSH_3(context->logger, &context->main_destroy_queue, vkDestroyCommandPool, context->device, context->frames[i].command_pool, NULL),
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

    for (size_t i = 0; i < ARRAY_SIZE(context->frames); i++)
    {
        RendererFrameData *frame = &context->frames[i];
        VK_RETURN_IF_ERROR(context->logger, result, vkCreateFence(context->device, &fence_create_info, NULL, &frame->render_fence),
                           -1, "Failed to create fence for frame %d: %d", i, result);

        RETURN_IF_ERROR(context->logger, ret,
                        RV_CALL_QUEUE_PUSH_3(context->logger, &context->main_destroy_queue, vkDestroyFence, context->device, frame->render_fence, NULL),
                        "Failed to push render fence destroy data to destroy queue: %d", ret);

        VK_RETURN_IF_ERROR(context->logger, result, vkCreateSemaphore(context->device, &semaphore_create_info, NULL, &frame->render_semaphore),
                           -1, "Failed to create render semaphore for frame %d: %d", i, result);

        RETURN_IF_ERROR(context->logger, ret,
                        RV_CALL_QUEUE_PUSH_3(context->logger, &context->main_destroy_queue, vkDestroySemaphore, context->device, frame->render_semaphore, NULL),
                        "Failed to push render semaphore destroy data to destroy queue: %d", ret);

        VK_RETURN_IF_ERROR(context->logger, result, vkCreateSemaphore(context->device, &semaphore_create_info, NULL, &frame->swapchain_semaphore),
                           -1, "Failed to create swacphain semaphore for frame %d: %d", i, result);

        RETURN_IF_ERROR(context->logger, ret,
                        RV_CALL_QUEUE_PUSH_3(context->logger, &context->main_destroy_queue, vkDestroySemaphore, context->device, frame->swapchain_semaphore, NULL),
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
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
    };

    VK_RETURN_IF_ERROR(context->logger, result, vmaCreateAllocator(&vma_alloc_create_info, &context->vma_allocator),
                       -1, "Unable to create vma allocator: %d", result);

    RETURN_IF_ERROR(context->logger, ret,
                    RV_CALL_QUEUE_PUSH_1(context->logger, &context->main_destroy_queue, vmaDestroyAllocator, context->vma_allocator),
                    "Failed to push swapchain semaphore destroy data to destroy queue: %d", ret);

    return 0;
}

int32_t create_draw_image(RendererContext *context)
{
    assert(context != NULL);

    VkResult result;
    uint32_t ret;

    VkExtent3D draw_image_extent = {
        .width = context->swapchain_extent.width,
        .height = context->swapchain_extent.height,
        .depth = 1,
    };

    VkFormat draw_image_format = VK_FORMAT_R16G16B16A16_SFLOAT;

    VkImageUsageFlags draw_image_usage_flags =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_STORAGE_BIT |
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo draw_image_create_info = rv_create_image_info(
        draw_image_format, draw_image_usage_flags, draw_image_extent);

    VmaAllocationCreateInfo allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};

    VK_RETURN_IF_ERROR(
        context->logger, result,
        vmaCreateImage(context->vma_allocator, &draw_image_create_info, &allocation_create_info, &context->draw_image.image, &context->draw_image.allocation, NULL),
        -1, "Failed to create draw image: %d", result);

    RETURN_IF_ERROR(context->logger, ret,
                    RV_CALL_QUEUE_PUSH_3(context->logger, &context->swapchain_destroy_queue, vmaDestroyImage, context->vma_allocator, context->draw_image.image, context->draw_image.allocation),
                    "Failed to push image to destroy queue: %d", ret);

    VkImageViewCreateInfo draw_image_view_create_info = rv_create_image_view_info(draw_image_format, context->draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_RETURN_IF_ERROR(
        context->logger, result,
        vkCreateImageView(context->device, &draw_image_view_create_info, NULL, &context->draw_image.image_view),
        -1, "Failed to create draw image: %d", result);

    RETURN_IF_ERROR(context->logger, ret,
                    RV_CALL_QUEUE_PUSH_3(context->logger, &context->swapchain_destroy_queue, vkDestroyImageView, context->device, context->draw_image.image_view, NULL),
                    "Failed to push image view to destroy queue: %d", ret);

    context->draw_image.image_format = (RV_VkFormat)draw_image_format;
    context->draw_image.image_extent.width = draw_image_extent.width;
    context->draw_image.image_extent.height = draw_image_extent.height;
    context->draw_image.image_extent.depth = draw_image_extent.depth;

    return 0;
}

#define MAX_POOL_SIZES_LEN 64

TODO("Find a better abstraction for this")
typedef struct DescriptorLayoutBuilder
{
    ARRAY_FIELD(VkDescriptorSetLayoutBinding, bindings, MAX_POOL_SIZES_LEN);
} DescriptorLayoutBuilder;

int32_t add_binding(RendererContext *context, DescriptorLayoutBuilder *builder, uint32_t binding, VkDescriptorType type)
{
    assert(context != NULL);
    assert(builder != NULL);

    VkDescriptorSetLayoutBinding layout_binding = {
        .binding = binding,
        .descriptorCount = 1,
        .descriptorType = type,
    };

    ARRAY_PUSH_CHECKED(
        builder->bindings,
        layout_binding,
        {
            LOG_ERR(context->logger, "Unable to push descriptor set layout binding, max capacity");
            return -1;
        });

    return 0;
}

void clear(DescriptorLayoutBuilder *builder)
{
    assert(builder != NULL);
    GET_ARRAY_LENGTH(builder->bindings) = 0;
}

int32_t build(RendererContext *context, DescriptorLayoutBuilder *builder, VkShaderStageFlags shader_stages, const void *next, VkDescriptorSetLayoutCreateFlags flags, VkDescriptorSetLayout *out_descriptor_set_layout)
{
    assert(context != NULL);
    assert(builder != NULL);
    assert(out_descriptor_set_layout != NULL);

    VkResult result;

    ARRAY_FOR(builder->bindings, i)
    {
        builder->bindings[i].stageFlags |= shader_stages;
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = next,
        .pBindings = builder->bindings,
        .bindingCount = (uint32_t)GET_ARRAY_LENGTH(builder->bindings),
        .flags = flags,
    };

    VK_RETURN_IF_ERROR(context->logger, result,
                       vkCreateDescriptorSetLayout(context->device, &descriptor_set_layout_create_info, NULL, out_descriptor_set_layout),
                       -1, "Unable to create descriptor set layout: %d", result);

    return 0;
}

TODO("Is this struct necessary?")
typedef struct PoolSizeRatio
{
    VkDescriptorType type;
    uint32_t ratio;
} PoolSizeRatio;

int32_t init_pool(RendererContext *context, uint32_t max_sets, PoolSizeRatio *pool_ratios)
{
    assert(context != NULL);
    assert(pool_ratios != NULL);

    VkResult result;

    CREATE_ARRAY(VkDescriptorPoolSize, pool_sizes, MAX_POOL_SIZES_LEN);

    ARRAY_FOR(pool_ratios, i)
    {
        VkDescriptorPoolSize pool_size = {
            .type = pool_ratios[i].type,
            .descriptorCount = (uint32_t)(pool_ratios[i].ratio * max_sets)};

        ARRAY_PUSH_CHECKED(pool_sizes, pool_size, {
            LOG_ERR(context->logger, "Unable to pushn to pool_sizes");
            return -1;
        });
    }

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = max_sets,
        .pPoolSizes = pool_sizes,
        .poolSizeCount = (uint32_t)GET_ARRAY_LENGTH(pool_sizes),
    };

    VK_RETURN_IF_ERROR(context->logger, result, vkCreateDescriptorPool(context->device, &descriptor_pool_create_info, NULL, &context->global_descriptor_pool),
                       -1, "Failed to allocate descriptor pool: %d", result);

    return 0;
}

int32_t clear_descriptors(RendererContext *context)
{
    assert(context != NULL);

    VkResult result;

    VK_RETURN_IF_ERROR(context->logger, result, vkResetDescriptorPool(context->device, context->global_descriptor_pool, 0),
                       -1, "Failed to reset descriptor pool: %d", result);

    return 0;
}

void destroy_pool(RendererContext *context)
{
    assert(context != NULL);

    vkDestroyDescriptorPool(context->device, context->global_descriptor_pool, NULL);
}

int32_t allocate(RendererContext *context, VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet *out_descriptor_set)
{
    assert(context != NULL);
    assert(descriptor_set_layout != VK_NULL_HANDLE);
    assert(out_descriptor_set != NULL);

    VkResult result;

    VkDescriptorSetAllocateInfo descriptor_set_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = context->global_descriptor_pool,
        .pSetLayouts = &descriptor_set_layout,
        .descriptorSetCount = 1,
    };

    VK_RETURN_IF_ERROR(context->logger, result, vkAllocateDescriptorSets(context->device, &descriptor_set_alloc_info, out_descriptor_set),
                       -1, "Failed to allocate descriptor sets: %d", result);

    return 0;
}

void destroy_descriptor_sets(RendererContext *context)
{
    destroy_pool(context);

    vkDestroyDescriptorSetLayout(context->device, context->draw_image_descriptor_set_layout, NULL);
}

int32_t init_descriptor_sets(RendererContext *context)
{
    assert(context != NULL);

    uint32_t ret;

    CREATE_INITIALIZED_ARRAY(
        PoolSizeRatio, pool_size_ratios,
        {
            (PoolSizeRatio){
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .ratio = 1,
            },
        });

    RETURN_IF_ERROR(context->logger, ret, init_pool(context, 10, pool_size_ratios),
                    "Failed to init pool: %d", ret);

    {
        DescriptorLayoutBuilder builder = {0};
        SET_ARRAY_FIELD_CAPACITY(builder.bindings);
        RETURN_IF_ERROR(context->logger, ret, add_binding(context, &builder, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE),
                        "Failed to add binding to descriptor layout builder: %d", ret);

        RETURN_IF_ERROR(context->logger, ret, build(context, &builder, VK_SHADER_STAGE_COMPUTE_BIT, NULL, 0, &context->draw_image_descriptor_set_layout),
                        "Failed to build descriptor layout: %d", ret);
    }

    RETURN_IF_ERROR(context->logger, ret, allocate(context, context->draw_image_descriptor_set_layout, &context->draw_image_descriptor_set),
                    "Failed to allocate descriptor set: %d", ret);

    VkDescriptorImageInfo draw_image_descriptor_info = {
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
        .imageView = context->draw_image.image_view,
    };

    VkWriteDescriptorSet draw_image_write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = 0,
        .dstSet = context->draw_image_descriptor_set,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &draw_image_descriptor_info,
    };

    vkUpdateDescriptorSets(context->device, 1, &draw_image_write_descriptor_set, 0, NULL);

    RETURN_IF_ERROR(context->logger, ret,
                    RV_CALL_QUEUE_PUSH_1(context->logger, &context->main_destroy_queue, destroy_descriptor_sets, context),
                    "Failed to push destroy descriptor sets to destroy queue: %d", ret);

    return 0;
}

int32_t renderer_vulkan_start(RendererContext *context)
{
    assert(context != NULL);

    int32_t ret;
    VK_TRY_INIT(context->logger, ret, renderer_vulkan_bootstrap(context), &context->main_destroy_queue,
                "Failed to bootstrap vulkan: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_command_buffers(context), &context->main_destroy_queue,
                "Failed to init commands: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_sync_structures(context), &context->main_destroy_queue,
                "Failed to create sync structures: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_vma_allocator(context), &context->main_destroy_queue,
                "Failed to create vma allocator: %d", ret);

    VK_TRY_INIT(context->logger, ret, create_draw_image(context), &context->main_destroy_queue,
                "Failed to create draw image: %d", ret);

    VK_TRY_INIT(context->logger, ret, init_descriptor_sets(context), &context->main_destroy_queue,
                "Failed to create draw image: %d", ret);

    return 0;
}

int32_t renderer_vulkan_start_recreate_swapchain(RendererContext *context)
{
    TODO("What needs to happen of fail? delete all queues?")
    assert(context != NULL);
    assert(context->device != VK_NULL_HANDLE);
    assert(context->swapchain != VK_NULL_HANDLE);

    int32_t ret;
    VkResult result;

    VK_RETURN_IF_ERROR(context->logger, result, vkDeviceWaitIdle(context->device),
                       -1, "Failed to wait for device to idle: %d", result);

    rv_call_queue_flush(&context->swapchain_destroy_queue);

    if (context->resize_extent.width == 0 || context->resize_extent.height == 0)
    {
        context->resize_requested = false;
        context->halt_render = true;
        return 0;
    }

    context->halt_render = false;

    RETURN_IF_ERROR(context->logger, ret, renderer_vulkan_bootstrap_recreate_swapchain(context),
                    "Failed to bootstrap recreate swapchain: %d", ret);

    RETURN_IF_ERROR(context->logger, ret, create_draw_image(context),
                    "Failed to recreate draw image: %d", ret);

    context->resize_requested = false;
    return 0;
}