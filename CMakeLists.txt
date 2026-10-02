cmake_minimum_required(VERSION 3.16)

project(Chip8Emulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(chip8
    src/main.c
    src/chip8.c
)

target_include_directories(chip8 PRIVATE
    include
)

target_link_libraries(chip8 PRIVATE
    SDL2::SDL2
)