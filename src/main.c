#include "common.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "stb_ds.h"
#include "stb_truetype.h"

#include "renderer.h"

#define WIDTH 1280
#define HEIGHT 720

#ifdef WIN32_WINMAIN
int WinMain()
#else
int main()
#endif
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("cplayground", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_MOUSE_CAPTURE);

    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    if (!renderer_init(window)) {
        fprintf(stderr, "Failed to init renderer\n");
        return 1;
    }

    bool quit = false;
    while (!quit) {
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            uint32_t type = sdl_event.type;
            switch (type) {
            case SDL_QUIT: {
                quit = true;
            } break;

            case SDL_KEYDOWN: {
                if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            } break;

            default:
                break;
            }
        }

        renderer_clear(91, 0, 176);

        renderer_print_text("Heljo world!", 24, 35, 200);
        renderer_print_text("Wassup world!", 24, 35, 250);
        renderer_print_text("Heljo world!", 32, 35, 350);
        renderer_print_text("Wassup world!", 32, 35, 400);
        renderer_print_text("Wazzzaaaa!", 32, 35, 500);
        renderer_print_text("Larger text?", 128, WIDTH / 2 - 100, HEIGHT / 2);

        renderer_present();
    }

    renderer_deinit();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
