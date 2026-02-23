add_library(window_win32 SHARED)

target_sources(window_win32
    PRIVATE
        window_win32.c
        window_win32_register.c
        window_win32_window_events.c
        window_win32_key_converter.c

    PUBLIC
        FILE_SET HEADERS
)

target_compile_definitions(window_win32 PRIVATE UNICODE _UNICODE COBJMACROS)

target_link_libraries(window_win32
    PRIVATE
        plugin_manager_include
)