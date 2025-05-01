#include <SDL3/SDL.h>
#include <stdio.h>

#include "logging.h"
#include "piece_table.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void handle_key_down( SDL_KeyboardEvent keyboard_event ) {
  printf( "%c was pressed\n", keyboard_event.key );
}

int main() {
  PieceTable pt = { 0 };
  PieceTable_load_file( &pt, "test.txt" );
  PieceTable_insert( &pt, 0, "MIDDLE" );
  PieceTable_dump_pieces( &pt );
  PieceTable_output_final( &pt );
  PieceTable_free( &pt );
  return 0;
}

// int main(int argc, char* argv[]) {
//     printf("Started mim!\n");

//     SDL_Window* window = NULL;
//     SDL_Renderer* renderer = NULL;
//     SDL_Surface* screenSurface = NULL;

//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         fprintf(stderr, "ERROR: Couldn't initialize SDL: %s\n",
//         SDL_GetError()); return 1;
//     }

//     SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;

//     if (SDL_CreateWindowAndRenderer("mim", SCREEN_WIDTH, SCREEN_HEIGHT,
//                                     windowFlags, &window, &renderer) < 0) {
//         fprintf(stderr, "ERROR: Couldn't create SDL window and renderer:
//         %s\n",
//                 SDL_GetError());
//         return 1;
//     }

//     if (SDL_ShowWindow(window) < 0) {
//         fprintf(stderr, "ERROR: Couldn't show SDL window: %s\n",
//                 SDL_GetError());
//         return 1;
//     }

//     SDL_Surface* image = SDL_LoadBMP("../fonts/round_6x6.bmp");
//     SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);

//     SDL_FRect image_rect = {
//         .x = 0,
//         .y = 0,
//         .w = image->w,
//         .h = image->h,
//     };

//     bool quit = false;
//     while (!quit) {
//         SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             switch (event.type) {
//                 case SDL_EVENT_QUIT:
//                     quit = true;
//                     break;

//                 case SDL_EVENT_KEY_DOWN:
//                     handle_key_down(event.key);
//                     break;

//                 default:
//             }

//             if (quit) {
//                 break;
//             }
//         }

//         SDL_RenderTexture(renderer, texture, &image_rect, &image_rect);
//         SDL_RenderPresent(renderer);
//     }

//     printf("Closing :(\n");

//     SDL_DestroyTexture(texture);
//     SDL_DestroySurface(image);

//     SDL_DestroyWindow(window);
//     SDL_DestroyRenderer(renderer);
//     SDL_Quit();

//     return 0;
// }
