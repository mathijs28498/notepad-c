
add_library(TestLib)

target_sources(TestLib
    PRIVATE
        test_lib.c

    PUBLIC
        FILE_SET HEADERS
        FILES
            test_lib.h
)

if(MSVC)
    target_compile_options(TestLib PRIVATE /W3 /WX-) # W3 = normal, WX- = No error
else()
    target_compile_options(TestLib PRIVATE -w) # -w disables all warnings (extreme)
endif()