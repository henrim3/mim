cmake_minimum_required(VERSION 3.24)
project(mim)

set(SDL_VERSION 3.2.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# get SDL3
FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL3-${SDL_VERSION}.tar.gz
)
FetchContent_MakeAvailable(SDL3)

# create mim target
add_executable(mim src/main.c src/editor.c src/piece_table.c src/font.c)
# if(WIN32)
#     add_executable(mim WIN32 src/main.c)
# else()
#     add_executable(mim src/main.c)
# endif()

# link SDL3
target_link_libraries(mim PRIVATE SDL3::SDL3)

# copy dll to build/ if Windows
if(WIN32)
  add_custom_command(
    TARGET mim POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "$<TARGET_FILE:SDL3::SDL3>"
      "$<TARGET_FILE_DIR:mim>"
    VERBATIM
  )
endif()
