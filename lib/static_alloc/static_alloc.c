#include "custom_alloc.h"

#include <stdint.h>

#define STATIC_BUFFER_SIZE 4096
static unsigned char static_buffer[STATIC_BUFFER_SIZE];
static uint32_t buffer_offset = 0;

void *static_malloc(uint32_t size)
{
    // Ensure memory alignment
    uint32_t aligned_size = (size + 7) & ~7;

    if (buffer_offset + aligned_size > STATIC_BUFFER_SIZE)
    {
        // Out of memory
        return NULL;
    }

    void *ptr = (void *)(static_buffer + buffer_offset);
    buffer_offset += aligned_size;
    return ptr;
}

void static_free(void *ptr)
{
    // Individual frees are not supported in a simple bump allocator.
    (void)ptr;
}