#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define NUM_VARIABLE_REGISTERS 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 256
#define FONT_START_ADDR 0x50

typedef uint8_t byte;
typedef uint16_t word;

typedef struct chip8
{
    word index;
    word pc;
    word sp;
    byte variable_regs[NUM_VARIABLE_REGISTERS];
    byte sound_timer;
    byte delay_timer;
    byte memory[MEMORY_SIZE];
    word stack[STACK_SIZE];
} chip8;

void chip8_init(chip8 *);
void chip8_memory_dump(const chip8 * const);

extern int quit;

#endif