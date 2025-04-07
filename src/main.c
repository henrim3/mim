#include <SDL3/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void scc(int code) {
    if (!code) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

void* scp(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

void drawThing() {}

int main(int argc, char* argv[]) {
    printf("Started mim!\n");

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* screenSurface = NULL;

    scc(SDL_Init(SDL_INIT_VIDEO));

    scc(SDL_CreateWindowAndRenderer("mim", SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                    &window, &renderer));

    scc(SDL_ShowWindow(window));

    SDL_FRect rect = {.x = 5, .y = 5, .w = 5, .h = 5};

    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT) {
            break;
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    printf("Closing :(\n");

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
