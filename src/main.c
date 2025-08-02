#include <SDL3/SDL.h>
#include <stdio.h>

#include "editor.h"
#include "font.h"
#include "logging.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// int main() {
//   PieceTable* pt = PieceTable_new();
//   PieceTable_load_file( pt, "test.txt" );
//   PieceTable_insert_char( pt, 'M', 6 );
//   PieceTable_insert_char( pt, 'D', 7 );
//   PieceTable_insert_char( pt, 'I', 7 );
//   PieceTable_insert_char( pt, 'Z', 0 );
//   PieceTable_insert_char( pt, 'X', 1 );
//   PieceTable_insert_char( pt, 'Y', 1 );
//   PieceTable_insert_char( pt, '5', 14 );
//   PieceTable_dump( pt );
//   PieceTable_dump_pieces( pt );
//   PieceTable_free( pt );
// }

void handle_key_down( SDL_KeyboardEvent keyboard_event, Editor* editor ) {
  if ( keyboard_event.key >= 'A' && keyboard_event.key <= 'z' ) {
    printf( "%c was pressed\n", keyboard_event.key );
  } else {
    printf( "non-alpha key was pressed\n" );
  }

  if ( keyboard_event.key == SDLK_O && keyboard_event.mod & SDL_KMOD_CTRL ) {
    Editor_open_file_picker( editor );
  }
}

int main( int argc, char* argv[] ) {
  printf( "Started mim!\n" );

  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Surface* screenSurface = NULL;

  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    LOG_ERROR( "couldn't initialize SDL: %s\n", SDL_GetError() );
    return 1;
  }

  SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;

  if ( SDL_CreateWindowAndRenderer( "mim", SCREEN_WIDTH, SCREEN_HEIGHT,
                                    windowFlags, &window, &renderer ) < 0 ) {
    LOG_ERROR( "couldn't create SDL window and renderer: %s\n ",
               SDL_GetError() );
    return 1;
  }

  if ( SDL_ShowWindow( window ) < 0 ) {
    LOG_ERROR( "couldn't show SDL window: %s\n", SDL_GetError() );
    return 1;
  }

  // create font
  SDL_Surface* font_surface = SDL_LoadBMP( "../fonts/round_6x6.bmp" );
  Editor* editor = Editor_new( window, renderer, font_surface, 6, 6 );
  SDL_DestroySurface( font_surface );

  bool quit = false;
  while ( !quit ) {
    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
      switch ( event.type ) {
      case SDL_EVENT_QUIT:
        quit = true;
        break;

      case SDL_EVENT_KEY_DOWN:
        handle_key_down( event.key, editor );
        break;

      default:
      }

      if ( quit ) {
        break;
      }
    }

    Editor_render( editor );

    SDL_RenderPresent( renderer );
  }

  printf( "Closing :(\n" );

  // PieceTable_dump( editor->piece_table );
  // PieceTable_dump_pieces( editor->piece_table );

  Editor_free( editor );

  SDL_DestroyWindow( window );
  SDL_DestroyRenderer( renderer );
  SDL_Quit();

  return 0;
}
