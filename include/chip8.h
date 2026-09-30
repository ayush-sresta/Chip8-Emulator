#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>

typedef struct {
    __uint8_t memory[4096];

    __uint8_t V[16];

    __uint16_t I;
    __uint16_t pc;

    __uint8_t stack[16];
    __uint8_t sp;

    __uint8_t delay_timer;
    __uint8_t sound_timer;

    __uint8_t display[64 * 32];

    __uint8_t keypad[16];
} Chip8;

#endif