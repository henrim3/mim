#include "font.h"

#include <SDL3/SDL.h>
#include <stdlib.h>

Font* Font_new( SDL_Renderer* renderer, SDL_Surface* font_surface, int char_width, int char_height ) {
  Font* font = malloc( sizeof( Font ) );

  // create font texture
  font->texture = SDL_CreateTextureFromSurface( renderer, font_surface );

  font->char_width = char_width;
  font->char_height = char_height;
}

void Font_free( Font* font ) {
  SDL_DestroyTexture( font->texture );
  free( font );
}
