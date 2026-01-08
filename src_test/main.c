#include <stdio.h>

#include <test_lib.h>
#include <test_lib_i.h>

#include <vulkan/vulkan.h>
#include <cglm.h>

int main(int argc, const char *argv[])
{
    vec2 v0 = {1.0, 2.0}; 
    vec2 v1 = {1.0, 5.0}; 
    // glm_vec2((float[]){1.0, 1.0}, v);
    float cross = glm_vec2_cross(v0, v1);

    // glm_vec2_print(v, stdout);
    printf("cross: %f\n", cross);

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    TestStruct test_struct = {
        .test_int = 400};

    printf("%s() - Value: %d\n", __func__, test_struct.test_int);
    if (argc >= 3)
    {
        printf("%s() argc: %d - argv[1]: %s - argv[2]: %s\n", __func__, argc, argv[1], argv[2]);
    }

    test_lib_print();

    return 0;
}