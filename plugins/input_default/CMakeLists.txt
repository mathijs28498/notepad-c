add_library(input_default SHARED)

target_sources(input_default
    PRIVATE
        input_default_register.c
        input_default.c
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(input_default
    PRIVATE
        plugin_manager_include
)