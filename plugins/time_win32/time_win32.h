#pragma once

#include <stdint.h>

struct TimeContext;

#define TIME_WIN32_STRING_LEN sizeof("[00:00:00.000,000]")
void time_win32_get_string(struct TimeContext *context, char time_str[TIME_WIN32_STRING_LEN]);

uint64_t time_win32_ms(struct TimeContext *context);
uint64_t time_win32_ns(struct TimeContext *context);
double time_win32_get_elapsed_ms(struct TimeContext *context, uint64_t start_ns, uint64_t end_ns);