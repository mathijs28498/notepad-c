#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <string.h>

#include <project_common.h>

#include "renderer_vk.h"

#define MVK_MAX_LAYER_PROPERTIES_COUNT 32

// TODO: Create a container for these
VkInstance g_instance = NULL;

#if IS_DEBUG
const char *const VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"};
#endif // #if IS_DEBUG

VkResult create_instance(VkInstance *instance);

#if IS_DEBUG
bool check_validation_layer_support(void);
#endif // #if IS_DEBUG

int32_t vk_init(void)
{
    VkResult res;

    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    printf("%s() - extension count: %d\n", __func__, extension_count);
    res = create_instance(&g_instance);

    if (res)
    {
        printf("%s() - ERROR: Unable to create instance: %d\n", __func__, res);
        return -1;
    }

    check_validation_layer_support();

    printf("%s() - Successful vk_init\n", __func__);
    return 0;
}

int32_t vk_cleanup(void)
{
    vkDestroyInstance(g_instance, NULL);

    return 0;
}

VkResult create_instance(VkInstance *instance)
{
    VkResult res = VK_SUCCESS;

#if IS_DEBUG
    if (!check_validation_layer_support())
    {
        return VK_ERROR_LAYER_NOT_PRESENT;
    }
#endif // #if IS_DEBUG

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    // TODO: Figure out how get the needed extensions, like done with glfw in the tutorial:
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Instance

#if IS_DEBUG
    create_info.enabledLayerCount = (uint32_t)ARRAY_SIZE(VALIDATION_LAYERS);
    create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
#else  // #if IS_DEBUG
    create_info.enabledLayerCount = 0;
#endif // #if IS_DEBUG

    res = vkCreateInstance(&create_info, NULL, instance);
    if (res)
    {
        return res;
    }

    return res;
}

#if IS_DEBUG
bool check_validation_layer_support(void)
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    if (layer_count > MVK_MAX_LAYER_PROPERTIES_COUNT)
    {
        printf("%s() - WARNING: Too many layer properties (%d), truncating to %d\n", __func__, layer_count, MVK_MAX_LAYER_PROPERTIES_COUNT);
        layer_count = MVK_MAX_LAYER_PROPERTIES_COUNT;
    }

    VkLayerProperties available_layers[MVK_MAX_LAYER_PROPERTIES_COUNT];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (int i = 0; i < (int)ARRAY_SIZE(VALIDATION_LAYERS); i++)
    {
        bool layer_found = false;

        for (int j = 0; j < (int)layer_count; j++)
        {
            if (strcmp(VALIDATION_LAYERS[i], available_layers[j].layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }

        if (!layer_found)
        {
            return false;
        }
    }

    return true;
}
#endif // #if IS_DEBUG
