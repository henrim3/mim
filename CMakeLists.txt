cmake_minimum_required(VERSION 3.24)
project(mim)

set(SDL_VERSION 3.2.10)

# create mim target
add_executable(mim WIN32 src/main.c)

include(FetchContent)

# get SDL3
FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL3-${SDL_VERSION}.tar.gz
)
FetchContent_MakeAvailable(SDL3)

# link SDL3
target_link_libraries(mim PRIVATE SDL3::SDL3)
