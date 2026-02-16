add_library(app_plugin SHARED)

target_sources(app_plugin
    PRIVATE
        app_plugin_register.c
        app_plugin.c
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(app_plugin
    PRIVATE
        plugin_manager_common
)