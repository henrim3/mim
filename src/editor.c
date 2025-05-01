#include "editor.h"

#include "common.h"
#include "logging.h"
#include "piece_table.h"
#include <stdio.h>
#include <stdlib.h>

int Editor_init( Editor* editor, SDL_Window* window ) {
  memset( editor, 0, sizeof( Editor ) );

  editor->window = window;

  editor->piece_table = malloc( sizeof( PieceTable ) );
  if ( editor->piece_table == NULL ) {
    LOG_PERROR( "malloc() while allocating editor piece table" );
    return MIM_FAILURE;
  }

  PieceTable_init( editor->piece_table );

  return MIM_SUCCESS;
}

void Editor_free( Editor* editor ) {
  free( editor->piece_table );
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