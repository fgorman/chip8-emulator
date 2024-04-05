#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#include "io.h"

#define NUM_VARIABLE_REGISTERS 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define FONT_START_ADDR 0x50
#define ROM_START_ADDR 0x200
#define FLAG_REGISTER 0xFu

typedef uint8_t byte;
typedef uint16_t word;

typedef struct chip8
{
    word index;
    word pc;
    byte sp;
    byte variable_regs[NUM_VARIABLE_REGISTERS];
    byte sound_timer;
    byte delay_timer;
    byte memory[MEMORY_SIZE];
    word stack[STACK_SIZE];
    bool video[SCREEN_WIDTH * SCREEN_HEIGHT];
} chip8;

void chip8_init(chip8 * const);
void chip8_load_rom(chip8 * const, const char * const);
void chip8_cycle(chip8 * const);

extern int quit;

#endif