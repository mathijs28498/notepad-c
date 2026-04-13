#pragma once

#include <plugin_utils.h>

#include "plugin_dependencies.h"
#include "renderer_vulkan_utils.h"

#pragma pack(push, 8)

#define MAX_RV_VK_DESTROY_QUEUE_LEN 256

#define FRAMES_LEN 2
#define MAX_SWAPCHAIN_IMAGES_LEN 4

CREATE_VK_HANDLE_DEFINITION(VkDebugUtilsMessengerEXT);
CREATE_VK_HANDLE_DEFINITION(VkInstance);
CREATE_VK_HANDLE_DEFINITION(VkSurfaceKHR);
CREATE_VK_HANDLE_DEFINITION(VkPhysicalDevice);
CREATE_VK_HANDLE_DEFINITION(VkDevice);
CREATE_VK_HANDLE_DEFINITION(VkQueue);
CREATE_VK_HANDLE_DEFINITION(VkSwapchainKHR);
CREATE_VK_HANDLE_DEFINITION(VkImage);
CREATE_VK_HANDLE_DEFINITION(VkImageView);
CREATE_VK_HANDLE_DEFINITION(VkCommandPool);
CREATE_VK_HANDLE_DEFINITION(VkCommandBuffer);
CREATE_VK_HANDLE_DEFINITION(VkSemaphore);
CREATE_VK_HANDLE_DEFINITION(VkFence);
CREATE_VK_HANDLE_DEFINITION(VmaAllocator);

typedef uint32_t RV_VkFormat;

typedef struct RV_VkExtent2D
{
    uint32_t width;
    uint32_t height;
} RV_VkExtent2D;

typedef void (*rv_vk_destroy_1)(void *allocator);
typedef void (*rv_vk_destroy_2)(void *subject, void *allocator);
typedef void (*rv_vk_destroy_3)(void *context, void *subject, void *allocator);

typedef struct RV_VkDestroyData
{
    void *context;
    TODO("This technically violates aliasing rules as the pointer isnt guarantueed to be the right size on other machines, look into this")
    void **subject;
    void *allocator;
    union
    {
        rv_vk_destroy_1 destroy_1;
        rv_vk_destroy_2 destroy_2;
        rv_vk_destroy_3 destroy_3;
    };
} RV_VkDestroyData;

typedef struct RV_VkDestroyQueue
{
    RV_VkDestroyData queue[MAX_RV_VK_DESTROY_QUEUE_LEN];
    size_t queue_len;
} RV_VkDestroyQueue;

typedef struct RendererFrameData
{
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphore;
    VkSemaphore render_semaphore;
    VkFence render_fence;

    RV_VkDestroyQueue destroy_queue;
} RendererFrameData;

TODO("Maybe split up the struct into smaller structs, like a queue/logical device struct")
TODO("The smaller struct could also be one for the bootstrap and one for runtime")
typedef struct RendererContext
{
    PLUGIN_CONTEXT_DEPENDENCIES

    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;

    VkDevice device;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;
    VkQueue present_queue;
    uint32_t present_queue_family;
    VkSwapchainKHR swapchain;
    TODO("Figure out what to do with the size/capacity here")
    ARRAY_FIELD(VkImage, swapchain_images, MAX_SWAPCHAIN_IMAGES_LEN);
    ARRAY_FIELD(VkImageView, swapchain_image_views, MAX_SWAPCHAIN_IMAGES_LEN);
    RV_VkFormat swapchain_image_format;
    RV_VkExtent2D swapchain_extent;

    uint32_t frame_number;
    RendererFrameData frames[FRAMES_LEN];

    VmaAllocator vma_allocator;

    RV_VkDestroyQueue destroy_queue;
} RendererContext;

#pragma pack(pop)
