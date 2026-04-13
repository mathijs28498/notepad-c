find_package(Vulkan REQUIRED)

add_library(vma STATIC vk_mem_alloc.cpp)

# The simplest and most reliable way to expose the header to consumers
target_include_directories(vma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vma
    PRIVATE
    Vulkan::Vulkan
)