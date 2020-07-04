#include "renderer.h"
#include "utils.h"

#include "stb_ds.h"
#include "stb_truetype.h"

#include "SDL.h"

static SDL_Renderer *renderer;
static stbtt_fontinfo default_font;

static void set_surface_pixel_color(
    SDL_Surface *surface, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    int off = y * surface->w + x;
    uint32_t *ptr = ((uint32_t *)surface->pixels) + off;
    uint32_t color = SDL_MapRGBA(surface->format, r, g, b, a);
    *ptr = color;
}

bool renderer_init(SDL_Window *window)
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Render backend failed to initalize SDL_Renderer %s", SDL_GetError());
        return false;
    }

    uint8_t *font_buffer = read_entire_binary_file("Roboto-Regular.ttf");
    stbtt_InitFont(&default_font, font_buffer, 0);

    return true;
}

void renderer_clear(uint8_t r, uint8_t g, uint8_t b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void renderer_present(void) { SDL_RenderPresent(renderer); }

typedef struct {
    SDL_Texture *texture;
    int width, height;
    int offset_x, offset_y;
} GlyphData;

static GlyphData create_glyph_data(char character, int pixel_size)
{
    GlyphData result = { 0 };

    float font_scale = stbtt_ScaleForPixelHeight(&default_font, (float)pixel_size);

    int w, h, xoff, yoff;
    uint8_t *bmp = stbtt_GetCodepointBitmap(
        &default_font, font_scale, font_scale, character, &w, &h, &xoff, &yoff);
    if (!w || !h) {
        stbtt_FreeBitmap(bmp, 0);

        int advance, left_side_bearing;
        stbtt_GetCodepointHMetrics(&default_font, character, &advance, &left_side_bearing);

        result.width = (int)(font_scale * advance);

        return result;
    }

    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    if (!surface) {
        fprintf(stderr, "Failed to create SDL_Surface: %s\n", SDL_GetError());
        return result;
    }

    SDL_LockSurface(surface);
    for (int i = 0; i < surface->w; ++i) {
        for (int j = 0; j < surface->h; ++j) {
            uint8_t alpha = bmp[j * surface->w + i];
            set_surface_pixel_color(surface, i, j, alpha, alpha, alpha, alpha);
        }
    }
    SDL_UnlockSurface(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "Failed to create SDL_Texturer: %s\n", SDL_GetError());
        return result;
    }
    SDL_FreeSurface(surface);

    result.texture = texture;
    result.width = w;
    result.height = h;
    result.offset_x = xoff;
    result.offset_y = yoff;

    stbtt_FreeBitmap(bmp, 0);

    return result;
}

typedef struct {
    char key;
    GlyphData value;
} CharacterToGlyphDataMap;

typedef struct {
    int key;
    CharacterToGlyphDataMap *value;
} FontSizeToCharacterMap;

static FontSizeToCharacterMap *font_size_to_character_map = NULL;

static GlyphData get_or_create_glyph_data(char character, int pixel_size)
{
    GlyphData result;

    if (hmgeti(font_size_to_character_map, pixel_size) >= 0) {
        CharacterToGlyphDataMap *glyph_data_map = hmget(font_size_to_character_map, pixel_size);
        if (hmgeti(glyph_data_map, character) >= 0) {
            result = hmget(glyph_data_map, character);
        } else {
            result = create_glyph_data(character, pixel_size);
            hmput(glyph_data_map, character, result);
            hmput(font_size_to_character_map, pixel_size, glyph_data_map);
        }
    } else {
        CharacterToGlyphDataMap *glyph_data_map = NULL;
        result = create_glyph_data(character, pixel_size);
        hmput(glyph_data_map, character, result);
        hmput(font_size_to_character_map, pixel_size, glyph_data_map);
    }

    return result;
}

void renderer_print_text(char *text, int pixel_size, int x, int y)
{
    GlyphData *arr_glyph_datas = NULL;
    for (int ch = 0; text[ch]; ++ch) {
        GlyphData data = get_or_create_glyph_data(text[ch], pixel_size);
        arrpush(arr_glyph_datas, data);
    }

    for (int i = 0; i < arrlen(arr_glyph_datas); ++i) {
        GlyphData *gd = &arr_glyph_datas[i];
        if (gd->texture) { // for spaces we have null texture, but have to advance x
            SDL_Rect dstRect = {
                .x = x + gd->offset_x,
                .y = y + gd->offset_y,
                .w = gd->width,
                .h = gd->height,
            };
            SDL_Rect srcRect = {
                .w = gd->width,
                .h = gd->height,
                .x = 0,
                .y = 0,
            };

            SDL_SetTextureBlendMode(gd->texture, SDL_BLENDMODE_ADD);
            SDL_RenderCopy(renderer, gd->texture, &srcRect, &dstRect);
        }
        x += gd->width + gd->offset_x;
    }

    arrfree(arr_glyph_datas);
}

void renderer_deinit(void)
{
    // Clear font caches
    size_t fs_map_len = hmlen(font_size_to_character_map);
    for (size_t i = 0; i < fs_map_len; ++i) {
        CharacterToGlyphDataMap *ch_map = font_size_to_character_map[i].value;
        size_t ch_map_len = hmlen(ch_map);
        for (size_t j = 0; j < ch_map_len; ++j) {
            GlyphData *gd = &ch_map[j].value;
            if (gd->texture) {
                SDL_DestroyTexture(gd->texture);
            }
        }
        hmfree(ch_map);
    }
    hmfree(font_size_to_character_map);

    SDL_DestroyRenderer(renderer);
}
