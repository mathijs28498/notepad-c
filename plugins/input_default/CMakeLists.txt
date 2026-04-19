add_library(input_default)

target_compile_options(input_default PRIVATE ${MY_STRICT_WARNINGS})

target_sources(input_default
    PRIVATE
    input_default_register.c
    input_default.c

    PUBLIC
    FILE_SET HEADERS
)

target_link_libraries(input_default
    PRIVATE
    plugin_sdk
)

include("PluginSDK")
message(STATUS "Generate inc: inp")
plugin_generate_register_inc(input_default
    GEN_DIR_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated"
    TOML_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}"
)