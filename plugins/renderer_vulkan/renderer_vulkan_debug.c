#include "renderer_vulkan_debug.h"

#include <stdint.h>
#include <renderdoc/renderdoc_app.h>
#include <vulkan/vulkan.h>

#include <plugin_sdk/renderer/v1/renderer_interface.h>
#include <plugin_sdk/plugin_utils.h>
#include <plugin_sdk/logger/v1/logger_interface.h>
#include <plugin_sdk/logger/v1/logger_interface_macros.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_debug, LOG_LEVEL_DEBUG)

TODO("Move this to a separate windows only part")
#include <Windows.h>

#include "renderer_vulkan_register.h"
#include "renderer_vulkan_utils.h"
#include "renderer_vulkan.h"

int32_t renderer_vulkan_debug_init(RendererContext *context)
{
    assert(context != NULL);

    int32_t ret;

    HMODULE mod = GetModuleHandleA("renderdoc.dll");
    if (!mod)
    {
        LOG_WRN_TRACE(context->deps.logger, "Launched without renderdoc");
    }
    else
    {
        LOG_WRN(context->deps.logger, "Renderdoc successfully loaded");
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
            (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_7_0, (void **)&context->debug_context.renderdoc_api);
    }

    RETURN_IF_ERROR(
        context->deps.logger, ret,
        rv_get_instance_proc(context->deps.logger, context->instance, "vkSetDebugUtilsObjectNameEXT", (void **)&context->debug_context.set_object_name_fn),
        "Could not get instance proc: %d", ret);

    return 0;
}

void renderer_vulkan_debug_begin_capture(RendererContext *context)
{
    assert(context != NULL);

    if (context->debug_context.renderdoc_api == NULL)
    {
        LOG_WRN_TRACE(context->deps.logger, "Calling debug start capture with no renderdoc api found");
        return;
    }

    context->debug_context.renderdoc_api->StartFrameCapture(NULL, NULL);
}

void renderer_vulkan_debug_end_capture(RendererContext *context)
{
    assert(context != NULL);

    if (context->debug_context.renderdoc_api == NULL)
    {
        LOG_WRN_TRACE(context->deps.logger, "Calling debug end capture with no renderdoc api found");
        return;
    }

    context->debug_context.renderdoc_api->EndFrameCapture(NULL, NULL);
}

int32_t renderer_vulkan_debug_rename_buffer(RendererContext *context, RendererBufferHandle buffer_handle, const char *name)
{
    assert(context != NULL);
    assert(context->debug_context.set_object_name_fn != NULL);

    VkResult result;

    PFN_vkSetDebugUtilsObjectNameEXT set_object_name_fn = (PFN_vkSetDebugUtilsObjectNameEXT)context->debug_context.set_object_name_fn;

    RV_AllocatedBuffer allocated_buffer = {0};
    RV_RES_RENDERER_HANDLE_GET_OR_RETURN(context->deps.logger, context->allocated_buffer_generations_a, context->allocated_buffers_a, buffer_handle, allocated_buffer);

    VkDebugUtilsObjectNameInfoEXT name_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = VK_OBJECT_TYPE_BUFFER,
        .objectHandle = (uint64_t)allocated_buffer.buffer,
        .pObjectName = name,
    };

    RV_RETURN_IF_ERROR(context->deps.logger, result, set_object_name_fn(context->device, &name_info),
                       -1, "Failed to set buffer name: %d", result);

    return 0;
}