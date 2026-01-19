add_library(TestPlugin SHARED)

target_sources(TestPlugin
    PRIVATE
        test_plugin.c
    
    PUBLIC
        FILE_SET HEADERS
        FILES
            test_plugin.h
)