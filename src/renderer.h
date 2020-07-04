#ifndef _renderer_h
#define _renderer_h

#include "common.h"

typedef struct SDL_Window SDL_Window;

bool renderer_init(SDL_Window *window);
void renderer_deinit(void);

void renderer_clear(uint8_t r, uint8_t g, uint8_t b);
void renderer_present(void);

void renderer_print_text(char *text, int pixel_size, int x, int y);

#endif /* _renderer_h */