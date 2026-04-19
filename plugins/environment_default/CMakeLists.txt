add_library(environment_default)

target_compile_options(environment_default PRIVATE ${MY_STRICT_WARNINGS})

target_sources(environment_default
    PRIVATE
    environment_default.c
    environment_default_register.c

    PUBLIC
    FILE_SET HEADERS
)

target_link_libraries(environment_default
    PRIVATE
    plugin_sdk
)

include("PluginSDK")
plugin_generate_register_inc(environment_default
    GEN_DIR_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated"
    TOML_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}"
)
