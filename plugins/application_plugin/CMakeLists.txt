add_library(application_plugin SHARED)

target_sources(application_plugin
    PRIVATE
        application_plugin_register.c
        application_plugin.c
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(application_plugin
    PRIVATE
        plugin_manager_common
)