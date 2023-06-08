#include "io.h"

#include <stdio.h>

#include <SDL.h>

#include "chip8.h"

#define SCREEN_WIDTH 200
#define SCREEN_HEIGHT 200

int keys[NUM_KEYS];

void set_key(SDL_Keycode, int);

SDL_Window * window;
SDL_Renderer * renderer;

void io_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf( stderr, "Could not initialise SDL: %s\n", SDL_GetError() );
        exit( -1 );
    }

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer) < 0)
    {
        fprintf( stderr, "Could not set video mode: %s\n", SDL_GetError() );
        SDL_Quit();
        exit( -1 );
    }

    SDL_SetWindowData(window, "window_name", "chip8");

    memset(keys, 0, NUM_KEYS*sizeof(keys[0]));
}

void io_teardown()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

void io_get_key_press()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        
        switch( event.type ){
            case SDL_KEYDOWN:
                set_key(event.key.keysym.sym, 1);
                break;

            case SDL_KEYUP:
                set_key(event.key.keysym.sym, 0);
                break;

            case SDL_QUIT:
                quit = 1;
                break;

            default:
                return;
        }

    }
}

void print_to_screen()
{

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
        case SDLK_ESCAPE:
            quit = 1;
            break;
        default:
            break;
    }
}