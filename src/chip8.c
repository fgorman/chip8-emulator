#include "chip8.h"
#include "font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int quit = 0;

void chip8_init(chip8 * const em)
{
    memset(em->memory, 0, MEMORY_SIZE);
    memcpy(em->memory+FONT_START_ADDR, FONT, FONT_NUM_SPRITES*FONT_SPRITE_SIZE);

    em->pc = ROM_START_ADDR;
    em->sp = 0;
}

void chip8_load_rom(chip8 * const em, const char * const rom_file_name)
{
    FILE * fp = fopen(rom_file_name, "rb");

    if (fp == NULL)
    {  
        printf("[CHIP8] %s: Unable to load rom: %s\n", __func__, rom_file_name);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);

    fread(&em->memory[ROM_START_ADDR], sizeof(em->memory[0]), file_size, fp);

    fclose(fp);
}

void chip8_memory_dump(const chip8 * const em)
{
    printf("[");

    for (int i = 0; i < MEMORY_SIZE-1; i++)
    {
        printf("0x%X, ", em->memory[i]);
    }

    printf("0x%X]\n", em->memory[MEMORY_SIZE-1]);
}