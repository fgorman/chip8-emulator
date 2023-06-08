#ifndef KEYPAD_H
#define KEYPAD_H

#include "chip8.h"

#define NUM_KEYS 16

void io_init();
void io_teardown();
void io_get_key_press();
void print_to_screen();

extern int keys[NUM_KEYS];

#endif