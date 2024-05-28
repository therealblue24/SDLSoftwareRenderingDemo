#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "state.h"

state_t *state;

float SDL_GetMS()
{
	return (float)SDL_GetTicks64() / 1000.f;
}

uint32_t stpx(int x, int y, uint32_t col)
{
	state->pixels[x + y * state->width] = col;
	return state->pixels[x + y * state->width];
}

uint32_t gtpx(int x, int y)
{
	return state->pixels[x + y * state->width];
}

char *state_get_error()
{
	if(state->err_ok == false) {
		return state->last_err;
	}
	return NULL;
}

int init()
{
	state->pixels = calloc(state->width * 4, state->height);
	if(!state->pixels) {
		state->err_ok = false;
		strlcpy(state->last_err, "Failed to allocate memory", 2048);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
int frame()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev)) {
		switch(ev.type) {
		case SDL_QUIT:
			state->quit = true;
			return EXIT_FAILURE;
			break;
		}
	}
	// all software renderend.
	// i am horrible at spelling.
	float time = SDL_GetMS() * 2;
	for(int x = 0; x < state->width; x++) {
		for(int y = 0; y < state->height; y++) {
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
			union {
				uint32_t raw;
				struct {
					//uint8_t a, r, g, b;
					uint8_t b, g, r, a;
				};
			} c;
			c.a = 255;
			c.r = fr;
			c.g = fg;
			c.b = fb;
			stpx(x, y, c.raw);
		}
	}
	return EXIT_SUCCESS;
}
int cleanup()
{
	free(state->pixels);
	return EXIT_SUCCESS;
}

int main()
{
	state = calloc(1, sizeof(state_t));
	ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS),
		   "SDL failed to initialize: %s\n", SDL_GetError());
	state->width = 960;
	state->height = 720;
	state->err_ok = true;
	state->window = SDL_CreateWindow("Software Rendering Demo",
									 SDL_WINDOWPOS_CENTERED_DISPLAY(0),
									 SDL_WINDOWPOS_CENTERED_DISPLAY(0),
									 state->width, state->height,
									 SDL_WINDOW_ALLOW_HIGHDPI);
	ASSERT(state->window, "failed to create SDL window: %s\n", SDL_GetError());
	state->renderer =
		SDL_CreateRenderer(state->window, -1, SDL_RENDERER_PRESENTVSYNC);
	ASSERT(state->renderer, "failed to create SDL renderer: %s\n",
		   SDL_GetError());

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
		if(now - last_frame > 1. / 45.) {
			last_frame = now;
			if(frame()) {
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
			SDL_UpdateTexture(state->texture, NULL, state->pixels,
							  state->width * 4);
			SDL_RenderCopyEx(state->renderer, state->texture, NULL, NULL, 0.0,
							 NULL, SDL_FLIP_VERTICAL);
			SDL_RenderPresent(state->renderer);
		}
		now = SDL_GetMS();
	}
escape:
	ASSERT(!cleanup(), "failed to cleanup software renderer (lmao): %s\n",
		   state_get_error());
	SDL_DestroyTexture(state->texture);
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);
	SDL_Quit();
	return EXIT_SUCCESS;
}
