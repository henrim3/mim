#ifndef __MIM_FONT_H__
#define __MIM_FONT_H__

#include <SDL3/SDL.h>

#include "vec.h"

typedef struct Font {
    SDL_Texture* texture;
    int char_width;
    int char_height;
    int rows;
    int cols;
} Font;

Font* Font_new( SDL_Renderer* renderer, SDL_Surface* font_surface, int char_width, int char_height );
IntVec2 Font_get_char_coords( Font* font, char c );
void Font_free( Font* font );

#endif // __MIM_FONT_H__