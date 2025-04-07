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

int main() {
    printf("Started mim!");

    SDL_Window* window =
        scp(SDL_CreateWindow("mim", SCREEN_WIDTH, SCREEN_HEIGHT, 0));

    SDL_ShowWindow(window);

    return 0;
}