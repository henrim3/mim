#include <SDL3/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* argv[]) {
    printf("Started mim!\n");

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* screenSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_CreateWindowAndRenderer("mim", SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                    &window, &renderer) < 0) {
        fprintf(stderr, "ERROR: Couldn't create SDL window and renderer: %s\n",
                SDL_GetError());
        return 1;
    }

    if (SDL_ShowWindow(window) < 0) {
        fprintf(stderr, "ERROR: Couldn't show SDL window: %s\n",
                SDL_GetError());
        return 1;
    }

    SDL_FRect rect = {.x = 5, .y = 5, .w = 5, .h = 5};

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    printf("key pressed: %d\n", event.key.key);
                    break;

                case SDL_EVENT_KEY_UP:
                    printf("key released: %d\n", event.key.key);
                    break;

                default:
            }

            if (!running) {
                break;
            }
        }
    }

    printf("Closing :(\n");

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}
