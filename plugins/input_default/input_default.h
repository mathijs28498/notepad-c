#pragma once



#include <stdint.h>
#include <stdbool.h>

struct InputInterfaceContext;
struct WindowEvent;
enum WindowEventKey;

int32_t input_default_prepare_processing(struct InputInterfaceContext *context);
int32_t input_default_process_window_event(struct InputInterfaceContext *context, struct WindowEvent *window_event);

bool input_default_key_pressed(struct InputInterfaceContext *context, enum WindowEventKey key);
bool input_default_key_held(struct InputInterfaceContext *context, enum WindowEventKey key);
bool input_default_key_released(struct InputInterfaceContext *context, enum WindowEventKey key);

