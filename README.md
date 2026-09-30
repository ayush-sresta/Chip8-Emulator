# CHIP-8 Emulator

A CHIP-8 emulator written from scratch in C.

## About

This project is a simple CHIP-8 emulator built to explore how a virtual machine works at a low level. It implements the CHIP-8 CPU, memory, registers, stack, timers, display, and input.

## Features

* CHIP-8 instruction set
* 4 KB memory
* 16 general-purpose registers
* Index register
* Program counter
* 16-level stack
* Delay and sound timers
* 64×32 display
* 16-key keypad
* ROM loading
* SDL2 rendering and input

## Built With

* C
* CMake
* SDL2

## Build

```bash
git clone <repository-url>
cd chip8

mkdir build
cd build
cmake ..
cmake --build .
```

## Run

```bash
./chip8 path/to/rom.ch8
```
