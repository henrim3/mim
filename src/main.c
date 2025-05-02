#include <SDL3/SDL.h>
#include <stdio.h>

#include "editor.h"
#include "logging.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main() {
  PieceTable* pt = PieceTable_new();
  PieceTable_load_file( pt, "test.txt" );
  PieceTable_insert_char( pt, 'M', 6 );
  PieceTable_insert_char( pt, 'D', 7 );
  PieceTable_insert_char( pt, 'I', 7 );
  PieceTable_insert_char( pt, 'Z', 0 );
  PieceTable_insert_char( pt, 'X', 1 );
  PieceTable_insert_char( pt, 'Y', 1 );
  PieceTable_insert_char( pt, '5', 14 );
  PieceTable_dump( pt );
  PieceTable_dump_pieces( pt );
  PieceTable_free( pt );
}

// void handle_key_down( SDL_KeyboardEvent keyboard_event, Editor* editor ) {
//   printf( "%c was pressed\n", keyboard_event.key );

//   if ( keyboard_event.key == SDLK_O ) {
//     Editor_open_file_picker( editor );
//   }
// }

// void display_char( SDL_Renderer* renderer, SDL_Texture* texture, char c, int
// x,
//                    int y, int w, int h ) {
//   SDL_FRect char_rect = {
//       .x = 0,
//       .y = 0,
//       .w = 6,
//       .h = 6,
//   };
//   SDL_FRect dest_rect = {
//       .x = x,
//       .y = y,
//       .w = w,
//       .h = h,
//   };
//   SDL_RenderTexture( renderer, texture, &char_rect, &dest_rect );
// }

// int main( int argc, char* argv[] ) {
//   printf( "Started mim!\n" );

//   SDL_Window* window = NULL;
//   SDL_Renderer* renderer = NULL;
//   SDL_Surface* screenSurface = NULL;

//   if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
//     LOG_ERROR( "couldn't initialize SDL: %s\n", SDL_GetError() );
//     return 1;
//   }

//   SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;

//   if ( SDL_CreateWindowAndRenderer( "mim", SCREEN_WIDTH, SCREEN_HEIGHT,
//                                     windowFlags, &window, &renderer ) < 0 ) {
//     LOG_ERROR( "couldn't create SDL window and renderer: %s\n ",
//                SDL_GetError() );
//     return 1;
//   }

//   if ( SDL_ShowWindow( window ) < 0 ) {
//     LOG_ERROR( "couldn't show SDL window: %s\n", SDL_GetError() );
//     return 1;
//   }

//   SDL_Surface* image = SDL_LoadBMP( "../fonts/round_6x6.bmp" );
//   SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, image );

//   Editor* editor = Editor_new( window );

//   bool quit = false;
//   while ( !quit ) {
//     SDL_Event event;
//     while ( SDL_PollEvent( &event ) ) {
//       switch ( event.type ) {
//       case SDL_EVENT_QUIT:
//         quit = true;
//         break;

//       case SDL_EVENT_KEY_DOWN:
//         handle_key_down( event.key, editor );
//         break;

//       default:
//       }

//       if ( quit ) {
//         break;
//       }
//     }

//     SDL_RenderPresent( renderer );
//   }

//   printf( "Closing :(\n" );

//   Editor_free( editor );

//   SDL_DestroyTexture( texture );
//   SDL_DestroySurface( image );

//   SDL_DestroyWindow( window );
//   SDL_DestroyRenderer( renderer );
//   SDL_Quit();

//   return 0;
// }
