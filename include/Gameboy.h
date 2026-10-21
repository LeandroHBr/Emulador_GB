#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdint.h>
#include "system_memory.h"
#include "registers.h"
#include <string.h>
#include "PPU.h"


struct Gameboy
{
    struct System_Memory SYS_MEMORY;
    struct Registers Register;
    struct PPU PPU;    
    // Teclado
    // APU 
};


void CPU_Init(struct Gameboy *gb);
void CPU_load_Rom(struct Gameboy *gb, const char* rom, size_t size);
void CPU_exec(struct Gameboy *gb,uint8_t opcode);

#endif