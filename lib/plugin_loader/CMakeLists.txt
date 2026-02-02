# TODO: Add platform independent plugin loader

add_library(plugin_loader)

target_sources(plugin_loader
    PRIVATE
        plugin_loader.c
        plugin_loader_json_parser.c
        plugin_loader_config_reader.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(plugin_loader
    PRIVATE
        plugin_common
        cjson
)