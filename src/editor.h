#ifndef __MIM_EDITOR_H__
#define __MIM_EDITOR_H__

#include "font.h"
#include "piece_table.h"

#include <SDL3/SDL.h>
#include <stdio.h>

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  Font* font;
  PieceTable* piece_table;
  int64_t global_cursor_pos;
} Editor;

Editor* Editor_new( SDL_Window* window, SDL_Renderer* renderer,
                    SDL_Surface* font_surface, int char_width,
                    int char_height );
void Editor_free( Editor* editor );
void Editor_open_file_picker( Editor* editor );
int Editor_insert_at_cursor( Editor* editor, char c );
void Editor_render( Editor* editor );

#endif // __MIM_EDITOR_H__