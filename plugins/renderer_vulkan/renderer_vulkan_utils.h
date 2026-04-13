#pragma once

#include <stdint.h>

#include <plugin_utils.h>

#define CREATE_VK_HANDLE_DEFINITION(object) \
    struct object##_T;                      \
    typedef struct object##_T *object;

#define VK_RETURN_IF_ERROR(logger, err_var, func_call, err_ret_val, ...) \
    RETURN_IF_ERROR_CONDITION_RET_VALUE(logger, err_var, ((err_var) < VK_SUCCESS), func_call, err_ret_val, ##__VA_ARGS__)

#define VK_TRY_INIT(logger, err_var, create_func_call, destroy_queue, ...) \
    do                                                                     \
    {                                                                      \
        (err_var) = (create_func_call);                                    \
        if ((err_var) < 0)                                                 \
        {                                                                  \
            LOG_ERR((logger), __VA_ARGS__);                                \
            rv_vk_destroy_queue_flush(destroy_queue);                      \
            return (err_var);                                              \
        }                                                                  \
    } while (0)

struct LoggerInterface;

struct VkInstance_T;
typedef struct VkInstance_T *VkInstance;

typedef void (*vk_func_void_void)(void);

int32_t vk_get_instance_proc(struct LoggerInterface *logger, VkInstance instance, const char *proc_name, vk_func_void_void *out_func);

struct RV_VkDestroyQueue;
struct RV_VkDestroyData;

int32_t rv_vk_destroy_queue_push(struct LoggerInterface *logger, struct RV_VkDestroyQueue *queue, struct RV_VkDestroyData *destroy_data);
void rv_vk_destroy_queue_flush(struct RV_VkDestroyQueue *queue);