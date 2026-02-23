add_library(draw_2d_default SHARED)

target_sources(draw_2d_default
    PRIVATE
        draw_2d_default.c
        draw_2d_default_register.c

    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(draw_2d_default
    PRIVATE
        plugin_manager_include
)