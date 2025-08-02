#include "editor.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "font.h"
#include "logging.h"
#include "piece_table.h"
#include "vec.h"

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

void _Editor_display_char( Editor* editor, char c, int x, int y, int w,
                           int h ) {
  IntVec2 coords = Font_get_char_coords( editor->font, c );

  SDL_FRect char_rect = {
      .x = coords.x,
      .y = coords.y,
      .w = editor->font->char_width,
      .h = editor->font->char_height,
  };
  SDL_FRect dest_rect = {
      .x = x,
      .y = y,
      .w = w,
      .h = h,
  };
  SDL_RenderTexture( editor->renderer, editor->font->texture, &char_rect,
                     &dest_rect );
}

void _Editor_display_line( Editor* editor, char* s ) {
  int x = 0;
  for ( char* ptr = s; *ptr != '\0'; ptr++ ) {
    _Editor_display_char( editor, *ptr, x, 30, 30, 30 );
    x += 30;
  }
}

void Editor_render( Editor* editor ) {
  _Editor_display_line( editor,
                        "The quick brown fox jumped over the lazy dog" );
  // _Editor_display_char( editor, 'a', 5, 5, 20, 20 );
}
