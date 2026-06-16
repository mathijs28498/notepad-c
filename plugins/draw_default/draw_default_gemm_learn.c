#include "draw_default_gemm_learn.h"

#include <stdint.h>
#include <assert.h>
#include <inttypes.h>

#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(draw_default_gemm_learn, LOG_LEVEL_DEBUG)
#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/plugin_utils.h>

#include "draw_default_register.h"

typedef struct SimpleCopyExampleData
{
    LoggerInterface *logger;
    RendererInterface *renderer;
    RendererUploadBufferDataInfo *upload_data_info;
    RendererCopyBufferDataInfo *copy_data_info;
} SimpleCopyExampleData;

int32_t simple_copy_example_callback(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    SimpleCopyExampleData *simple_copy_example_data = (SimpleCopyExampleData *)user_data;

    LoggerInterface *logger = simple_copy_example_data->logger;
    RendererInterface *renderer = simple_copy_example_data->renderer;

    uint32_t ret;

    RETURN_IF_ERROR(logger, ret, renderer_upload_buffer_data(renderer, command_list, simple_copy_example_data->upload_data_info),
                    "Failed to upload buffer data: %s", ret);

    RendererBufferMemoryBarrier copy_memory_barrier = {
        .src_access_mask = RENDERER_ACCESS_TRANSFER_WRITE_BIT,
        .dst_access_mask = RENDERER_ACCESS_TRANSFER_READ_BIT,
        .buffer = simple_copy_example_data->copy_data_info->source_buffer_handle,
        .offset = 0,
        .size = RENDERER_WHOLE_SIZE,
    };

    RendererBarrierInfo copy_barrier_info = {
        .src_stage_mask = RENDERER_PIPELINE_STAGE_TRANSFER_BIT,
        .dst_stage_mask = RENDERER_PIPELINE_STAGE_TRANSFER_BIT,
        .buffer_memory_barrier_len = 1,
        .buffer_memory_barriers = &copy_memory_barrier,
    };

    RETURN_IF_ERROR(logger, ret, renderer_cmd_barrier(renderer, command_list, &copy_barrier_info),
                    "Failed to set copy barrier: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_copy_buffer_data(renderer, command_list, simple_copy_example_data->copy_data_info),
                    "Failed to copy buffer data: %s", ret);

    RendererBufferMemoryBarrier host_memory_barrier = {
        .src_access_mask = RENDERER_ACCESS_TRANSFER_WRITE_BIT,
        .dst_access_mask = RENDERER_ACCESS_HOST_READ_BIT,
        .buffer = simple_copy_example_data->copy_data_info->destination_buffer_handle,
        .offset = 0,
        .size = RENDERER_WHOLE_SIZE,
    };

    RendererBarrierInfo host_barrier_info = {
        .src_stage_mask = RENDERER_PIPELINE_STAGE_TRANSFER_BIT,
        .dst_stage_mask = RENDERER_PIPELINE_STAGE_HOST_BIT,
        .buffer_memory_barrier_len = 1,
        .buffer_memory_barriers = &host_memory_barrier,
    };

    RETURN_IF_ERROR(logger, ret, renderer_cmd_barrier(renderer, command_list, &host_barrier_info),
                    "Failed to set host barrier: %d", ret);

    return 0;
}

int32_t simple_copy_example(DrawContext *context)
{
    assert(context != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;
    int32_t ret;

    CREATE_INITIALIZED_ARRAY(
        float,
        buffer_data_a,
        {78.5f, 54568.097f, 0.00056f});

    size_t buffer_data_size_bytes = GET_ARRAY_LENGTH_BYTES(buffer_data_a);

    RendererBufferHandle buffer_handle;
    RendererBufferCreateInfo buffer_create_info = {
        .size = buffer_data_size_bytes,
        .memory_usage = RENDERER_MEMORY_USAGE_GPU_ONLY,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_DST_BIT | RENDERER_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };

    RETURN_IF_ERROR(logger, ret, renderer_create_buffer(renderer, &buffer_create_info, &buffer_handle),
                    "Unable to create buffer: %d", ret);

    RendererBufferCreateInfo staging_buffer_create_info = {
        .memory_usage = RENDERER_MEMORY_USAGE_CPU_ONLY,
        .size = buffer_data_size_bytes,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_DST_BIT,
        .memory_allocation_flags = RENDERER_MEMORY_ALLOCATION_MAPPED_BIT | RENDERER_MEMORY_ALLOCATION_HOST_ACCESS_RANDOM_BIT,
    };

    RendererBufferHandle staging_buffer_handle;
    RETURN_IF_ERROR(logger, ret,
                    renderer_create_buffer(renderer, &staging_buffer_create_info, &staging_buffer_handle),
                    "Unable to create staging buffer: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, buffer_handle, "gpu_buffer"),
                    "Failed to rename gpu buffer: %d", ret);
    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, staging_buffer_handle, "read_staging_buffer"),
                    "Failed to rename read staging buffer buffer: %d", ret);

    RendererUploadBufferDataInfo upload_data_info = {
        .destination_buffer_handle = buffer_handle,
        .upload_data = buffer_data_a,
        .upload_size = buffer_data_size_bytes,
    };

    RendererCopyBufferDataInfo copy_data_info = {
        .source_buffer_handle = buffer_handle,
        .destination_buffer_handle = staging_buffer_handle,
        .size = buffer_data_size_bytes,
    };

    SimpleCopyExampleData simple_copy_example_data = {
        .logger = logger,
        .renderer = renderer,
        .upload_data_info = &upload_data_info,
        .copy_data_info = &copy_data_info,
    };

    RETURN_IF_ERROR(logger, ret, renderer_immediate_execute(renderer, simple_copy_example_callback, &simple_copy_example_data),
                    "Failed to simple copy example execute: %s", ret);

    RETURN_IF_ERROR(logger, ret, renderer_immediate_flush(renderer),
                    "Failed to flush immediate execution: %s", ret);

    float values[3] = {0};

    RendererReadCPUBufferDataInfo read_cpu_buffer_data_info = {
        .size = buffer_data_size_bytes,
        .source_buffer_handle = staging_buffer_handle,
        .source_offset = 0,
        .destination_buffer = values,
    };

    RETURN_IF_ERROR(logger, ret, renderer_read_cpu_buffer_data(renderer, &read_cpu_buffer_data_info),
                    "Failed to read cpu buffer data: %s", ret);

    // LOG_INF_TRACE(logger, "Values after copy: %" PRIu32", %" PRIu32 ", %" PRIu32 "", values[0], values[1], values[2]);
    LOG_INF_TRACE(logger, "Values after copy: %.6f, %.6f, %.6f", values[0], values[1], values[2]);

    RETURN_IF_ERROR(logger, ret, renderer_destroy_buffer(renderer, staging_buffer_handle),
                    "Unable to destroy buffer: %s", ret);

    RETURN_IF_ERROR(logger, ret, renderer_destroy_buffer(renderer, buffer_handle),
                    "Unable to destroy buffer: %s", ret);

    return 0;
}

int32_t draw_default_gemm_execute(DrawContext *context)
{
    assert(context != NULL);

    LoggerInterface *logger = context->deps.logger;
    int32_t ret;

    RETURN_IF_ERROR(logger, ret, simple_copy_example(context),
                    "Failed simple copy example: %d", ret);

    return 0;
}