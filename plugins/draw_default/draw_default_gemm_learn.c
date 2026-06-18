#include "draw_default_gemm_learn.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>

#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER_URGENCY(draw_default_gemm_learn, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG)
#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/allocator/v1/allocator_interface.h>
#include <plugin_sdk/plugin_utils.h>

#include "draw_default_register.h"

#include "shader_gemm_naive_compute.h"
#include "shader_gemm_shared_compute.h"

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

    RETURN_IF_ERROR(logger, ret, renderer_cmd_copy_buffer_data(renderer, command_list, simple_copy_example_data->copy_data_info),
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

typedef struct GemmLayout
{
    RendererResourceSetLayoutHandle resource_set_layout_handle;
    RendererPipelineLayoutHandle pipeline_layout_handle;
} GemmLayout;

typedef struct GemmBuffers
{
    RendererBufferHandle a_handle;
    uint64_t a_size;
    RendererBufferHandle b_handle;
    uint64_t b_size;
    RendererBufferHandle c_handle;
    uint64_t c_size;
    RendererBufferHandle read_staging_handle;
    uint64_t read_staging_size;
} GemmBuffers;

typedef struct GemmComputeData
{
    LoggerInterface *logger;
    RendererInterface *renderer;

    GemmLayout layout;
    GemmBuffers buffers;

    RendererComputePipelineHandle pipeline_handle;

} GemmComputeData;

#define MATRIX_WIDTH 1024
#define MATRIX_BUFFER_SIZE MATRIX_WIDTH *MATRIX_WIDTH
#define MATRIX_BUFFER_BYTE_SIZE MATRIX_BUFFER_SIZE * sizeof(float)

#define SHADER_WORK_GROUP_SIZE 16
static float buffer_a_data[MATRIX_BUFFER_SIZE] = {0};
static float buffer_b_data[MATRIX_BUFFER_SIZE] = {0};
static float buffer_c_data[MATRIX_BUFFER_SIZE] = {0};
// static float *buffer_a_data = {0};
// static float *buffer_b_data = {0};
// static float *buffer_c_data = {0};

// https://matrixcalc.org/#%7B%7B51,451,81,145%7D,%7B71,5,145,1%7D,%7B41,41,1,41%7D,%7B1556,1,111,1%7D%7D*%7B%7B0,10,20,30%7D,%7B40,50,60,70%7D,%7B80,90,100,110%7D,%7B120,130,140,150%7D%7D
int32_t fill_gemm_buffers_callback(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    GemmComputeData *data = (GemmComputeData *)user_data;

    LoggerInterface *logger = data->logger;
    RendererInterface *renderer = data->renderer;
    int32_t ret;

    for (size_t i = 0; i < MATRIX_BUFFER_SIZE; i++)
    {
        buffer_a_data[i] = (float)(rand() % 100) / 10.0f;
        buffer_b_data[i] = (float)(rand() % 100) / 10.0f;
    }

    RendererUploadBufferDataInfo buffer_a_upload_data = {
        .upload_data = buffer_a_data,
        .upload_size = data->buffers.a_size,
        .destination_buffer_handle = data->buffers.a_handle,
    };
    RETURN_IF_ERROR(logger, ret, renderer_upload_buffer_data(renderer, command_list, &buffer_a_upload_data),
                    "Failed to upload data to buffer a: %d", ret);

    RendererUploadBufferDataInfo buffer_b_upload_data = {
        .upload_data = buffer_b_data,
        .upload_size = data->buffers.b_size,
        .destination_buffer_handle = data->buffers.b_handle,
    };
    RETURN_IF_ERROR(logger, ret, renderer_upload_buffer_data(renderer, command_list, &buffer_b_upload_data),
                    "Failed to upload data to buffer b: %d", ret);

    return 0;
}

int32_t allocate_and_bind_gemm_resource_buffers(LoggerInterface *logger, RendererInterface *renderer, RendererCommandList *command_list, GemmLayout *layout, GemmBuffers *buffers)
{
    assert(logger != NULL);
    assert(renderer != NULL);
    assert(command_list != NULL);
    assert(layout != NULL);
    assert(buffers != NULL);

    int32_t ret;

    RendererResourceSetHandle resource_set_handle;
    RETURN_IF_ERROR(logger, ret, renderer_allocate_transient_resource_set(renderer, layout->resource_set_layout_handle, &resource_set_handle),
                    "Failed to allocate transient resource set: %d", ret);

    RendererResourceBufferBinding resource_buffer_binding_0 = {
        .offset = 0,
        .size = buffers->a_size,
        .buffer_handle = buffers->a_handle,
    };
    RendererResourceBufferBinding resource_buffer_binding_1 = {
        .offset = 0,
        .size = buffers->b_size,
        .buffer_handle = buffers->b_handle,
    };
    RendererResourceBufferBinding resource_buffer_binding_2 = {
        .offset = 0,
        .size = buffers->c_size,
        .buffer_handle = buffers->c_handle,
    };

    RendererResourceSetWrite resource_set_writes[] = {
        {
            .binding = 0,
            .first_resource = 0,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .resource_bindings_len = 1,
            .buffer_bindings = &resource_buffer_binding_0,
        },
        {
            .binding = 1,
            .first_resource = 0,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .resource_bindings_len = 1,
            .buffer_bindings = &resource_buffer_binding_1,
        },
        {
            .binding = 2,
            .first_resource = 0,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .resource_bindings_len = 1,
            .buffer_bindings = &resource_buffer_binding_2,
        },
    };

    RendererResourceSetUpdateInfo resource_set_update_info = {
        .resource_set_handle = resource_set_handle,
        .resource_set_writes_len = ARRAY_SIZE(resource_set_writes),
        .resource_set_writes = resource_set_writes,
    };

    renderer_update_resource_set(renderer, &resource_set_update_info);

    renderer_cmd_bind_resource_sets(renderer, command_list, RENDERER_PIPELINE_TYPE_COMPUTE, layout->pipeline_layout_handle, 0, 1, &resource_set_handle, 0, NULL);

    return 0;
}

int32_t gemm_compute_callback(RendererCommandList *command_list, void *user_data)
{
    assert(command_list != NULL);
    assert(user_data != NULL);

    GemmComputeData *data = (GemmComputeData *)user_data;
    LoggerInterface *logger = data->logger;
    RendererInterface *renderer = data->renderer;
    uint32_t ret;

    renderer_cmd_bind_compute_pipeline(renderer, command_list, data->pipeline_handle);

    RETURN_IF_ERROR(logger, ret, allocate_and_bind_gemm_resource_buffers(logger, renderer, command_list, &data->layout, &data->buffers),
                    "Failed to allocate and bind resource buffers for naive gemm: %d", ret);

    uint32_t matrix_width = MATRIX_WIDTH;
    renderer_cmd_push_constants(renderer, command_list, data->layout.pipeline_layout_handle, RENDERER_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uint32_t), &matrix_width);

    uint32_t group_count_x = (MATRIX_WIDTH + SHADER_WORK_GROUP_SIZE - 1) / SHADER_WORK_GROUP_SIZE;
    uint32_t group_count_y = (MATRIX_WIDTH + SHADER_WORK_GROUP_SIZE - 1) / SHADER_WORK_GROUP_SIZE;
    renderer_cmd_dispatch(renderer, command_list, group_count_x, group_count_y, 1);

    RendererBufferMemoryBarrier compute_memory_barrier = {
        .src_access_mask = RENDERER_ACCESS_SHADER_WRITE_BIT,
        .dst_access_mask = RENDERER_ACCESS_TRANSFER_READ_BIT,
        .buffer = data->buffers.c_handle,
        .offset = 0,
        .size = data->buffers.c_size,
    };

    RendererBarrierInfo compute_barrier_info = {
        .src_stage_mask = RENDERER_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        .dst_stage_mask = RENDERER_PIPELINE_STAGE_TRANSFER_BIT,
        .buffer_memory_barrier_len = 1,
        .buffer_memory_barriers = &compute_memory_barrier,
    };

    RETURN_IF_ERROR(logger, ret, renderer_cmd_barrier(renderer, command_list, &compute_barrier_info),
                    "Failed to barrier: %d", ret);

    RendererCopyBufferDataInfo copy_info = {
        .source_buffer_handle = data->buffers.c_handle,
        .source_offset = 0,
        .size = data->buffers.c_size,
        .destination_buffer_handle = data->buffers.read_staging_handle,
        .destination_offset = 0,
    };
    RETURN_IF_ERROR(logger, ret, renderer_cmd_copy_buffer_data(renderer, command_list, &copy_info),
                    "Failed to copy shader output to read staging buffer: %d", ret);

    RendererBufferMemoryBarrier host_memory_barrier = {
        .src_access_mask = RENDERER_ACCESS_TRANSFER_WRITE_BIT,
        .dst_access_mask = RENDERER_ACCESS_HOST_READ_BIT,
        .buffer = data->buffers.read_staging_handle,
        .offset = 0,
        .size = data->buffers.read_staging_size,
    };

    RendererBarrierInfo host_barrier_info = {
        .src_stage_mask = RENDERER_PIPELINE_STAGE_TRANSFER_BIT,
        .dst_stage_mask = RENDERER_PIPELINE_STAGE_HOST_BIT,
        .buffer_memory_barrier_len = 1,
        .buffer_memory_barriers = &host_memory_barrier,
    };

    RETURN_IF_ERROR(logger, ret, renderer_cmd_barrier(renderer, command_list, &host_barrier_info),
                    "Failed to barrier: %d", ret);

    return 0;
}

int32_t create_gemm_layout(DrawContext *context, GemmLayout *out_layout)
{
    assert(context != NULL);
    assert(out_layout != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;
    int32_t ret;

    RendererResourceSetLayoutBinding resource_set_layout_bindings[] = {
        {
            .binding = 0,
            .resource_len = 1,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .stage_flags = RENDERER_SHADER_STAGE_COMPUTE_BIT,
        },
        {
            .binding = 1,
            .resource_len = 1,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .stage_flags = RENDERER_SHADER_STAGE_COMPUTE_BIT,
        },
        {
            .binding = 2,
            .resource_len = 1,
            .resource_type = RENDERER_RESOURCE_TYPE_STORAGE_BUFFER,
            .stage_flags = RENDERER_SHADER_STAGE_COMPUTE_BIT,
        },
    };

    RendererResourceSetLayoutCreateInfo resource_set_layout_create_info = {
        .bindings_len = ARRAY_SIZE(resource_set_layout_bindings),
        .bindings = resource_set_layout_bindings,
    };

    RETURN_IF_ERROR(logger, ret, renderer_create_resource_set_layout(renderer, &resource_set_layout_create_info, &out_layout->resource_set_layout_handle),
                    "Failed to create naive gemm resource set layout: %d", ret);

    RendererPushConstantsInfo push_constant_info = {
        .offset = 0,
        .render_stage_flags = RENDERER_SHADER_STAGE_COMPUTE_BIT,
        .size = sizeof(uint32_t),
    };

    RendererPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .resource_set_layout_handles_len = 1,
        .resource_set_layout_handles = &out_layout->resource_set_layout_handle,
        .push_constants_len = 1,
        .push_constants = &push_constant_info,
    };

    RETURN_IF_ERROR(logger, ret, renderer_create_pipeline_layout(renderer, &pipeline_layout_create_info, &out_layout->pipeline_layout_handle),
                    "Failed to create naive gemm compute pipeline layout: %d", ret);

    return 0;
}

int32_t create_gemm_buffers(DrawContext *context, GemmBuffers *out_buffers)
{
    assert(context != NULL);
    assert(out_buffers != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;
    int32_t ret;

    out_buffers->a_size = MATRIX_BUFFER_BYTE_SIZE;
    out_buffers->b_size = MATRIX_BUFFER_BYTE_SIZE;
    out_buffers->c_size = MATRIX_BUFFER_BYTE_SIZE;
    out_buffers->read_staging_size = MATRIX_BUFFER_BYTE_SIZE;

    RendererBufferCreateInfo shader_input_buffer_create_info = {
        .memory_usage = RENDERER_MEMORY_USAGE_GPU_ONLY,
        .size = MATRIX_BUFFER_BYTE_SIZE,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_DST_BIT | RENDERER_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    };
    RETURN_IF_ERROR(logger, ret, renderer_create_buffer(renderer, &shader_input_buffer_create_info, &out_buffers->a_handle),
                    "Failed to create buffer a: %d", ret);
    RETURN_IF_ERROR(logger, ret, renderer_create_buffer(renderer, &shader_input_buffer_create_info, &out_buffers->b_handle),
                    "Failed to create buffer a: %d", ret);

    RendererBufferCreateInfo shader_output_buffer_create_info = {
        .memory_usage = RENDERER_MEMORY_USAGE_GPU_ONLY,
        .size = MATRIX_BUFFER_BYTE_SIZE,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_SRC_BIT | RENDERER_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    };
    RETURN_IF_ERROR(logger, ret, renderer_create_buffer(renderer, &shader_output_buffer_create_info, &out_buffers->c_handle),
                    "Failed to create buffer a: %d", ret);

    RendererBufferCreateInfo read_staging_buffer_create_info = {
        .memory_usage = RENDERER_MEMORY_USAGE_CPU_ONLY,
        .size = MATRIX_BUFFER_BYTE_SIZE,
        .usage_flags = RENDERER_BUFFER_USAGE_TRANSFER_DST_BIT,
        .memory_allocation_flags = RENDERER_MEMORY_ALLOCATION_MAPPED_BIT | RENDERER_MEMORY_ALLOCATION_HOST_ACCESS_RANDOM_BIT,
    };
    RETURN_IF_ERROR(logger, ret, renderer_create_buffer(renderer, &read_staging_buffer_create_info, &out_buffers->read_staging_handle),
                    "Failed to create read staging buffer: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, out_buffers->a_handle, "buffer a"),
                    "Failed to rename buffer: %d", ret);
    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, out_buffers->b_handle, "buffer b"),
                    "Failed to rename buffer: %d", ret);
    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, out_buffers->c_handle, "buffer c"),
                    "Failed to rename buffer: %d", ret);
    RETURN_IF_ERROR(logger, ret, renderer_debug_rename_buffer(renderer, out_buffers->read_staging_handle, "buffer read staging"),
                    "Failed to rename buffer: %d", ret);
    return 0;
}

int32_t create_naive_gemm_pipeline(DrawContext *context, RendererPipelineLayoutHandle pipeline_layout_handle, RendererComputePipelineHandle *out_pipeline_handle)
{
    assert(context != NULL);
    assert(out_pipeline_handle != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    int32_t ret;

    RendererShaderHandle compute_shader_handle;
    RETURN_IF_ERROR(logger, ret, renderer_create_shader(renderer, GEMM_NAIVE_COMPUTE_SHADER_U32_CODE, GEMM_NAIVE_COMPUTE_SHADER_BYTES_LEN, &compute_shader_handle),
                    "Failed to create naive gemm shader: %d", ret);

    RendererComputePipelineCreateInfo pipeline_create_info = {
        .compute_shader = {
            .entry_point = "main",
            .shader_handle = compute_shader_handle,
        },
        .layout_handle = pipeline_layout_handle,
    };
    RETURN_IF_ERROR(logger, ret, renderer_create_compute_pipeline(renderer, &pipeline_create_info, out_pipeline_handle),
                    "Failed to create naive gemm compute pipeline: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_destroy_shader(renderer, compute_shader_handle),
                    "Failed to destroy shader: %d", ret);
    return 0;
}

int32_t create_shared_gemm_pipeline(DrawContext *context, RendererPipelineLayoutHandle pipeline_layout_handle, RendererComputePipelineHandle *out_pipeline_handle)
{
    assert(context != NULL);
    assert(out_pipeline_handle != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;

    int32_t ret;

    RendererShaderHandle compute_shader_handle;
    RETURN_IF_ERROR(logger, ret, renderer_create_shader(renderer, GEMM_SHARED_COMPUTE_SHADER_U32_CODE, GEMM_SHARED_COMPUTE_SHADER_BYTES_LEN, &compute_shader_handle),
                    "Failed to create naive gemm shader: %d", ret);

    RendererComputePipelineCreateInfo pipeline_create_info = {
        .compute_shader = {
            .entry_point = "main",
            .shader_handle = compute_shader_handle,
        },
        .layout_handle = pipeline_layout_handle,
    };
    RETURN_IF_ERROR(logger, ret, renderer_create_compute_pipeline(renderer, &pipeline_create_info, out_pipeline_handle),
                    "Failed to create naive gemm compute pipeline: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_destroy_shader(renderer, compute_shader_handle),
                    "Failed to destroy shader: %d", ret);
    return 0;
}

void gemm_teardown(DrawContext *context, GemmComputeData *data)
{
    assert(context != NULL);
    assert(data != NULL);

    RendererInterface *renderer = context->deps.renderer;

    (void)renderer_destroy_buffer(renderer, data->buffers.read_staging_handle);
    (void)renderer_destroy_buffer(renderer, data->buffers.c_handle);
    (void)renderer_destroy_buffer(renderer, data->buffers.b_handle);
    (void)renderer_destroy_buffer(renderer, data->buffers.a_handle);
    (void)renderer_destroy_compute_pipeline(renderer, data->pipeline_handle);
    (void)renderer_destroy_pipeline_layout(renderer, data->layout.pipeline_layout_handle);
    (void)renderer_destroy_resource_set_layout(renderer, data->layout.resource_set_layout_handle);
}

bool float_compare(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

bool verify_matrix_multiplication_freivalds(DrawContext *context, float *buffer_a, float *buffer_b, float *buffer_c, uint32_t matrix_side, uint32_t iterations)
{
    void *buf;
    size_t individual_buf_size = matrix_side * sizeof(float);
    AllocatorAllocationHandle alloc_handle;
    allocator_alloc(context->deps.allocator, individual_buf_size * 4, &alloc_handle, &buf);

    uintptr_t buf_ptr = (uintptr_t)buf;

    float *r = (float *)buf;
    float *Br = (float *)(buf_ptr + individual_buf_size);
    float *ABr = (float *)(buf_ptr + individual_buf_size * 2);
    float *Cr = (float *)(buf_ptr + individual_buf_size * 3);

    bool is_correct = true;

    // Run a few times to eliminate the tiny chance of a false positive
    for (uint32_t iter = 0; iter < iterations; iter++)
    {
        // 1. Generate random 0/1 vector
        for (uint32_t i = 0; i < matrix_side; i++)
        {
            r[i] = (rand() % 2 == 0) ? 0.0f : 1.0f;
        }

        // 2. Compute Br = B * r
        for (uint32_t i = 0; i < matrix_side; i++)
        {
            Br[i] = 0.0f;
            for (uint32_t j = 0; j < matrix_side; j++)
            {
                Br[i] += buffer_b[i * matrix_side + j] * r[j];
            }
        }

        // 3. Compute ABr = A * (Br)
        for (uint32_t i = 0; i < matrix_side; i++)
        {
            ABr[i] = 0.0f;
            for (uint32_t j = 0; j < matrix_side; j++)
            {
                ABr[i] += buffer_a[i * matrix_side + j] * Br[j];
            }
        }

        // 4. Compute Cr = C * r
        for (uint32_t i = 0; i < matrix_side; i++)
        {
            Cr[i] = 0.0f;
            for (uint32_t j = 0; j < matrix_side; j++)
            {
                Cr[i] += buffer_c[i * matrix_side + j] * r[j];
            }
        }

        // 5. Compare ABr and Cr
        for (uint32_t i = 0; i < matrix_side; i++)
        {
            float relative_epsilon = fmaxf(0.001f, fabsf(Cr[i]) * 0.0001f);

            if (fabs(ABr[i] - Cr[i]) > relative_epsilon)
            {
                LOG_ERR(context->deps.logger, "Mismatch at index %d! CPU: %f, GPU: %f", i, ABr[i], Cr[i]);
                is_correct = false;
                break;
            }
        }

        if (!is_correct)
            break;
    }

    allocator_free(context->deps.allocator, alloc_handle);
    return is_correct;
}

bool test_matrix_result(DrawContext *context)
{
    assert(context != NULL);

    return verify_matrix_multiplication_freivalds(context, buffer_a_data, buffer_b_data, buffer_c_data, MATRIX_WIDTH, 5);
}

// bool test_matrix_result_(DrawContext *context, float *values)
// {
//     assert(context != NULL);
//     assert(values != NULL);

//     float correct_values[] =
//         {
//             41920,
//             49200,
//             56480,
//             63760,
//             11920,
//             14140,
//             16360,
//             18580,
//             6640,
//             7880,
//             9120,
//             10360,
//             9040,
//             25730,
//             42420,
//             59110,
//         };

//     float compare_epsilon = 0.0001f;
//     for (uint32_t i = 0; i < MATRIX_BUFFER_SIZE; i++)
//     {
//         if (!float_compare(values[i], correct_values[i], compare_epsilon))
//         {
//             return false;
//         }
//     }

//     return true;
// }

int32_t gemm_compute_example(DrawContext *context, int32_t (*create_pipeline_fn)(DrawContext *, RendererPipelineLayoutHandle, RendererComputePipelineHandle *))
{
    assert(context != NULL);

    LoggerInterface *logger = context->deps.logger;
    RendererInterface *renderer = context->deps.renderer;
    int32_t ret;

    renderer_debug_begin_capture(renderer);
    GemmLayout gemm_layout = {0};
    GemmBuffers gemm_buffers = {0};

    RETURN_IF_ERROR(logger, ret, create_gemm_layout(context, &gemm_layout),
                    "Failed to create gemm layout: %d", ret);

    RETURN_IF_ERROR(logger, ret, create_gemm_buffers(context, &gemm_buffers),
                    "Failed to create gemm buffers: %d", ret);

    GemmComputeData naive_gemm_data = {
        .logger = context->deps.logger,
        .renderer = context->deps.renderer,
        .layout = gemm_layout,
        .buffers = gemm_buffers,
    };
    RETURN_IF_ERROR(logger, ret, create_pipeline_fn(context, gemm_layout.pipeline_layout_handle, &naive_gemm_data.pipeline_handle),
                    "Failed to create naive gemm pipeline: %d", ret);

    RETURN_IF_ERROR(logger, ret, renderer_immediate_execute(renderer, fill_gemm_buffers_callback, &naive_gemm_data),
                    "Failed to immediate execute naive setup: %d", ret);

    if (ret < 0)
    {
        LOG_ERR_TRACE(logger, "Failed to setup naive gemm example: %d", ret);
    }

    if (ret >= 0)
    {
        RETURN_IF_ERROR(logger, ret, renderer_immediate_execute(renderer, gemm_compute_callback, &naive_gemm_data),
                        "Failed to execute naive gemm immediate execute: %d", ret);
        RETURN_IF_ERROR(logger, ret, renderer_immediate_flush(renderer),
                        "Failed to flush immediate execute: %d", ret);

        RendererReadCPUBufferDataInfo read_cpu_buffer_data_info = {
            .source_buffer_handle = gemm_buffers.read_staging_handle,
            .source_offset = 0,
            .size = MATRIX_BUFFER_SIZE * sizeof(float),
            .destination_buffer = buffer_c_data,
        };
        RETURN_IF_ERROR(logger, ret, renderer_read_cpu_buffer_data(renderer, &read_cpu_buffer_data_info),
                        "Failed to read cpu buffer data: %d", ret);
    }
    renderer_debug_end_capture(renderer);

    if (test_matrix_result(context))
    {
        LOG_INF(logger, "Correct calculation!!!");
    }
    else
    {
        LOG_ERR(logger, "Calculation is wrong and you should feel bad!");
    }

    gemm_teardown(context, &naive_gemm_data);

    return 0;
}

int32_t draw_default_gemm_execute(DrawContext *context)
{
    assert(context != NULL);

    LoggerInterface *logger = context->deps.logger;
    int32_t ret;

    // RETURN_IF_ERROR(logger, ret, simple_copy_example(context),
    //                 "Failed simple copy example: %d", ret);

    RETURN_IF_ERROR(logger, ret, gemm_compute_example(context, create_naive_gemm_pipeline),
                    "Failed naive gemm example: %d", ret);

    RETURN_IF_ERROR(logger, ret, gemm_compute_example(context, create_shared_gemm_pipeline),
                    "Failed shared gemm example: %d", ret);

    return 0;
}