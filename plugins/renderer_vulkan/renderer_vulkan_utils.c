#include "renderer_vulkan_utils.h"

#include <stdint.h>
#define VK_USE_64_BIT_PTR_DEFINES 1
#include <vulkan/vulkan.h>
#include <assert.h>
#include <string.h>

#include <plugin_utils.h>
#include <logger_interface.h>
LOGGER_INTERFACE_REGISTER(renderer_vulkan_utils, LOG_LEVEL_DEBUG)

#include "renderer_vulkan_register.h"

int32_t vk_get_instance_proc(LoggerInterface *logger, VkInstance instance, const char *proc_name, vk_func_void_void *out_func)
{
    assert(logger != NULL);
    assert(instance != NULL);
    assert(proc_name != NULL);
    assert(out_func != NULL);

    RETURN_IF_ERROR_CONDITION_RET_VALUE(
        logger, *out_func, *out_func == NULL,
        (vk_func_void_void)vkGetInstanceProcAddr(instance, proc_name),
        (int32_t)VK_ERROR_EXTENSION_NOT_PRESENT,
        "Unable to find vk instance proc function '%s'", proc_name);

    return 0;
}

void rv_vk_destroy_queue_flush(RV_VkDestroyQueue *queue)
{
    assert(queue != NULL);
    for (size_t i = 0; i < queue->queue_len; i++)
    {
        size_t queue_index = queue->queue_len - i - 1;
        RV_VkDestroyData *destroy_data = &queue->queue[queue_index];

        if (destroy_data->subject == NULL)
        {
            destroy_data->destroy_1(destroy_data->allocator);
            continue;
        }

        TODO("Add destroy 4 when needed");
        if (*destroy_data->subject == VK_NULL_HANDLE)
        {
            continue;
        }

        if (destroy_data->context == NULL)
        {
            destroy_data->destroy_2(*destroy_data->subject, destroy_data->allocator);
        }
        else
        {
            destroy_data->destroy_3(destroy_data->context, *destroy_data->subject, destroy_data->allocator);
        }

        *destroy_data->subject = VK_NULL_HANDLE;
    }
    queue->queue_len = 0;
}

int32_t rv_vk_destroy_queue_push(LoggerInterface *logger, RV_VkDestroyQueue *queue, RV_VkDestroyData *destroy_data)
{
    assert(logger != NULL);
    assert(queue != NULL);
    assert(destroy_data != NULL);

    RETURN_IF_FALSE(logger, queue->queue_len < ARRAY_SIZE(queue->queue),
                    -1, "Failed to add destroy data to queue");

    memcpy(&queue->queue[queue->queue_len], destroy_data, sizeof(queue->queue[0]));
    queue->queue_len += 1;

    return 0;
}