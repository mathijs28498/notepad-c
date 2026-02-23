add_library(renderer_vulkan SHARED)

target_sources(renderer_vulkan
    PRIVATE
        renderer_vulkan.c
        renderer_vulkan_register.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(renderer_vulkan
    PRIVATE
        plugin_manager_include
)