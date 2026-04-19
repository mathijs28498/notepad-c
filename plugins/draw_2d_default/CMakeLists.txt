add_library(draw_2d_default)

target_compile_options(draw_2d_default PRIVATE ${MY_STRICT_WARNINGS})

target_sources(draw_2d_default
    PRIVATE
    draw_2d_default_register.c
    draw_2d_default.c
    PUBLIC
    FILE_SET HEADERS
)

target_link_libraries(draw_2d_default
    PRIVATE
    plugin_sdk
)

include("pluginSDK")
message(STATUS "Generate inc: draw 2d")
plugin_generate_register_inc(draw_2d_default
    GEN_DIR_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated"
    TOML_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}"
)