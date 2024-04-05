#include "io.h"

#include "log.h"

#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#define WINDOW_TITLE "CHIP-8 Emulator"

int keys[NUM_KEYS];

void set_key(SDL_Keycode, int);

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;

void io_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        error_log("Could not initialize SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH*SCREEN_SCALE, SCREEN_HEIGHT*SCREEN_SCALE, 0);
    if (window == NULL)
    {
        error_log("Could not create SDL window: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        error_log("Could not create SDL renderer: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL)
    {
        error_log("Could not create SDL texture: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    memset(keys, 0, NUM_KEYS*sizeof(keys[0]));
}

void io_teardown()
{
    SDL_DestroyTexture(texture);
    texture = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

void io_update_screen(void const * buffer)
{
    if (SDL_UpdateTexture(texture, NULL, buffer, SCREEN_WIDTH) != 0)
    {
        error_log("Could not update texture: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    if (SDL_RenderClear(renderer) != 0)
    {
        error_log("Could not clear renderer: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
    {
        error_log("Could not copy texture to renderer: %s\n", SDL_GetError());
        io_teardown();
        exit(-1);
    }

    SDL_RenderPresent(renderer);
}

bool io_get_key_press()
{
    bool quit = false;

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        
        switch(event.type)
        {
            case SDL_KEYDOWN:
                set_key(event.key.keysym.sym, 1);
                break;

            case SDL_KEYUP:
                set_key(event.key.keysym.sym, 0);
                break;

            case SDL_QUIT:
                quit = true;
                break;

            default:
                break;
        }
    }

    return quit;
}

/*
1 2 3 4 -> 1 2 3 C
Q W E R -> 4 5 6 D
A S D F -> 7 8 9 E
Z X C V -> A 0 B F
*/
void set_key(SDL_Keycode key, int val)
{
    switch (key)
    {
        case SDLK_1:
            keys[0x1] = val;
			break;
        case SDLK_2:
            keys[0x2] = val;
			break;
        case SDLK_3:
            keys[0x3] = val;
			break;
        case SDLK_4:
            keys[0xC] = val;
			break;
        case SDLK_q:
            keys[0x4] = val;
			break;
        case SDLK_w:
            keys[0x5] = val;
			break;
        case SDLK_e:
            keys[0x6] = val;
			break;
        case SDLK_r:
            keys[0xD] = val;
			break;
        case SDLK_a:
            keys[0x7] = val;
			break;
        case SDLK_s:
            keys[0x8] = val;
			break;
        case SDLK_d:
            keys[0x9] = val;
			break;
        case SDLK_f:
            keys[0xE] = val;
			break;
        case SDLK_z:
            keys[0xA] = val;
			break;
        case SDLK_x:
            keys[0x0] = val;
			break;
        case SDLK_c:
            keys[0xB] = val;
			break;
        case SDLK_v:
            keys[0xF] = val;
			break;
        default:
            break;
    }
}
