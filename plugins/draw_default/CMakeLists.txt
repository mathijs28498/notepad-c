add_library(draw_default SHARED)

target_sources(draw_default
    PRIVATE
        draw_default.c
        draw_default_register.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(draw_default
    PRIVATE
        plugin_manager_include
)