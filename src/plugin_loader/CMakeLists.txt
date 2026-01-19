# TODO: Add platform independent plugin loader

add_library(PluginLoader)

target_sources(PluginLoader
    PRIVATE
        plugin_loader.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(PluginLoader
    PluginCommon
)