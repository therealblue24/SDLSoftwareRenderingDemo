#ifndef STATE_H_
#define STATE_H_

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

typedef struct state_s {
    // our window
    SDL_Window *window;
    // our texture to draw on
    SDL_Texture *texture;
    // our renderer to display the texture
    SDL_Renderer *renderer;
    // our screen we want to render to the texture
    uint32_t *pixels;
    // strlcpy() needed
    char last_err[2048];
    // if this is false there was an error
    bool err_ok;
    // did user request quit?
    bool quit;
    // if you don't know what these variables mean
    // don't program
    uint32_t width;
    uint32_t height;
} state_t;

// custom ASSERT macro
#define ASSERT(_e, ...)               \
    if(!(_e)) {                       \
        fprintf(stderr, __VA_ARGS__); \
        exit(1);                      \
    }

#endif /* STATE_H_ */
