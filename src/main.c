#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "chip8.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define SCALE 10

#define WINDOW_WIDTH (SCREEN_WIDTH * SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCALE)

#define CPU_HZ 700

static int map_key(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_1:
        return 0x1;
    case SDLK_2:
        return 0x2;
    case SDLK_3:
        return 0x3;
    case SDLK_4:
        return 0xC;

    case SDLK_q:
        return 0x4;
    case SDLK_w:
        return 0x5;
    case SDLK_e:
        return 0x6;
    case SDLK_r:
        return 0xD;

    case SDLK_a:
        return 0x7;
    case SDLK_s:
        return 0x8;
    case SDLK_d:
        return 0x9;
    case SDLK_f:
        return 0xE;

    case SDLK_z:
        return 0xA;
    case SDLK_x:
        return 0x0;
    case SDLK_c:
        return 0xB;
    case SDLK_v:
        return 0xF;

    default:
        return -1;
    }
}

static void handle_events(
    Chip8 *chip8,
    bool *running)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            *running = false;
        }

        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                *running = false;
                continue;
            }

            if (event.key.repeat)
                continue;

            int key =
                map_key(event.key.keysym.sym);

            if (key != -1)
            {
                chip8_key_press(
                    chip8,
                    (uint8_t)key);
            }
        }

        else if (event.type == SDL_KEYUP)
        {
            int key =
                map_key(event.key.keysym.sym);

            if (key != -1)
            {
                chip8_key_release(
                    chip8,
                    (uint8_t)key);
            }
        }
    }
}

static void render_display(
    SDL_Renderer *renderer,
    const Chip8 *chip8)
{
    // Black background
    SDL_SetRenderDrawColor(
        renderer,
        0, 0, 0, 255);

    SDL_RenderClear(renderer);

    // White pixels
    SDL_SetRenderDrawColor(
        renderer,
        255, 255, 255, 255);

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int index =
                y * SCREEN_WIDTH + x;

            if (chip8->display[index])
            {
                SDL_Rect pixel = {
                    x * SCALE,
                    y * SCALE,
                    SCALE,
                    SCALE
                };

                SDL_RenderFillRect(
                    renderer,
                    &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(
            "Usage: %s <rom>\n",
            argv[0]);

        return 1;
    }

    Chip8 chip8;

    chip8_init(&chip8);

    chip8_load_rom(
        &chip8,
        argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(
            stderr,
            "SDL_Init failed: %s\n",
            SDL_GetError());

        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow(
            "CHIP-8 Emulator",

            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,

            WINDOW_WIDTH,
            WINDOW_HEIGHT,

            SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateWindow failed: %s\n",
            SDL_GetError());

        SDL_Quit();

        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateRenderer failed: %s\n",
            SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    bool running = true;

    /*
     * High-resolution clock.
     */
    uint64_t last_time =
        SDL_GetPerformanceCounter();

    double frequency =
        (double)SDL_GetPerformanceFrequency();

    /*
     * Time accumulated for CPU
     * and CHIP-8 timers.
     */
    double cpu_accumulator = 0.0;
    double timer_accumulator = 0.0;

    /*
     * CPU runs at 700 instructions/sec.
     */
    const double cpu_period =
        1.0 / CPU_HZ;

    /*
     * CHIP-8 timers run at 60 Hz.
     */
    const double timer_period =
        1.0 / 60.0;

    while (running)
    {
        /*
         * Always process SDL events.
         */
        handle_events(
            &chip8,
            &running);

        /*
         * Calculate elapsed real time.
         */
        uint64_t current_time =
            SDL_GetPerformanceCounter();

        double elapsed =
            (double)(current_time - last_time)
            / frequency;

        last_time = current_time;

        /*
         * Add elapsed time to both
         * accumulators.
         */
        cpu_accumulator += elapsed;
        timer_accumulator += elapsed;

        /*
         * Run CPU at 700 Hz.
         */
        while (cpu_accumulator >= cpu_period)
        {
            if (!chip8.waiting_for_key)
            {
                uint16_t opcode =
                    chip8_fetch_opcode(&chip8);

                chip8_execute(
                    &chip8,
                    opcode);
            }

            cpu_accumulator -= cpu_period;
        }

        /*
         * Update CHIP-8 timers at 60 Hz.
         */
        while (timer_accumulator >= timer_period)
        {
            chip8_update_timers(&chip8);

            timer_accumulator -= timer_period;
        }

        /*
         * Render the display.
         */
        render_display(
            renderer,
            &chip8);
    }

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
