#pragma once
#ifndef ENVIRONMENT_API_H
#define ENVIRONMENT_API_H

#pragma pack(push, 8)

struct EnvironmentApiContext;

typedef struct EnvironmentApi
{
    struct EnvironmentApiContext *context;

    void (*get_args)(struct EnvironmentApiContext *context, int *argc, char ***argv);
} EnvironmentApi;

#pragma pack(pop)

#endif // #ifndef ENVIRONMENT_API_H