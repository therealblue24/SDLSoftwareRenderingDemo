#ifndef STATE_H_
#define STATE_H_

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

typedef struct state_s {
	SDL_Window *window;
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	uint32_t *pixels;
	char last_err[2048];
	bool err_ok;
	bool quit;
	uint32_t width;
	uint32_t height;
} state_t;

#define ASSERT(_e, ...)               \
	if(!(_e)) {                       \
		fprintf(stderr, __VA_ARGS__); \
		exit(1);                      \
	}

#endif /* STATE_H_ */
