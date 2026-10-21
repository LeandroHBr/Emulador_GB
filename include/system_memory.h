#ifndef SYSTEM_MEMORY
#define SYSTEM_MEMORY
#include <stdint.h>
#include "config.h"

struct Gameboy;

struct System_Memory
{
    uint8_t  Memory[MEMORY_SIZE]; 
};

uint8_t System_Memory_Get(struct System_Memory *memory,uint16_t index);
void System_Memory_PUT(struct System_Memory *memory,uint16_t index, uint8_t value);
void Stack_Push(struct Gameboy *gb, uint16_t value);
uint16_t Stack_Pop(struct Gameboy* gb);

#endif