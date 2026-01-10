add_library(TestLibI INTERFACE)

target_sources(TestLibI
    INTERFACE
        FILE_SET HEADERS
        FILES
            test_lib_i.h
)

if(MSVC)
    target_compile_options(TestLib PRIVATE /W3 /WX-) # W3 = normal, WX- = No error
else()
    target_compile_options(TestLib PRIVATE -w) # -w disables all warnings (extreme)
endif()