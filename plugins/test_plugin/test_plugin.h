#pragma once
#ifndef TEST_PLUGIN_H
#define TEST_PLUGIN_H

#include <stdint.h>

__declspec(dllexport) void test_func(uint32_t num);

#endif // #ifndef TEST_PLUGIN_H