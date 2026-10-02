#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint8_t memory[4096];

    uint8_t V[16];

    uint16_t I;
    uint16_t pc;

    uint16_t stack[16];
    uint8_t sp;

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t display[64 * 32];

    uint8_t keypad[16];

    // FX0A
    uint8_t waiting_for_key;
    uint8_t key_register;

} Chip8;

void chip8_init(Chip8 *chip8);

void chip8_load_rom(Chip8 *chip8, const char *filename);

uint16_t chip8_fetch_opcode(Chip8 *chip8);

void chip8_execute(Chip8 *chip8, uint16_t opcode);

void chip8_key_press(Chip8 *chip8, uint8_t key);

void chip8_key_release(Chip8 *chip8, uint8_t key);

void chip8_update_timers(Chip8 *chip8);

#endif