#pragma once
#ifndef TEST_PLUGIN_H
#define TEST_PLUGIN_H

#pragma pack(push, 8)

struct TestApi2;

typedef struct TestApiContext
{
    struct TestApi2 *test_api_2;
} TestApiContext;

#pragma pack(pop)

#endif // #ifndef TEST_PLUGIN_H