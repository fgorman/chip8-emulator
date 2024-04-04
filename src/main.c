#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "io.h"

int main(int argc, char ** argv)
{
    printf("%d\n", argc);
    if (argc < 2)
    {
        printf("[CHIP] %s: must include ROM file name\n", __func__);
        exit(-1);
    }

    chip8 emulator;

    chip8_init(&emulator);
    chip8_load_rom(&emulator, argv[1]);

    chip8_memory_dump(&emulator);
}