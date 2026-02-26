find_package(Vulkan REQUIRED)

add_executable(program main.c)

if (WIN32)
    set_target_properties(program PROPERTIES
        WIN32_EXECUTABLE ON
    )
    target_compile_definitions(program PRIVATE UNICODE _UNICODE COBJMACROS)
endif()
# TODO: Add other OS support

set(requested_plugins "")
set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# TODO: Create json to include the plugins and move this to the plugin_manager 
#   this will also have the capability to link interfaces together (draw -> draw2d, draw3d, drawUI, etc...)

include(PluginFramework)
# plugin_framework_add(requested_plugins "gui_application" VARIANT "default")
plugin_framework_add(requested_plugins "logger" )
plugin_framework_load(requested_plugins "${gen_dir}")

target_include_directories(program PRIVATE "${gen_dir}")

target_link_libraries(program
    PRIVATE
        plugin_manager_include
        plugin_manager
        test_lib
        test_yes
)
