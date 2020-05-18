#include "common.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "stb_truetype.h"

#define WIDTH  1280
#define HEIGHT 720

static uint32_t *getSurfaceColorPointer(SDL_Surface *surface, int x, int y)
{
    uint32_t *ptr = ((uint32_t *)surface->pixels) + y * surface->h + x;
    return ptr;
}

static void setSurfacePixelColor(SDL_Surface *surface, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t color = SDL_MapRGBA(surface->format, r, g, b, a);
    *getSurfaceColorPointer(surface, x, y) = color;
}

unsigned char fontBuffer[24<<20];

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "cplayground",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_CAPTURE
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    );

    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    stbtt_fontinfo font;
    {
        FILE *file;
        fopen_s(&file, "c:/windows/fonts/arialbd.ttf", "rb"); //"Roboto-Regular.ttf", "rb");
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        fread(fontBuffer, 1, size, file);
        stbtt_InitFont(&font, fontBuffer, 0);
    }

    float fontScale = stbtt_ScaleForPixelHeight(&font, HEIGHT/3);

    int fontAscent;
    stbtt_GetFontVMetrics(&font, &fontAscent, 0, 0);

    int fontBaseline = (int) (fontAscent*fontScale);

    int w = WIDTH, h = HEIGHT;
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    if (!surface) {
        fprintf(stderr, "Failed to create SDL_Surface: %s", SDL_GetError());
        return 1;
    }

    SDL_LockSurface(surface);

    float xpos = 2; // leave a little padding in case the character extends left
    int ch = 0;
    char *text = "Heljo World!"; // intentionally misspelled to show 'lj' brokenness
    while (text[ch]) {
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);

        float x_shift = xpos - (float)floor(xpos);
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], fontScale, fontScale, x_shift, 0, &x0, &y0, &x1, &y1);

        uint32_t *ptr = getSurfaceColorPointer(surface, (int)xpos + x0, fontBaseline + y0);
        // ptr = surface->pixels;
        // stbtt_MakeCodepointBitmap(&font, (uint8_t *)ptr, x1-x0, y1-y0, surface->pitch, fontScale, fontScale, text[ch]);
        stbtt_MakeCodepointBitmapSubpixel(&font, (uint8_t *)ptr, x1-x0, y1-y0, surface->pitch, fontScale, fontScale, x_shift, 0, text[ch]);

        xpos += (advance * fontScale);
        if (text[ch+1]) {
            xpos += fontScale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
        }
        ch+=1;
    }

    // for (int i = 0; i < surface->w; ++i) {
    //     for (int j = 0; j < surface->h; ++j) {
    //         setSurfacePixelColor(surface, i, j,
    //             (i / (float)surface->w) * 255,
    //             (j / (float)surface->h) * 255,
    //             255, 255);
    //     }
    // }
    SDL_UnlockSurface(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    bool quit = false;
    while (!quit) {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            uint32_t type = sdlEvent.type;
            if (type == SDL_QUIT) {
                quit = true;
            }
            if (type == SDL_KEYDOWN) {
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderClear(renderer);

        // SDL_Rect dstRect = {
        //     .x = 256, .y = 256,
        //     .w = 256, .h = 256,
        // };
        SDL_RenderCopy(renderer, texture, NULL, NULL);// &dstRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
