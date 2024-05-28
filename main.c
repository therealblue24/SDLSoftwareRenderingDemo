#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "state.h"

// see state.h
state_t *state;

// get the elasped time in seconds from program begining
float SDL_GetMS()
{
    return (float)SDL_GetTicks64() / 1000.f;
}

// set a pixel @ (x,y) with ABGR col
uint32_t stpx(int x, int y, uint32_t col)
{
    state->pixels[x + y * state->width] = col;
    return state->pixels[x + y * state->width];
}

// set a pixel @ (x,y) with R,G,B
uint32_t stpx_rgb(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    union {
        uint32_t raw;
        struct {
            //uint8_t a, r, g, b;
            uint8_t b, g, r, a;
        };
    } c;
    c.a = 255;
    c.r = r;
    c.g = g;
    c.b = b;
    return stpx(x, y, c.raw);
}

// get a pixel @ (x,y)
uint32_t gtpx(int x, int y)
{
    return state->pixels[x + y * state->width];
}

// get last string error
char *state_get_error()
{
    if(state->err_ok == false) {
        return state->last_err;
    }
    return NULL;
}

// init the program
// enter your own init stuff here
int init()
{
    // allocate pixels
    state->pixels = calloc(state->width * 4, state->height);
    if(!state->pixels) {
        // skill issue
        // (buy more RAM, malloc() and by extension calloc only fail if there is no more memory)
        state->err_ok = false;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
// frame of the program
// runs every [FPS] times p/sec
int frame()
{
    // check if we exited program
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        switch(ev.type) {
        case SDL_QUIT:
            state->quit = true;
            return EXIT_FAILURE;
            break;
        }
    }
    // main rendering loop
    float time = SDL_GetMS() * 2;
    for(int x = 0; x < state->width; x++) {
        for(int y = 0; y < state->height; y++) {
            // demo rendering program
            // replace this
            float uvx = (float)x / (float)state->width;
            float uvy = (float)y / (float)state->height;
            float r = time + uvx;
            float g = time + uvy + 2.0;
            float b = time + uvx + 4.0;
            r = cosf(r) * 0.5;
            g = cosf(g) * 0.5;
            b = cosf(b) * 0.5;
            r += 0.5;
            g += 0.5;
            b += 0.5;
            uint32_t fr = (uint32_t)roundf(r * 255);
            uint32_t fg = (uint32_t)roundf(g * 255);
            uint32_t fb = (uint32_t)roundf(b * 255);

            // set pixels @ (x,y) with (fr,fg,fb)
            stpx_rgb(x, y, fr, fg, fb);
        }
    }
    return EXIT_SUCCESS;
}
int cleanup()
{
    // cleanup state
    free(state->pixels);
    return EXIT_SUCCESS;
}

int main()
{
    // allocate memory state
    state = calloc(1, sizeof(state_t));
    // init SDL
    ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS),
           "SDL failed to initialize: %s\n", SDL_GetError());
    // config
    state->width = 960;
    state->height = 720;
    state->err_ok = true;
    // create centered window that also is highdpi
    state->window = SDL_CreateWindow("Software Rendering Demo",
                                     SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                                     SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                                     state->width, state->height,
                                     SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(state->window, "failed to create SDL window: %s\n", SDL_GetError());
    // create an SDL renderer
    state->renderer =
        SDL_CreateRenderer(state->window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state->renderer, "failed to create SDL renderer: %s\n",
           SDL_GetError());
    // such that we can render our ABGR texture
    state->texture = SDL_CreateTexture(state->renderer,
                                       SDL_PIXELFORMAT_ABGR8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       state->width, state->height);
    ASSERT(state->texture, "failed to create SDL texture: %s\n",
           SDL_GetError());
    ASSERT(!init(), "failed to init software renderer: %s\n",
           state_get_error());
    float now = SDL_GetMS();
    float last_frame = -1.0f;
    while(!state->quit) {
        // really primitive vsync
        // checks the time between the last frame and now to check
        // if we need to frame
        if(now - last_frame > 1. / 45.) {
            last_frame = now;
            // check if frame ran well
            if(frame()) {
                // error
                state->quit = true;
                if(state->err_ok == false) {
                    fprintf(stderr, "software render frame error: %s\n",
                            state_get_error());
                    goto escape;
                }
            }
            if(state->quit) {
                goto escape;
            }
            // update texture
            SDL_UpdateTexture(state->texture, NULL, state->pixels,
                              state->width * 4);
            // render the texture
            SDL_RenderCopyEx(state->renderer, state->texture, NULL, NULL, 0.0,
                             NULL, SDL_FLIP_VERTICAL);
            // present it to the screen
            SDL_RenderPresent(state->renderer);
        }
        now = SDL_GetMS();
    }
    // cleanup goto
escape:
    ASSERT(!cleanup(), "failed to cleanup software renderer (lmao): %s\n",
           state_get_error());
    // cleanup our mess
    SDL_DestroyTexture(state->texture);
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
