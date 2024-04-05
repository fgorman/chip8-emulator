#include "chip8.h"
#include "log.h"
#include "io.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        error_log("Program requires an input ROM as an argument\n");
        exit(-1);
    }

    chip8 emulator;

    chip8_init(&emulator);
    chip8_load_rom(&emulator, argv[1]);

    io_init();

    time_t prev_time = time(NULL);
    bool quit = false;

    while (!quit)
    {
        quit = io_get_key_press();

        time_t current_time = time(NULL);
        time_t time_diff = current_time - prev_time;

        if (time_diff > CYCLE_DELAY)
        {
            prev_time = current_time;

            chip8_cycle(&emulator);

            io_update_screen(&emulator.video);
        }
    }

    io_teardown();
}