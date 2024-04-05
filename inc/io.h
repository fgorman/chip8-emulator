#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>

#define NUM_KEYS 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SCALE 10

#define PIXEL_ON 0xFF
#define PIXEL_OFF 0x00

void io_init();
void io_teardown();
void io_update_screen(void const * buffer);
bool io_get_key_press();

extern int keys[NUM_KEYS];

#endif