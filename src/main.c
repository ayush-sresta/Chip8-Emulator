#include <stdio.h>
#include "chip8.h"

int main(void)
{
    Chip8 chip8;

    chip8_init(&chip8);

    chip8_load_rom(&chip8, "roms/test.ch8");

    printf("PC: 0x%X\n", chip8.pc);
    // printf("Font byte: 0x%X\n", chip8.memory[0x95]);


    return 0;
}