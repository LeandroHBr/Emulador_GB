#ifndef CONFIG_H
#define CONFIG_H

#define TITLE "Gameboy Emulator"
#define WIDTH 160
#define HEIGHT 144
#define MULTIPLIER 5
#define MEMORY_SIZE 65536
#define PPU_MEMORY_SIZE 8192
// APU

#define Clock 4194304
#define System_Clock Clock/4
#define ROM_LOAD_ADDRES 0x0000
#define ROM_MAX_SIZE 0x7FFF
#define STACK_INITIAL_POSICION 0XFFFE

#endif