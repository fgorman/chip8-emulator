#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "io.h"

int main(int argc, char ** argv)
{
    chip8 emulator;

    chip8_init(&emulator);

    io_init();

    while (!quit) 
    { 
        io_get_key_press();
    }

    io_teardown();
}