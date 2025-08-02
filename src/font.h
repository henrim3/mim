#ifndef __MIM_FONT_H__
#define __MIM_FONT_H__

#include <SDL3/SDL.h>

typedef struct Font {
    SDL_Texture* texture;
    int char_width;
    int char_height;
} Font;

Font* Font_new( SDL_Renderer* renderer, SDL_Surface* font_surface, int char_width, int char_height );
void Font_free( Font* font );

#endif