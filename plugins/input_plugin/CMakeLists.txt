add_library(input_plugin SHARED)

target_sources(input_plugin
    PRIVATE
        input_plugin_register.c
        input_plugin.c
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(input_plugin
    PRIVATE
        plugin_manager_common
)