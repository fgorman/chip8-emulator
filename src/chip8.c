#include "chip8.h"
#include "font.h"

#include <stdio.h>
#include <string.h>

int quit = 0;

void chip8_init(chip8 * const em)
{
    memset(em->memory, 0, MEMORY_SIZE);
    memcpy(em->memory+FONT_START_ADDR, FONT, FONT_NUM_SPRITES*FONT_SPRITE_SIZE);

    em->pc = ROM_START_ADDR;
    em->sp = 0;
}

void chip8_memory_dump(const chip8 * const em)
{
    printf("[");

    for (int i = 0; i < MEMORY_SIZE-1; i++)
    {
        printf("%d, ", em->memory[i]);
    }

    printf("%d]\n", em->memory[MEMORY_SIZE-1]);
}