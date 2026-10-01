#include "chip8.h"
#include <string.h>

static const __uint8_t fontset[80] = {
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

    printf("Memory: 0x%02X\n", chip8->memory[0x200]);
}

__uint16_t chip8_fetch_opcode(Chip8 *chip8)
{
    __uint16_t opcode;

    opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];

    chip8->pc += 2;

    return opcode;
}

void chip8_execute(Chip8 *chip8, __uint16_t opcode)
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
        break;

    case 0x1:
        break;

    case 0x2:
        break;

    case 0x3:
        break;

    case 0x4:
        break;

    case 0x5:
        break;

    case 0x6:
        chip8->V[x] = nn;
        break;

    case 0x7:
        chip8->V[x] += nn;
        break;

    case 0x8:
        switch (n)
        {
        case 0x0:
            chip8->V[x] = chip8->V[y];
            break;

        case 0x1:
            chip8->V[x] |= chip8->V[y];
            break;

        case 0x2:
            chip8->V[x] &= chip8->V[y];
            break;

        case 0x3:
            chip8->V[x] ^= chip8->V[y];
            break;

        case 0x4:
        {
            uint16_t result = chip8->V[x] + chip8->V[y];

            chip8->V[0xF] = result > 0xFF;
            chip8->V[x] = result & 0xFF;

            break;
        }

        case 0x5:
            chip8->V[0xF] = chip8->V[x] >= chip8->V[y];
            chip8->V[x] = chip8->V[x] - chip8->V[y];
            break;

        case 0x6:
            chip8->V[0xF] = chip8->V[x] & 0x1;
            chip8->V[x] >>= 1;
            break;

        case 0x7:
            chip8->V[0xF] = chip8->V[y] >= chip8->V[x];
            chip8->V[x] = chip8->V[y] - chip8->V[x];
            break;

        case 0xE:
            chip8->V[0xF] = (chip8->V[x] >> 7) & 0x1;
            chip8->V[x] <<= 1;
            break;
        }

        break;

    case 0x9:
        if (chip8->V[x] != chip8->V[y])
        {
            chip8->pc += 2;
        }
        break;

    case 0xA:
        chip8->I = nnn;
        break;

    case 0xB:
        chip8->pc = chip8->V[0] + nnn;
        break;

    case 0xC:
        chip8->V[x] = (rand() & 0xFF) & nn;
        break;

    case 0xD:
    {
        chip8->V[0xF] = 0;

        for (uint8_t row = 0; row < n; row++)
        {
            uint8_t sprite = chip8->memory[chip8->I + row];

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
                        chip8->V[0xF] = 1;

                    chip8->display[y_pos * 64 + x_pos] ^= 1;
                }
            }
        }

        break;
    }

    case 0xE:
        switch (nn)
        {
        case 0x9E:
            if (chip8->keypad[chip8->V[x]])
                chip8->pc += 2;
            break;

        case 0xA1:
            if (!chip8->keypad[chip8->V[x]])
                chip8->pc += 2;
            break;
        }
        break;

    case 0xF:
        switch (nn)
        {
        case 0x07:
            chip8->V[x] = chip8->delay_timer;
            break;
        }
        break;
    }
}