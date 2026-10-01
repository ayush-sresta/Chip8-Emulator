#include <stdio.h>
#include "chip8.h"

int main(void)
{
    Chip8 chip8;

    chip8_init(&chip8);
    chip8_load_rom(&chip8, "roms/test.ch8");

    __uint16_t opcode = chip8_fetch_opcode(&chip8);

    printf("Opcode: 0x%04X\n", opcode);
    printf("First ROM byte: 0x%02X\n", chip8.memory[0x200]);

    return 0;
}