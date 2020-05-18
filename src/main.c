#include "common.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "stb_truetype.h"

#define WIDTH  1280
#define HEIGHT 720

#if 0
unsigned char buffer[24<<20];
unsigned char screen[20][79];

int main(int arg, char **argv)
{
    int i,j,ascent,baseline,ch=0;

    stbtt_fontinfo font;
    {
        FILE *fs;
        fopen_s(&fs, "Roboto-Reqular.ttf", "rb");
        fread(buffer, 1, 1000000, fs);
        stbtt_InitFont(&font, buffer, 0);
    }

    float scale = stbtt_ScaleForPixelHeight(&font, 15);
    stbtt_GetFontVMetrics(&font, &ascent,0,0);
    baseline = (int) (ascent*scale);

    char *text = "Heljo World!"; // intentionally misspelled to show 'lj' brokenness
    float xpos=2; // leave a little padding in case the character extends left
    while (text[ch]) {
        int advance,lsb,x0,y0,x1,y1;
        float x_shift = xpos - (float) floor(xpos);
        stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
        stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale,scale,x_shift,0, &x0,&y0,&x1,&y1);
        stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int) xpos + x0], x1-x0,y1-y0, 79, scale,scale,x_shift,0, text[ch]);
        // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
        // because this API is really for baking character bitmaps into textures. if you want to render
        // a sequence of characters, you really need to render each bitmap to a temp buffer, then
        // "alpha blend" that into the working buffer
        xpos += (advance * scale);
        if (text[ch+1]) {
            xpos += scale*stbtt_GetCodepointKernAdvance(&font, text[ch],text[ch+1]);
        }
        ++ch;
    }

    for (j=0; j < 20; ++j) {
        for (i=0; i < 78; ++i) {
            putchar(" .:ioVM@"[screen[j][i]>>5]);
        }
        putchar('\n');
    }

    return 0;
}
#endif


#if 1
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
        fopen_s(&file, "Roboto-Regular.ttf", "rb");
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        fread(fontBuffer, 1, size, file);
        stbtt_InitFont(&font, fontBuffer, 0);
    }

    float fontScale = stbtt_ScaleForPixelHeight(&font, HEIGHT);

    int fontAscent;
    stbtt_GetFontVMetrics(&font, &fontAscent, 0, 0);

    int fontBaseline = (int) (fontAscent*fontScale);

    int w = 128, h = 128;
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    if (!surface) {
        fprintf(stderr, "Failed to create SDL_Surface: %s", SDL_GetError());
        return 1;
    }

    SDL_LockSurface(surface);
    for (int i = 0; i < surface->h; ++i) {
        for (int j = 0; j < surface->w; ++j) {
            uint32_t color = SDL_MapRGBA(
                surface->format,
                (j / (float)surface->w) * 255,
                (i / (float)surface->h) * 255,
                255, 255
            );
            *(((uint32_t *)surface->pixels) + i * surface->h + j) = color;
        }
    }
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect dstRect = {
            .x = 100, .y = 100,
            .w = 256, .h = 256,
        };
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
#endif
