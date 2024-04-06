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

    struct timespec current_time, prev_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &prev_time);
    bool quit = false;

    while (!quit)
    {
        quit = io_get_key_press();

        clock_gettime(CLOCK_MONOTONIC_RAW, &current_time);
        uint64_t time_diff = current_time.tv_nsec - prev_time.tv_nsec;

        if (time_diff > CYCLE_DELAY_NS)
        {
            prev_time = current_time;

            chip8_cycle(&emulator);

            io_update_screen(&emulator.video);
        }
    }

    io_teardown();
}
