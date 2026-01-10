add_library(Cglm INTERFACE)

target_sources(Cglm
    INTERFACE
        FILE_SET HEADERS
)

if(MSVC)
    target_compile_options(TestLib PRIVATE /W3 /WX-) # W3 = normal, WX- = No error
else()
    target_compile_options(TestLib PRIVATE -w) # -w disables all warnings (extreme)
endif()