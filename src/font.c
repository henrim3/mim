#include "font.h"

#include <SDL3/SDL.h>
#include <stdlib.h>

#include "logging.h"
#include "vec.h"

Font* Font_new( SDL_Renderer* renderer, SDL_Surface* font_surface, int char_width, int char_height ) {
  Font* font = malloc( sizeof( Font ) );

  // create font texture
  font->texture = SDL_CreateTextureFromSurface( renderer, font_surface );

  font->char_width = char_width;
  font->char_height = char_height;

  font->rows = font->texture->h / char_height;
  font->cols = font->texture->w / char_width;
}

IntVec2 Font_get_char_coords( Font* font, char c ) {
  // capital letter
  if ( c >= 'A' && c <= 'Z' ) {
    int char_idx = c - 'A';

    IntVec2 res = {
        .x = ( char_idx % font->cols ) * font->char_width,
        .y = ( char_idx / font->cols ) * font->char_height,
    };

    LOG_MESSAGE( "for char: %c, x=%d, y=%d", c, res.x, res.y );

    return res;
  }

  if ( c >= 'a' && c <= 'z' ) {
    int char_idx = c - 'a' + 26;

    IntVec2 res = {
        .x = ( char_idx % font->cols ) * font->char_width,
        .y = ( char_idx / font->cols ) * font->char_height,
    };

    LOG_MESSAGE( "for char: %c, x=%d, y=%d", c, res.x, res.y );

    return res;
  }

  // return bottom right corner for space
  if ( c == ' ' ) {
    IntVec2 res = {
        .x = ( font->cols - 1 ) * font->char_width,
        .y = ( font->rows - 1 ) * font->char_height,
    };
    return res;
  }

  // return bottom left for others for now
  IntVec2 res = {
      .x = 0,
      .y = ( font->rows - 1 ) * font->char_height,
  };
  return res;
}

void Font_free( Font* font ) {
  SDL_DestroyTexture( font->texture );
  free( font );
}
