#include "renderer_vulkan_buffer.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <string.h>
#include <inttypes.h>

#include <vk_mem_alloc.h>

#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_buffer, LOG_LEVEL_DEBUG)
#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/plugin_utils.h>

#include "renderer_vulkan_register.h"
#include "renderer_vulkan_utils.h"
#include "renderer_vulkan_conversion.h"
#include "renderer_vulkan.h"
#include "renderdoc/renderdoc_app.h"

int32_t renderer_vulkan_create_buffer(RendererContext *context, RendererBufferCreateInfo *renderer_buffer_create_info, RendererBufferHandle *out_buffer_handle)
{
    assert(context != NULL);
    assert(renderer_buffer_create_info != NULL);
    assert(out_buffer_handle != NULL);

    VkResult result;

    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = renderer_buffer_create_info->size,
        .usage = rv_buffer_usage_to_vk_buffer_usage(renderer_buffer_create_info->usage_flags),
    };

    VmaAllocationCreateInfo alloc_create_info = {
        .usage = rv_memory_usage_to_vma_memory_usage(renderer_buffer_create_info->memory_usage),
        .flags = rv_memory_allocation_flags_to_vma_allocation_flags(renderer_buffer_create_info->memory_allocation_flags),
    };

    RV_AllocatedBuffer allocated_buffer;
    VmaAllocationInfo allocation_info;
    RV_RETURN_IF_ERROR(context->deps.logger, result, vmaCreateBuffer(context->vma_allocator, &buffer_create_info, &alloc_create_info, &allocated_buffer.buffer, &allocated_buffer.allocation, &allocation_info),
                       -1, "Failed to create buffer: %d", result);

    RendererVulkanHandle rv_buffer_handle = {0};
    RV_RES_RV_HANDLE_ALLOC_OR_RETURN(context->deps.logger, context->allocated_buffer_occupied_a, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                     allocated_buffer, rv_buffer_handle, vmaDestroyBuffer(context->vma_allocator, allocated_buffer.buffer, allocated_buffer.allocation));

    *out_buffer_handle = rv_buffer_handle.raw;
    return 0;
}

int32_t renderer_vulkan_destroy_buffer(RendererContext *context, RendererBufferHandle buffer_handle)
{
    assert(context != NULL);

    int32_t ret;

    RV_AllocatedBuffer allocated_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a, buffer_handle, allocated_buffer);
    RV_RES_RENDERER_HANDLE_FREE_RETURN_IF_ERROR(context->deps.logger, context->allocated_buffer_occupied_a, context->allocated_buffer_generations_a, context->allocated_buffers_a, buffer_handle);

    RETURN_IF_ERROR(context->deps.logger, ret,
                    RV_CALL_QUEUE_PUSH_3(context->deps.logger, context->active_frame_state.frame->destroy_queue_a, vmaDestroyBuffer, context->vma_allocator, allocated_buffer.buffer, allocated_buffer.allocation),
                    "Failed to push buffer to destroy queue: %d", ret);

    return 0;
}

int32_t renderer_vulkan_upload_buffer_data(RendererContext *context, RendererCommandList *command_list, RendererUploadBufferDataInfo *upload_buffer_data_info)
{
    assert(context != NULL);
    assert(command_list != NULL);
    assert(upload_buffer_data_info != NULL);

    int32_t ret;

    RendererBufferCreateInfo staging_buffer_create_info = {
        .memory_usage = RENDERER_MEMORY_USAGE_CPU_ONLY,
        .size = upload_buffer_data_info->upload_size,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memory_allocation_flags = RENDERER_MEMORY_ALLOCATION_MAPPED_BIT | RENDERER_MEMORY_ALLOCATION_HOST_ACCESS_RANDOM_BIT,
    };

    RendererBufferHandle staging_buffer_handle;
    RETURN_IF_ERROR(context->deps.logger, ret,
                    renderer_vulkan_create_buffer(context, &staging_buffer_create_info, &staging_buffer_handle),
                    "Unable to create staging buffer: %d", ret);

    RV_AllocatedBuffer staging_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         staging_buffer_handle, staging_buffer);

    void *staging_buffer_data;
    vmaMapMemory(context->vma_allocator, staging_buffer.allocation, &staging_buffer_data);

    memcpy(staging_buffer_data, upload_buffer_data_info->upload_data, upload_buffer_data_info->upload_size);

    vmaUnmapMemory(context->vma_allocator, staging_buffer.allocation);

    VkBufferCopy copy_region = {
        .size = upload_buffer_data_info->upload_size,
        .srcOffset = 0,
        .dstOffset = upload_buffer_data_info->destination_offset,
    };

    RV_AllocatedBuffer destination_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         upload_buffer_data_info->destination_buffer_handle, destination_buffer);
    vkCmdCopyBuffer(command_list->command_buffer, staging_buffer.buffer, destination_buffer.buffer, 1, &copy_region);

    uint32_t test[3] = {0};
    RendererReadCPUBufferDataInfo read_cpu_buffer_data = {
        .size = 12,
        .source_buffer_handle = staging_buffer_handle,
        .destination_buffer = test,
    };
    renderer_vulkan_read_cpu_buffer_data(context, &read_cpu_buffer_data);

    RETURN_IF_ERROR(context->deps.logger, ret, renderer_vulkan_destroy_buffer(context, staging_buffer_handle),
                    "Failed to destroy staging buffer: %d", ret);

    return 0;
}

TODO("REMOVE THIS!!");
#include "renderer_vulkan_cmd.h"
int32_t renderer_vulkan_copy_buffer_data(RendererContext *context, RendererCommandList *command_list, RendererCopyBufferDataInfo *copy_buffer_data_info)
{
    assert(context != NULL);
    assert(command_list != NULL);
    assert(copy_buffer_data_info != NULL);

    RV_AllocatedBuffer staging_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         copy_buffer_data_info->destination_buffer_handle, staging_buffer);

    RV_AllocatedBuffer source_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         copy_buffer_data_info->source_buffer_handle, source_buffer);

    VkBufferCopy copy_region = {
        .size = copy_buffer_data_info->size,
        .srcOffset = copy_buffer_data_info->source_offset,
        .dstOffset = copy_buffer_data_info->destination_offset,
    };
    vkCmdCopyBuffer(command_list->command_buffer, source_buffer.buffer, staging_buffer.buffer, 1, &copy_region);

    return 0;
}

int32_t renderer_vulkan_read_cpu_buffer_data(RendererContext *context, RendererReadCPUBufferDataInfo *read_cpu_buffer_data)
{
    assert(context != NULL);
    assert(read_cpu_buffer_data != NULL);

    VkResult result;

    RV_AllocatedBuffer source_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         read_cpu_buffer_data->source_buffer_handle, source_buffer);

    RV_RETURN_IF_ERROR(context->deps.logger, result, vmaInvalidateAllocation(context->vma_allocator, source_buffer.allocation, 0, VK_WHOLE_SIZE),
                       -1, "Failed to invalidate allocation: %d", result);

    void *source_buffer_data;
    vmaMapMemory(context->vma_allocator, source_buffer.allocation, &source_buffer_data);
    memcpy(read_cpu_buffer_data->destination_buffer, source_buffer_data, read_cpu_buffer_data->size);
    vmaUnmapMemory(context->vma_allocator, source_buffer.allocation);

    return 0;
}

int32_t renderer_vulkan_get_buffer_device_address(RendererContext *context, RendererBufferHandle buffer_handle, RendererBufferDeviceAddress *out_device_address)
{
    assert(context != NULL);
    assert(out_device_address != NULL);

    RV_AllocatedBuffer allocated_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a,
                                         buffer_handle, allocated_buffer);

    VkBufferDeviceAddressInfo device_address_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = allocated_buffer.buffer,
    };

    *out_device_address = vkGetBufferDeviceAddress(context->device, &device_address_info);

    return 0;
}