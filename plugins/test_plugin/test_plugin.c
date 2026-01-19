#include "test_plugin.h"

#include <stdint.h>
#include <stdio.h>


void test_func(uint32_t num)
{
    printf("This works!: %d\n", num);
}