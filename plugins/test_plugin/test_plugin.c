#include "test_plugin.h"

#include <plugin_impl.h>
#include <test_api.h>
#include <test_api_2.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define PLUGIN_API_NAME test_api

TestApi2 *test_api_2;

#define TEST_API_DEPENDENCIES(X) \
    X(TestApi2, test_api_2, test_api2)

PLUGIN_REGISTER_DEPENDENCIES(TEST_API_DEPENDENCIES);

int32_t do_something(void)
{
    return test_api_2->add(123, 343);
}

TestApi g_test_api = {
    .do_something = do_something,
};

PLUGIN_REGISTER_API(TestApi, g_test_api);

int32_t init(void)
{
    printf("test_plugin init: \"%d\"\n", test_api_2->add(231, 452));
    return 0;
}

PLUGIN_REGISTER_INIT(init);

// TODO: Add api name in front of
// __declspec(dllexport) void get_dependencies(const char *const **dependencies, int32_t *count)
// {
//     static const char *const plugin_dependencies[] = {
//         "test_api_2",
//     };

//     assert(dependencies != NULL);
//     assert(count != NULL);

//     *dependencies = plugin_dependencies;
//     *count = sizeof(plugin_dependencies) / sizeof(plugin_dependencies[0]);

//     printf("%s - %s()\n", __FILE__, __func__);
// }

// __declspec(dllexport) void set_dependency_test_api_2(void *api)
// {
//     test_api_2 = (TestApi2 *)api;
//     printf("%s - %s()\n", __FILE__, __func__);
// }
