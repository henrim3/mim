#include <SDL3/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void scc(int code) {
    if (code < 0) {
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

int main(int argc, char* argv[]) {
    printf("Started mim!\n");

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Event event;

    scc(SDL_Init(SDL_INIT_VIDEO));

    window = scp(SDL_CreateWindow("mim", SCREEN_WIDTH, SCREEN_HEIGHT, 0));

    scc(SDL_ShowWindow(window));

    screenSurface = SDL_GetWindowSurface(window);

    // Main loop
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT) break;
    }

    printf("Closing :(\n");

    SDL_UpdateWindowSurface(window);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
