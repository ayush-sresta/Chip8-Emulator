#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *chip8)
{
    memset(chip8, 0, sizeof(Chip8));

    chip8->pc = 0x200;

    for (int i = 0; i < 80; i++)
    {
        chip8->memory[0x50 + i] = fontset[i];
    }
}

void chip8_load_rom(Chip8 *chip8, const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        printf("Failed to open ROM: %s\n", filename);
        return;
    }

    size_t read_bytes = fread(
        &chip8->memory[0x200],
        1,
        sizeof(chip8->memory) - 0x200,
        file);

    fclose(file);

    printf("Loaded %zu bytes\n", read_bytes);
    printf("Memory: 0x%02X\n", chip8->memory[0x200]);
}

uint16_t chip8_fetch_opcode(Chip8 *chip8)
{
    uint16_t opcode;

    opcode =
        (chip8->memory[chip8->pc] << 8) |
        chip8->memory[chip8->pc + 1];

    chip8->pc += 2;

    return opcode;
}

void chip8_execute(Chip8 *chip8, uint16_t opcode)
{
    uint8_t type = (opcode >> 12) & 0xF;
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint8_t n = opcode & 0xF;
    uint8_t nn = opcode & 0xFF;
    uint16_t nnn = opcode & 0xFFF;

    switch (type)
    {
    case 0x0:
        /*
         * 00E0
         * Clear display
         */
        if (opcode == 0x00E0)
        {
            memset(
                chip8->display,
                0,
                sizeof(chip8->display));
        }

        /*
         * 00EE
         * Return from subroutine
         */
        else if (opcode == 0x00EE)
        {
            if (chip8->sp > 0)
            {
                chip8->sp--;

                chip8->pc =
                    chip8->stack[chip8->sp];
            }
        }

        break;

    /*
     * 1NNN
     * Jump to NNN
     */
    case 0x1:
        chip8->pc = nnn;
        break;

    /*
     * 2NNN
     * Call subroutine at NNN
     */
    case 0x2:
        if (chip8->sp < 16)
        {
            chip8->stack[chip8->sp] =
                chip8->pc;

            chip8->sp++;

            chip8->pc = nnn;
        }

        break;

    /*
     * 3XNN
     * Skip next instruction if Vx == NN
     */
    case 0x3:
        if (chip8->V[x] == nn)
        {
            chip8->pc += 2;
        }

        break;

    /*
     * 4XNN
     * Skip next instruction if Vx != NN
     */
    case 0x4:
        if (chip8->V[x] != nn)
        {
            chip8->pc += 2;
        }

        break;

    /*
     * 5XY0
     * Skip next instruction if Vx == Vy
     */
    case 0x5:
        if (n == 0 &&
            chip8->V[x] == chip8->V[y])
        {
            chip8->pc += 2;
        }

        break;

    /*
     * 6XNN
     * Vx = NN
     */
    case 0x6:
        chip8->V[x] = nn;
        break;

    /*
     * 7XNN
     * Vx += NN
     */
    case 0x7:
        chip8->V[x] += nn;
        break;

    case 0x8:
        switch (n)
        {
        /*
         * 8XY0
         * Vx = Vy
         */
        case 0x0:
            chip8->V[x] =
                chip8->V[y];

            break;

        /*
         * 8XY1
         * Vx |= Vy
         */
        case 0x1:
            chip8->V[x] |=
                chip8->V[y];

            break;

        /*
         * 8XY2
         * Vx &= Vy
         */
        case 0x2:
            chip8->V[x] &=
                chip8->V[y];

            break;

        /*
         * 8XY3
         * Vx ^= Vy
         */
        case 0x3:
            chip8->V[x] ^=
                chip8->V[y];

            break;

        /*
         * 8XY4
         * Vx = Vx + Vy
         */
        case 0x4:
        {
            uint16_t result =
                chip8->V[x] +
                chip8->V[y];

            chip8->V[0xF] =
                result > 0xFF;

            chip8->V[x] =
                result & 0xFF;

            break;
        }

        /*
         * 8XY5
         * Vx = Vx - Vy
         */
        case 0x5:
            chip8->V[0xF] =
                chip8->V[x] >=
                chip8->V[y];

            chip8->V[x] =
                chip8->V[x] -
                chip8->V[y];

            break;

        /*
         * 8XY6
         * Vx >>= 1
         */
        case 0x6:
            chip8->V[0xF] =
                chip8->V[x] & 0x1;

            chip8->V[x] >>= 1;

            break;

        /*
         * 8XY7
         * Vx = Vy - Vx
         */
        case 0x7:
            chip8->V[0xF] =
                chip8->V[y] >=
                chip8->V[x];

            chip8->V[x] =
                chip8->V[y] -
                chip8->V[x];

            break;

        /*
         * 8XYE
         * Vx <<= 1
         */
        case 0xE:
            chip8->V[0xF] =
                (chip8->V[x] >> 7) & 0x1;

            chip8->V[x] <<= 1;

            break;
        }

        break;

    /*
     * 9XY0
     * Skip next instruction if Vx != Vy
     */
    case 0x9:
        if (chip8->V[x] !=
            chip8->V[y])
        {
            chip8->pc += 2;
        }

        break;

    /*
     * ANNN
     * I = NNN
     */
    case 0xA:
        chip8->I = nnn;
        break;

    /*
     * BNNN
     * PC = V0 + NNN
     */
    case 0xB:
        chip8->pc =
            chip8->V[0] + nnn;

        break;

    /*
     * CXNN
     * Vx = random & NN
     */
    case 0xC:
        chip8->V[x] =
            (rand() & 0xFF) & nn;

        break;

    /*
     * DXYN
     * Draw sprite
     */
    case 0xD:
    {
        chip8->V[0xF] = 0;

        for (uint8_t row = 0; row < n; row++)
        {
            uint8_t sprite =
                chip8->memory[chip8->I + row];

            for (uint8_t col = 0; col < 8; col++)
            {
                uint8_t sprite_pixel =
                    (sprite >> (7 - col)) & 1;

                uint8_t x_pos =
                    (chip8->V[x] + col) % 64;

                uint8_t y_pos =
                    (chip8->V[y] + row) % 32;

                if (sprite_pixel == 1)
                {
                    if (chip8->display[y_pos * 64 + x_pos] == 1)
                    {
                        chip8->V[0xF] = 1;
                    }

                    chip8->display[y_pos * 64 + x_pos] ^= 1;
                }
            }
        }

        break;
    }

    case 0xE:
        switch (nn)
        {
        /*
         * EX9E
         * Skip if key Vx is pressed
         */
        case 0x9E:
            if (chip8->V[x] < 16 &&
                chip8->keypad[chip8->V[x]])
            {
                chip8->pc += 2;
            }

            break;

        /*
         * EXA1
         * Skip if key Vx is NOT pressed
         */
        case 0xA1:
            if (chip8->V[x] >= 16 ||
                !chip8->keypad[chip8->V[x]])
            {
                chip8->pc += 2;
            }

            break;
        }

        break;

    case 0xF:
        switch (nn)
        {
        case 0x07:
            chip8->V[x] =
                chip8->delay_timer;
            break;

        case 0x0A:
            chip8->waiting_for_key = 1;
            chip8->key_register = x;
            break;

        case 0x15:
            chip8->delay_timer =
                chip8->V[x];
            break;

        case 0x18:
            chip8->sound_timer =
                chip8->V[x];
            break;

        case 0x1E:
            chip8->I += chip8->V[x];
            break;

        case 0x29:
            chip8->I =
                0x50 + (chip8->V[x] * 5);
            break;

        case 0x33:
        {
            uint8_t value =
                chip8->V[x];

            chip8->memory[chip8->I] =
                value / 100;

            chip8->memory[chip8->I + 1] =
                (value / 10) % 10;

            chip8->memory[chip8->I + 2] =
                value % 10;

            break;
        }

        case 0x55:
            for (uint8_t i = 0; i <= x; i++)
            {
                chip8->memory[chip8->I + i] =
                    chip8->V[i];
            }
            break;

        case 0x65:
            for (uint8_t i = 0; i <= x; i++)
            {
                chip8->V[i] =
                    chip8->memory[chip8->I + i];
            }
            break;
        }

        break;
    }
}

void chip8_key_press(Chip8 *chip8, uint8_t key)
{
    if (key >= 16)
        return;

    chip8->keypad[key] = 1;

    if (chip8->waiting_for_key)
    {
        chip8->V[chip8->key_register] = key;

        chip8->waiting_for_key = 0;
    }
}

void chip8_key_release(Chip8 *chip8, uint8_t key)
{
    if (key >= 16)
        return;

    chip8->keypad[key] = 0;
}

void chip8_update_timers(Chip8 *chip8)
{
    if (chip8->delay_timer > 0)
        chip8->delay_timer--;

    if (chip8->sound_timer > 0)
        chip8->sound_timer--;
}