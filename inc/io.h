#ifndef KEYPAD_H
#define KEYPAD_H

#define NUM_KEYS 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

void io_init();
void io_teardown();
void io_get_key_press();
void print_to_screen();

extern int keys[NUM_KEYS];

#endif