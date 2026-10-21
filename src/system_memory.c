#include <stdint.h>
#include "system_memory.h"
#include <memory.h>
#include <registers.h>
#include "Gameboy.h"

uint8_t System_Memory_Get(struct System_Memory *memory,uint16_t index){
    return memory->Memory[index];
}


void System_Memory_PUT(struct System_Memory *memory,uint16_t index, uint8_t value){
    memory->Memory[index] = value;
}

void Stack_Push(struct Gameboy *gb, uint16_t value){
    gb->Register.SP--;
    System_Memory_PUT(&gb->SYS_MEMORY,gb->Register.SP--,(value >> 8) & 0xFF);
    gb->Register.SP--;
    System_Memory_PUT(&gb->SYS_MEMORY,gb->Register.SP,value & 0xFF);

}

uint16_t Stack_Pop(struct Gameboy *gb){
    
    uint8_t low = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.SP);
    gb->Register.SP++;
    
    uint8_t high = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.SP);
    gb->Register.SP++;
    
    return (high << 8) | low;
    
}
