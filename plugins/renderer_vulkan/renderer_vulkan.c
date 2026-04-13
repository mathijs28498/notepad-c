#include "renderer_vulkan.h"

#include <assert.h>
#include <vulkan/vulkan.h>

#include <logger_interface.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan, LOG_LEVEL_DEBUG);

#include "renderer_vulkan_register.h"
#include "renderer_vulkan_bootstrap.h"
#include "renderer_vulkan_utils.h"
#include "renderer_vulkan_start.h"

int32_t renderer_vulkan_cleanup(RendererContext *context)
{
    assert(context != NULL);

    if (context->device != VK_NULL_HANDLE)
    {
        VkResult result;
        VK_RETURN_IF_ERROR(context->logger, result, vkDeviceWaitIdle(context->device),
                           -1, "Failed to wait for device to idle: %d", result);
    }

    rv_vk_destroy_queue_flush(&context->destroy_queue);

    return 0;
}
