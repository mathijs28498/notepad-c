add_library(gui_application_default SHARED)

target_sources(gui_application_default
    PRIVATE
        gui_application_default_register.c
        gui_application_default.c
    
    PUBLIC
        FILE_SET HEADERS
)

target_link_libraries(gui_application_default
    PRIVATE
        plugin_manager_include
)