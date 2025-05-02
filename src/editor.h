#ifndef __MIM_EDITOR_H__
#define __MIM_EDITOR_H__

#include "piece_table.h"
#include <SDL3/SDL.h>
#include <stdio.h>

typedef struct {
  SDL_Window* window;
  PieceTable* piece_table;
} Editor;

Editor* Editor_new( SDL_Window* window );
void Editor_free( Editor* editor );
void Editor_open_file_picker( Editor* editor );

#endif // __MIM_EDITOR_H__