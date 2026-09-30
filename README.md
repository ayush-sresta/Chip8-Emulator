# CHIP-8 Emulator

A CHIP-8 emulator written in C.

## About

This is a small emulator built from scratch to understand how CHIP-8 works, from memory and registers to fetching and executing instructions.

## Features

* 4 KB memory
* 16 general-purpose registers
* Index register
* Program counter
* Stack
* Delay and sound timers
* 64×32 display
* 16-key keypad
* CHIP-8 instruction set
* ROM loading

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

## CHIP-8 Architecture

```text
Memory       4096 bytes
Registers    V0 - VF
Index        I
PC           Program Counter
Stack        16 levels
Timers       Delay + Sound
Display      64 × 32
Keyboard     16 keys
```
