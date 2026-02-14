add_library(window_win32_plugin SHARED)

target_sources(window_win32_plugin
    PRIVATE
        window_win32_plugin.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(window_win32_plugin
    PRIVATE
        plugin_common
)