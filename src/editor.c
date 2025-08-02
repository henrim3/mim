#include "editor.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "font.h"
#include "logging.h"
#include "piece_table.h"

Editor* Editor_new( SDL_Window* window, SDL_Renderer* renderer,
                    SDL_Surface* font_surface, int char_width,
                    int char_height ) {
  Editor* editor = calloc( 1, sizeof( Editor ) );

  editor->window = window;
  editor->renderer = renderer;

  editor->font = Font_new( renderer, font_surface, char_width, char_height );

  editor->piece_table = PieceTable_new();
  if ( editor->piece_table == NULL ) {
    LOG_ERROR( "couldn't create piece table for editor" );
    return NULL;
  }

  return editor;
}

void Editor_free( Editor* editor ) {
  PieceTable_free( editor->piece_table );
  Font_free( editor->font );
  free( editor );
}

static void SDLCALL open_file_callback( void* userdata,
                                        const char* const* filelist,
                                        int filter ) {
  if ( !filelist ) {
    LOG_SDL_ERROR( "error occurred while selecting file" );
    return;
  } else if ( !*filelist ) {
    LOG_MESSAGE( "user didn't select a file" );
    return;
  }

  LOG_MESSAGE( "user selected file: %s", *filelist );

  // only open first file
  Editor* editor = (Editor*)userdata;
  PieceTable_load_file( editor->piece_table, *filelist );
}

void Editor_open_file_picker( Editor* editor ) {
  LOG_MESSAGE( "opening file" );
  SDL_ShowOpenFileDialog( open_file_callback, (void*)editor, editor->window,
                          NULL, 0, NULL, 0 );
}

void _display_char( SDL_Renderer* renderer, Font* font, char c, int x, int y,
                    int w, int h ) {
  SDL_FRect char_rect = {
      .x = 0,
      .y = 0,
      .w = font->char_width,
      .h = font->char_height,
  };
  SDL_FRect dest_rect = {
      .x = x,
      .y = y,
      .w = w,
      .h = h,
  };
  SDL_RenderTexture( renderer, font->texture, &char_rect, &dest_rect );
}

void Editor_render( Editor* editor ) {
  _display_char( editor->renderer, editor->font, 'a', 10, 10, 5, 5 );
}
