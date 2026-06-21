#pragma once

#include <stdint.h>

#include "../../plugin_utils.h"
#include "../../plugin_macros.h"

#pragma pack(push, 8)

typedef struct TimeContext TimeContext;

#define TIME_STRING_LEN sizeof("[00:00:00.000,000]")

TODO("Add functionality to allow for time formatting (year/day, no microsecond etc)")

typedef struct TimeVtable
{
    void (*get_string)(TimeContext *context, char time_str[TIME_STRING_LEN]);
    uint64_t (*ms)(TimeContext *context);
    uint64_t (*ns)(TimeContext *context);
    double (*get_elapsed_ms)(TimeContext *context, uint64_t start_ns, uint64_t end_ns);
} TimeVtable;

typedef struct TimeInterface
{
    TimeContext *context;
    TimeVtable *vtable;
} TimeInterface;

#pragma pack(pop)

static inline void time_get_string(TimeInterface *iface, char time_str[TIME_STRING_LEN])
{
    VTABLE_METHOD_CALL(iface, get_string, time_str);
}

static inline uint64_t time_ms(TimeInterface *iface)
{
    return VTABLE_METHOD_CALL_NO_ARGS(iface, ms);
}

static inline uint64_t time_ns(TimeInterface *iface)
{
    return VTABLE_METHOD_CALL_NO_ARGS(iface, ns);
}

static inline double time_get_elapsed_ms(TimeInterface *iface, uint64_t start_ns, uint64_t end_ns)
{
    return VTABLE_METHOD_CALL(iface, get_elapsed_ms, start_ns, end_ns);
}