#include "draw_default_register.h"

#include <assert.h>

#include <plugin_sdk/draw/v1/draw_interface.h>
#include <plugin_sdk/allocator/v1/allocator_interface.h>
#include <plugin_sdk/plugin_utils.h>

#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(draw_default_register, LOG_LEVEL_DEBUG)

#include "draw_default.h"
#include "draw_default_start.h"

static const DrawVtable plugin_vtable = {
    .start = draw_default_start,
    .present = draw_default_present,
    .on_window_resize = draw_default_on_window_resize,
};

#define MAX_DRAW_RESOURCE_LEN 10

static int32_t plugin_init(DrawContext *context)
{
    assert(context != NULL);

    int32_t ret;

    struct
    {
        INIT_ARRAY_MEMORY_FIELD(resources_mem, DrawResource, MAX_DRAW_RESOURCE_LEN);
    } *allocated_memory;

    RETURN_IF_ERROR(context->deps.logger, ret, allocator_alloc(context->deps.allocator, sizeof(*allocated_memory), &context->allocation_handle, &allocated_memory),
                    "Failed to allocate memory: %d", ret);

    BIND_ARRAY(DrawResource, allocated_memory->resources_mem, context->resources_a, MAX_DRAW_RESOURCE_LEN);

    DrawResource render_image_resource = {
        .name = "render_image",
    };
    ARRAY_PUSH_CHECKED_DEFAULT_RETURN(context->deps.logger, context->resources_a, render_image_resource);
    return 0;
}

static int32_t plugin_shutdown(DrawContext *context)
{
    assert(context != NULL);

    draw_default_cleanup(context);

    return 0;
}

#include "plugin_register.c.inc"