#include "common.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "stb_ds.h"
#include "stb_truetype.h"

#define WIDTH  1280
#define HEIGHT 720

typedef struct Vec2i {
    int32_t x, y;
} Vec2i;

typedef struct Vec2 {
    float x, y;
} Vec2;

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

    // int fontAscent;
    // stbtt_GetFontVMetrics(&font, &fontAscent, 0, 0);

    // int fontBaseline = (int) (fontAscent*fontScale);

    // int w = WIDTH, h = HEIGHT;
    // SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    // if (!surface) {
    //     fprintf(stderr, "Failed to create SDL_Surface: %s", SDL_GetError());
    //     return 1;
    // }

    SDL_Texture **fontTextures = NULL;
    Vec2i *sizes = NULL;
    Vec2i *offs = NULL;
    char *text = "Heljo World!";
    for (int ch = 0; text[ch]; ++ch) {
        int w, h, xoff, yoff;
        uint8_t *bmp = stbtt_GetCodepointBitmap(&font, fontScale, fontScale, text[ch], &w, &h, &xoff, &yoff);

        SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
        if (!surface) {
            fprintf(stderr, "Failed to create SDL_Surface: %s", SDL_GetError());
            return 1;
        }

        SDL_LockSurface(surface);
        memcpy(surface->pixels, bmp, w * h);
        SDL_UnlockSurface(surface);

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        arrpush(fontTextures, texture);
        Vec2i size = { w, h };
        arrpush(sizes, size);
        Vec2i off = { xoff, yoff };
        arrpush(offs, off);
    }

    // SDL_LockSurface(surface);
    // for (int i = 0; i < surface->w; ++i) {
    //     for (int j = 0; j < surface->h; ++j) {
    //         setSurfacePixelColor(surface, i, j,
    //             (i / (float)surface->w) * 255,
    //             (j / (float)surface->h) * 255,
    //             255, 255);
    //     }
    // }
    // SDL_UnlockSurface(surface);

    // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

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

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        int x = 0, y = 0;
        for (int i = 0; i < arrlen(fontTextures); ++i) {
            SDL_Texture *texture = fontTextures[i];
            Vec2i size = sizes[i];
            Vec2i off = offs[i];
            SDL_Rect dstRect = {
                .x = x, .y = y,
                .w = size.x-off.x, .h = size.y-off.y,
            };
            SDL_Rect srcRect = {
                .w = size.x/4, .h = size.y/4, // Why do I need to divide by 4??
                .x = 0, .y = 0,
            };
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
            SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
            x+=size.x;
        }

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < arrlen(fontTextures); ++i) {
        SDL_Texture *texture = fontTextures[i];
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
