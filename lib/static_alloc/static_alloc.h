#pragma once
#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H

#include <stdint.h>

void *static_malloc(uint32_t size);
void static_free(void *ptr);

#endif // #ifndef CUSTOM_ALLOC_H