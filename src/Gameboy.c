#include "Gameboy.h"
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <system_memory.h>

void CPU_Init(struct Gameboy *gb){
    memset(gb,0,sizeof(struct Gameboy));
    gb->Register.SP = STACK_INITIAL_POSICION;
}

void CPU_load_Rom(struct Gameboy *gb, const char* rom, size_t size){
    size_t load_size = (size > ROM_MAX_SIZE) ? ROM_MAX_SIZE : size;
    assert(load_size <= ROM_MAX_SIZE);
    memcpy(&gb->SYS_MEMORY.Memory[0], rom, load_size);
}

void LD_r16_n16(struct Gameboy *gb,uint16_t *r16){
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    *r16 = (msb << 8) | lsb;
}

void LD_r16_A(struct Gameboy *gb,uint16_t *r16){
    uint8_t value = gb->Register.a;
    System_Memory_PUT(&gb->SYS_MEMORY,r16,value);
    gb->Register.PC += 1;
}

void INC_r16(struct Gameboy *gb,uint16_t *r16){
    *r16 +=1;
    gb->Register.PC ++;
}

void INC_r8(struct Gameboy *gb,uint8_t *r8){
    *r8 +=1;
    gb->Register.N = 0;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.H = ((*r8 & 0x0F) == 0x00) ? 1 : 0;
    gb->Register.PC++;
}

void Dec_r8(struct Gameboy *gb,uint8_t *r8){
    *r8 -=1;
    gb->Register.N = 1;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.H = ((*r8 & 0x0f) == 0x0F) ? 1 : 0;
    gb->Register.PC ++;
}

void LD_r8_n8(struct Gameboy *gb,uint8_t *r8){
    *r8 = gb->SYS_MEMORY.Memory[gb->Register.PC++];
}

void RLCA(struct Gameboy *gb){
    uint8_t msb = (gb->Register.a & 0x80) >> 7;
    gb->Register.C = msb;
    gb->Register.a = (gb->Register.a << 1) | msb;

    gb->Register.Z = 0; 
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.PC++;
}

void LD_a16_SP(struct Gameboy *gb){
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint16_t address = (msb << 8) | lsb;
    System_Memory_PUT(&gb->SYS_MEMORY,address,gb->Register.SP);
    System_Memory_PUT(&gb->SYS_MEMORY,address+1,(gb->Register.SP >> 8) & 0xFF);
}

void ADD_HL_r16(struct Gameboy *gb,uint16_t *r16){
    uint16_t hl = gb->Register.hl;
    gb->Register.hl += *r16;
    gb->Register.N=0;
    gb->Register.C = (gb->Register.hl < hl || gb->Register.hl < *r16) ? 1 : 0;
    uint16_t sum_12_bits  = (gb->Register.hl & 0XFFF) + (*r16 & 0XFFF);
    gb->Register.H = ((sum_12_bits & 0xF000) != 0x0000) ? 1 : 0; 
    gb->Register.PC ++;
    // 11111 111111111111 
}


void LD_A_r16(struct Gameboy *gb,uint16_t *r16){
    gb->Register.a = System_Memory_Get(&gb->SYS_MEMORY,*r16);
    gb->Register.PC ++;

}

void Dec_r16(struct Gameboy *gb,uint16_t *r16){
    *r16--;
    gb->Register.PC ++;
}

void RRCA(struct Gameboy *gb){
    uint8_t lsb = (gb->Register.a & 0x01);
    gb->Register.C = lsb;
    gb->Register.a = (gb->Register.a >> 1) | lsb << 15;

    gb->Register.Z = 0; 
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.PC++;
}

//void Stop_n8(){}
void RLA(struct Gameboy *gb){
    uint8_t msb = (gb->Register.a & 0x80) >> 7;
    gb->Register.a = (gb->Register.a << 1) | gb->Register.C;
    gb->Register.C = msb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = 0;
    gb->Register.PC ++;
}

void RLC_r8(struct Gameboy *gb,uint8_t *r8){
    uint8_t msb = (*r8 & 0x80) >> 7;
    *r8 = (*r8 << 1) | msb;
    gb->Register.C = msb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.PC ++;

}

void JR_e8(struct Gameboy *gb){
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    gb->Register.PC+=e8;
}

void RRA(struct Gameboy *gb){
    uint8_t lsb = (gb->Register.a & 0x01);
    gb->Register.a = (gb->Register.a >> 1) | (gb->Register.C << 7);
    gb->Register.C = lsb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = 0;
    gb->Register.PC ++;
}

void JR_NZ_e8(struct Gameboy *gb){
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    if(gb->Register.Z == 0){
    gb->Register.PC+=e8;
    }
}

void JR_Z_e8(struct Gameboy *gb){
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    if(gb->Register.Z == 1){
    gb->Register.PC+=e8;
    }
}

void JR_NC_e8(struct Gameboy *gb){
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    if(gb->Register.N == 0){
    gb->Register.PC+=e8;
    }
}

void JR_C_e8(struct Gameboy *gb){
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    if(gb->Register.C == 1){
    gb->Register.PC+=e8;
    }
}



void LD_HLI_A(struct Gameboy *gb){
    System_Memory_PUT(&gb->SYS_MEMORY,gb->Register.hl,gb->Register.a);
    gb->Register.hl++;
    gb->Register.PC += 1;
}


void LD_A_HLI(struct Gameboy *gb){
    gb->Register.a = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.hl++;
    gb->Register.PC += 1;
}

void LD_HLD_A(struct Gameboy *gb){
    System_Memory_PUT(&gb->SYS_MEMORY,gb->Register.hl,gb->Register.a);
    gb->Register.hl--;
    gb->Register.PC += 1;
}


void LD_A_HLD(struct Gameboy *gb){
    gb->Register.a = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.hl--;
    gb->Register.PC += 1;
}

void CPL(struct Gameboy *gb){
    gb->Register.a = ~(gb->Register.a);
    gb->Register.H = 1;
    gb->Register.N = 1;
    gb->Register.PC += 1;
}


void DAA(struct Gameboy *gb){
    uint8_t adjustment = 0;
    
    if(gb->Register.N){
        if (gb->Register.H == 1) adjustment += 0x06;
        if (gb->Register.C == 1) adjustment += 0x60;
        gb->Register.a -= adjustment;
    } else {
        if (gb->Register.H == 1 || (gb->Register.a & 0x0F) > 0x09) adjustment += 0x06;
        if (gb->Register.C == 1 || gb->Register.a > 0x99) {
            adjustment += 0x60;
            gb->Register.C = 1; 
        }
        gb->Register.a += adjustment;
    }

    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.H = 0; 
    gb->Register.PC++;
}

void LD_SP_n16(struct Gameboy *gb){
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    gb->Register.SP = (msb << 8) | lsb;
}


void INC_HL(struct Gameboy *gb){
    uint16_t addr = gb->Register.hl;
    uint8_t value = System_Memory_Get(&gb->SYS_MEMORY, addr) + 1;
    
    System_Memory_PUT(&gb->SYS_MEMORY, addr, value);
    
    gb->Register.Z = (value == 0) ? 1 : 0;
    gb->Register.N = 0; 
    gb->Register.H = ((value & 0x0F) == 0x00) ? 1 : 0;
    
    gb->Register.PC++;
}

void DEC_HL(struct Gameboy *gb){
    uint16_t addr = gb->Register.hl;
    uint8_t value = System_Memory_Get(&gb->SYS_MEMORY, addr) - 1;
    
    System_Memory_PUT(&gb->SYS_MEMORY, addr, value);
    
    gb->Register.Z = (value == 0) ? 1 : 0;
    gb->Register.N = 1; 
    gb->Register.H = ((value & 0x0F) == 0x0F) ? 1 : 0;
    
    gb->Register.PC++;
}



void CPU_exec(struct Gameboy *gb,uint8_t opcode){
    switch (opcode)
    {
    case 0x00:
        gb->Register.PC +=1;
        break;
    case 0x01: //LD BC
        LD_r16_n16(gb,&gb->Register.bc);
        break;
    case 0x02:
        LD_r16_A(gb,&gb->Register.bc); //L[BC],A
        break;
    case 0x03:
        INC_r16(gb,&gb->Register.bc);
        break;
    case 0x04:
        INC_r8(gb,&gb->Register.b);
        break;
    case 0x05:
        Dec_r8(gb,&gb->Register.b);
        break;
    case 0x06:
        LD_r8_n8(gb,&gb->Register.b);
        break;
    case 0x07:
        RLCA(gb);
        break;
    case 0x08:
        LD_a16_SP(gb);
        break;
    case 0x09:
        ADD_HL_r16(gb,&gb->Register.bc);
        break;
    case 0x0A:
        LD_A_r16(gb,&gb->Register.bc);
        break;
    case 0x0B:
        Dec_r16(gb,&gb->Register.bc);
        break;
    case 0x0C:
        INC_r8(gb,&gb->Register.c);
        break;
    case 0x0D:
        Dec_r8(gb,&gb->Register.c);
        break;
    case 0x0E:
        LD_r8_n8(gb,&gb->Register.c);
        break;
    case 0x0F:
        RRCA(gb);
        break;
    case 0x10:
        //Stop
        break;
    case 0x11:
        LD_r16_n16(gb,&gb->Register.de);
        break;
    case 0x12:
        LD_r16_A(gb,&gb->Register.de);
        break;
    case 0x13:
        INC_r16(gb,&gb->Register.de);
        break;
    case 0x14:
        INC_r8(gb,&gb->Register.d);
        break;
    case 0x15:
        Dec_r8(gb,&gb->Register.d);
        break;
    case 0x16:
        LD_r8_n8(gb,&gb->Register.d);
        break;
    case 0x17:
        RLA(gb);
        break;
    case 0x18:
        JR_e8(gb);
        break;
    case 0x19:
        ADD_HL_r16(gb,&gb->Register.de);
        break;
    case 0x1A:
        LD_A_r16(gb,&gb->Register.de);
        break;
    case 0x1B:
        Dec_r16(gb,&gb->Register.de);
        break;
    case 0x1C:
        INC_r8(gb,&gb->Register.e);
        break;
    case 0x1D:
        Dec_r8(gb,&gb->Register.e);
        break;
    case 0x1E:
        LD_r8_n8(gb,&gb->Register.e);
        break;
    case 0x1F:
        RRA(gb);
        break;
    case 0x20:
        JR_NZ_e8(gb);
        break;
    case 0x21:
        LD_r16_n16(gb,&gb->Register.hl);
        break;
    case 0x22:
        LD_HLI_A(gb);
        break;
    case 0x23:
        INC_r16(gb,&gb->Register.hl);
        break;
    case 0x24:
        INC_r8(gb,&gb->Register.h);
        break;
    case 0x25:
        Dec_r8(gb,&gb->Register.h);
        break;
    case 0x26:
        LD_r8_n8(gb,&gb->Register.h);
        break;
    case 0x27:
        DAA(gb);
        break;
    case 0x28:
        JR_Z_e8(gb);
        break;
    case 0x29:
        ADD_HL_r16(gb,&gb->Register.hl);
        break;
    case 0x2A:
        LD_A_HLI(gb);
        break;
    case 0x2B:
        Dec_r16(gb,&gb->Register.hl);
        break;
    case 0x2C:
        INC_r8(gb,&gb->Register.l);
        break;
    case 0x2D:
        Dec_r8(gb,&gb->Register.l);
        break;
    case 0x2E:
        LD_r8_n8(gb,&gb->Register.l);
        break;
    case 0x2F:
        CPL(gb);
        break;
    case 0x30:
        JR_NC_e8(gb);
        break;
    case 0x31:
        LD_SP_n16(gb);
        break;
    case 0x32:
        LD_HLD_A(gb);
        break;
    case 0x33:
        INC_r16(gb,&gb->Register.SP);
        break;
    case 0x34:
        INC_HL(gb);
        break;
    case 0x35:
        DEC_HL(gb);
        break;
    case 0x36:
        break;
    case 0x37:
        break;
    case 0x38:
        break;
    case 0x39:
        break;
    case 0x3A:
        break;
    case 0x3B:
        break;
    case 0x3C:
        break;
    case 0x3D:
        break;
    case 0x3E:
        break;
    case 0x3F:
        break;
    case 0x40:
        break;
    
    }
}