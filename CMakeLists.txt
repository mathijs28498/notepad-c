cmake_minimum_required(VERSION 3.23)

project(NotepadC C)

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "C compiler version: ${CMAKE_C_COMPILER_VERSION}")

# if(MSVC)
#     add_compile_options(/WX)
# else()
#     add_compile_options(-Werror)
# endif()


if(MSVC)
    add_compile_options(
        /W4       # Warning level 4 (Production quality - standard "strict")
        /WX       # Treat warnings as errors
        /permissive- # Enforce strict standard conformance (no MSVC extensions/loose rules)
    )
    
    # Optional: If /Wall is too crazy (it usually is), use /W4 and add these specific extras:
    # /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545 ...
    # (See "Jason Turner's recommended flags" for a curated list)
    
else()
    # GCC / Clang
    target_compile_options(Program PRIVATE 
        -Wall -Wextra -Werror 
        
        # Example: How to disable a specific warning in GCC/Clang
        # -Wno-<warning-name>
        # -Wno-unused-variable 
        # -Wno-spectre-mitigation (hypothetical, doesn't actually exist/trigger usually)
    )
   
endif()

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(include)