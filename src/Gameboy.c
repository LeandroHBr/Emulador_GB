#include "Gameboy.h"
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <system_memory.h>

void CPU_Init(struct Gameboy *gb)
{
    memset(gb, 0, sizeof(struct Gameboy));
    gb->Register.SP = STACK_INITIAL_POSICION;
}

void CPU_load_Rom(struct Gameboy *gb, const char *rom, size_t size)
{
    size_t load_size = (size > ROM_MAX_SIZE) ? ROM_MAX_SIZE : size;
    assert(load_size <= ROM_MAX_SIZE);
    memcpy(&gb->SYS_MEMORY.Memory[0], rom, load_size);
}

void LD_r16_n16(struct Gameboy *gb, uint16_t *r16)
{
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    *r16 = (msb << 8) | lsb;
}

void LD_r8_r8(struct Gameboy *gb, uint8_t *r8_1, uint8_t *r8_2)
{
    *r8_1 = *r8_2;
    gb->Register.PC++;
}

// corrigir
void LD_r16_A(struct Gameboy *gb, uint16_t *r16)
{
    uint8_t value = gb->Register.a;
    // System_Memory_PUT(&gb->SYS_MEMORY,r16,value);
    gb->Register.PC += 1;
}

void INC_r16(struct Gameboy *gb, uint16_t *r16)
{
    *r16 += 1;
    gb->Register.PC++;
}

void INC_r8(struct Gameboy *gb, uint8_t *r8)
{
    *r8 += 1;
    gb->Register.N = 0;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.H = ((*r8 & 0x0F) == 0x00) ? 1 : 0;
    gb->Register.PC++;
}

void Dec_r8(struct Gameboy *gb, uint8_t *r8)
{
    *r8 -= 1;
    gb->Register.N = 1;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.H = ((*r8 & 0x0f) == 0x0F) ? 1 : 0;
    gb->Register.PC++;
}

void LD_r8_n8(struct Gameboy *gb, uint8_t *r8)
{
    *r8 = gb->SYS_MEMORY.Memory[gb->Register.PC++];
}

void RLCA(struct Gameboy *gb)
{
    uint8_t msb = (gb->Register.a & 0x80) >> 7;
    gb->Register.C = msb;
    gb->Register.a = (gb->Register.a << 1) | msb;

    gb->Register.Z = 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.PC++;
}

void LD_a16_SP(struct Gameboy *gb)
{
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint16_t address = (msb << 8) | lsb;
    System_Memory_PUT(&gb->SYS_MEMORY, address, gb->Register.SP);
    System_Memory_PUT(&gb->SYS_MEMORY, address + 1, (gb->Register.SP >> 8) & 0xFF);
}

void ADD_HL_r16(struct Gameboy *gb, uint16_t *r16)
{
    uint16_t hl = gb->Register.hl;
    gb->Register.hl += *r16;
    gb->Register.N = 0;
    gb->Register.C = (gb->Register.hl < hl || gb->Register.hl < *r16) ? 1 : 0;
    uint16_t sum_12_bits = (gb->Register.hl & 0XFFF) + (*r16 & 0XFFF);
    gb->Register.H = ((sum_12_bits & 0xF000) != 0x0000) ? 1 : 0;
    gb->Register.PC++;
    // 11111 111111111111
}

void LD_A_r16(struct Gameboy *gb, uint16_t *r16)
{
    gb->Register.a = System_Memory_Get(&gb->SYS_MEMORY, *r16);
    gb->Register.PC++;
}

void Dec_r16(struct Gameboy *gb, uint16_t *r16)
{
    *r16--;
    gb->Register.PC++;
}

void RRCA(struct Gameboy *gb)
{
    uint8_t lsb = (gb->Register.a & 0x01);
    gb->Register.C = lsb;
    gb->Register.a = (gb->Register.a >> 1) | lsb << 15;

    gb->Register.Z = 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.PC++;
}

// void Stop_n8(){}
void RLA(struct Gameboy *gb)
{
    uint8_t msb = (gb->Register.a & 0x80) >> 7;
    gb->Register.a = (gb->Register.a << 1) | gb->Register.C;
    gb->Register.C = msb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = 0;
    gb->Register.PC++;
}

void RLC_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t msb = (*r8 & 0x80) >> 7;
    *r8 = (*r8 << 1) | msb;
    gb->Register.C = msb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = (*r8 == 0) ? 1 : 0;
    gb->Register.PC++;
}

void JR_e8(struct Gameboy *gb)
{
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    gb->Register.PC += e8;
}

void RRA(struct Gameboy *gb)
{
    uint8_t lsb = (gb->Register.a & 0x01);
    gb->Register.a = (gb->Register.a >> 1) | (gb->Register.C << 7);
    gb->Register.C = lsb;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.Z = 0;
    gb->Register.PC++;
}

void JR_NZ_e8(struct Gameboy *gb)
{
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    if (gb->Register.Z == 0)
    {
        gb->Register.PC += e8;
    }
}

void JR_Z_e8(struct Gameboy *gb)
{
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    if (gb->Register.Z == 1)
    {
        gb->Register.PC += e8;
    }
}

void JR_NC_e8(struct Gameboy *gb)
{
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    if (gb->Register.N == 0)
    {
        gb->Register.PC += e8;
    }
}

void JR_C_e8(struct Gameboy *gb)
{
    int8_t e8 = (int8_t)System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    if (gb->Register.C == 1)
    {
        gb->Register.PC += e8;
    }
}

void LD_HLI_r8(struct Gameboy *gb, uint8_t *r8)
{
    System_Memory_PUT(&gb->SYS_MEMORY, gb->Register.hl, *r8);
    gb->Register.hl++;
    gb->Register.PC += 1;
}

void LD_r8_HLI(struct Gameboy *gb, uint8_t *r8)
{
    *r8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.hl++;
    gb->Register.PC += 1;
}

void LD_HLD_r8(struct Gameboy *gb, uint8_t *r8)
{
    System_Memory_PUT(&gb->SYS_MEMORY, gb->Register.hl, *r8);
    gb->Register.hl--;
    gb->Register.PC += 1;
}

void LD_r8_HLD(struct Gameboy *gb, uint8_t *r8)
{
    *r8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.hl--;
    gb->Register.PC += 1;
}

void CPL(struct Gameboy *gb)
{
    gb->Register.a = ~(gb->Register.a);
    gb->Register.H = 1;
    gb->Register.N = 1;
    gb->Register.PC += 1;
}

void DAA(struct Gameboy *gb)
{
    uint8_t adjustment = 0;

    if (gb->Register.N)
    {
        if (gb->Register.H == 1)
            adjustment += 0x06;
        if (gb->Register.C == 1)
            adjustment += 0x60;
        gb->Register.a -= adjustment;
    }
    else
    {
        if (gb->Register.H == 1 || (gb->Register.a & 0x0F) > 0x09)
            adjustment += 0x06;
        if (gb->Register.C == 1 || gb->Register.a > 0x99)
        {
            adjustment += 0x60;
            gb->Register.C = 1;
        }
        gb->Register.a += adjustment;
    }

    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.H = 0;
    gb->Register.PC++;
}

void LD_SP_n16(struct Gameboy *gb)
{
    uint8_t lsb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    uint8_t msb = gb->SYS_MEMORY.Memory[gb->Register.PC++];
    gb->Register.SP = (msb << 8) | lsb;
}

void INC_HL(struct Gameboy *gb)
{
    uint16_t addr = gb->Register.hl;
    uint8_t value = System_Memory_Get(&gb->SYS_MEMORY, addr) + 1;

    System_Memory_PUT(&gb->SYS_MEMORY, addr, value);

    gb->Register.Z = (value == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = ((value & 0x0F) == 0x00) ? 1 : 0;

    gb->Register.PC++;
}

void DEC_HL(struct Gameboy *gb)
{
    uint16_t addr = gb->Register.hl;
    uint8_t value = System_Memory_Get(&gb->SYS_MEMORY, addr) - 1;

    System_Memory_PUT(&gb->SYS_MEMORY, addr, value);

    gb->Register.Z = (value == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((value & 0x0F) == 0x0F) ? 1 : 0;

    gb->Register.PC++;
}

void LD_HL_n8(struct Gameboy *gb)
{
    uint8_t n8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    System_Memory_PUT(&gb->SYS_MEMORY, gb->Register.hl, n8);
}

void LD_HL_r8(struct Gameboy *gb, uint8_t *r8)
{
    System_Memory_PUT(&gb->SYS_MEMORY, gb->Register.hl, *r8);
    gb->Register.PC++;
}

void LD_r8_HL(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t value = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    *r8 = value;
    gb->Register.PC++;
}

void SCF(struct Gameboy *gb)
{
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 1;
    gb->Register.PC++;
}

void CCF(struct Gameboy *gb)
{
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = ~(gb->Register.C);
    gb->Register.PC++;
}

void ADD_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t r8_val = *r8;
    uint8_t ra_val = gb->Register.a;
    uint16_t result = (r8_val + ra_val);
    gb->Register.a = (uint8_t)result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (r8_val & 0x0F)) & 0x10) ? 1 : 0;
    gb->Register.C = (result > 0xFF) ? 1 : 0;
    gb->Register.PC++;
}

void ADD_A_hl(struct Gameboy *gb)
{
    uint8_t hl_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    uint8_t ra_val = gb->Register.a;
    uint16_t result = (hl_val + ra_val);
    gb->Register.a = (uint8_t)result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (hl_val & 0x0F)) & 0x10) ? 1 : 0;
    gb->Register.C = (result > 0xFF) ? 1 : 0;
    gb->Register.PC++;
}

void ADC_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t r8_val = *r8;
    uint8_t ra_val = gb->Register.a;
    uint8_t result = (uint8_t)(r8_val + ra_val + gb->Register.C);
    gb->Register.a = result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (r8_val & 0x0F) + gb->Register.C) & 0x10) ? 1 : 0;
    gb->Register.C = (result < r8_val || result < ra_val) ? 1 : 0;
    gb->Register.PC++;
}

void ADC_A_hl(struct Gameboy *gb)
{
    uint8_t hl_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    uint8_t ra_val = gb->Register.a;
    uint8_t result = (uint8_t)(hl_val + ra_val + gb->Register.C);
    gb->Register.a = result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (hl_val & 0x0F) + gb->Register.C) & 0x10) ? 1 : 0;
    gb->Register.C = (result < hl_val || result < ra_val) ? 1 : 0;
    gb->Register.PC++;
}


void Sub_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t r8_val = *r8;
    uint8_t ra_val = gb->Register.a;
    gb->Register.a -= *r8;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((r8_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (r8_val > ra_val) ? 1 : 0;
    gb->Register.PC++;
}



void Sub_A_hl(struct Gameboy *gb)
{
    uint8_t hl_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    uint8_t ra_val = gb->Register.a;
    gb->Register.a -= hl_val;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((hl_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (hl_val > ra_val) ? 1 : 0;
    gb->Register.PC++;
}




void SubC_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint16_t r8_plus_C_val = *r8 + gb->Register.C;
    uint8_t ra_val = gb->Register.a;

    int16_t result_full = (int16_t)ra_val - r8_plus_C_val;
    gb->Register.a = (uint8_t)result_full;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((r8_plus_C_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}

void SubC_A_hl(struct Gameboy *gb)
{
    uint16_t hl_plus_C_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl) + gb->Register.C;
    uint8_t ra_val = gb->Register.a;

    int16_t result_full = (int16_t)ra_val - hl_plus_C_val;
    gb->Register.a = (uint8_t)result_full;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((hl_plus_C_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}

void And_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    gb->Register.a &= *r8;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 1;
    gb->Register.C = 0;
    gb->Register.PC++;
}

void And_A_Hl(struct Gameboy *gb)
{
    gb->Register.a &= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 1;
    gb->Register.C = 0;
    gb->Register.PC++;
}



void XOR_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    gb->Register.a ^= *r8;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;
}

void XOR_A_hl(struct Gameboy *gb)
{
    gb->Register.a ^= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;
}


void OR_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    gb->Register.a |= *r8;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;
}

void OR_A_hl(struct Gameboy *gb)
{
    gb->Register.a |= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;
}


void CP_A_r8(struct Gameboy *gb, uint8_t *r8)
{
    uint8_t r8_val = *r8;
    uint8_t ra_val = gb->Register.a;
    int16_t result_full = (int16_t)ra_val - r8_val;

    gb->Register.Z = (result_full == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((r8_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}

void CP_A_hl(struct Gameboy *gb)
{
    uint8_t hl_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.hl);
    uint8_t ra_val = gb->Register.a;
    int16_t result_full = (int16_t)ra_val - hl_val;

    gb->Register.Z = (result_full == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((hl_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}


void RET_NZ(struct Gameboy *gb)
{

    if (gb->Register.Z == 0)
    {
        gb->Register.PC = Stack_Pop(gb);
    }
    else
    {
        gb->Register.PC += 1;
    }
}

void RET_Z(struct Gameboy *gb)
{

    if (gb->Register.Z == 1)
    {
        gb->Register.PC = Stack_Pop(gb);
    }
    else
    {
        gb->Register.PC += 1;
    }
}

void RET_NC(struct Gameboy *gb)
{

    if (gb->Register.C == 0)
    {
        gb->Register.PC = Stack_Pop(gb);
    }
    else
    {
        gb->Register.PC += 1;
    }
}

void RET_C(struct Gameboy *gb)
{

    if (gb->Register.C == 1)
    {
        gb->Register.PC = Stack_Pop(gb);
    }
    else
    {
        gb->Register.PC += 1;
    }
}

void RET(struct Gameboy *gb)
{
    gb->Register.PC = Stack_Pop(gb);
}

void POP_R16(struct Gameboy *gb, uint16_t *r16){
    *r16 = Stack_Pop(gb);
    gb->Register.PC++;
}

void PUSH_R16(struct Gameboy *gb, uint16_t *r16){
    Stack_Push(gb,*r16);
    gb->Register.PC++;
}




void JP_NZ_a16(struct Gameboy *gb){
    if (gb->Register.Z == 0)
    {
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void JP_Z_a16(struct Gameboy *gb){
    if (gb->Register.Z == 1)
    {
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void JP_NC_a16(struct Gameboy *gb){
    if (gb->Register.C == 0)
    {
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void JP_C_a16(struct Gameboy *gb){
    if (gb->Register.C == 1)
    {
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void JP_a16(struct Gameboy *gb){
    uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC++);
    gb->Register.PC = (msb << 8) | lsb;
}

void CALL_NZ_a16(struct Gameboy *gb){
    if (gb->Register.Z == 0)
    {
        uint16_t return_addr = gb->Register.PC + 3;
        Stack_Push(gb,return_addr);
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 1);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 2);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void CALL_Z_a16(struct Gameboy *gb){
    if (gb->Register.Z == 1)
    {
        uint16_t return_addr = gb->Register.PC + 3;
        Stack_Push(gb,return_addr);
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 1);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 2);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}


void CALL_NC_a16(struct Gameboy *gb){
    if (gb->Register.C == 0)
    {
        uint16_t return_addr = gb->Register.PC + 3;
        Stack_Push(gb,return_addr);
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 1);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 2);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void CALL_C_a16(struct Gameboy *gb){
    if (gb->Register.C == 1)
    {
        uint16_t return_addr = gb->Register.PC + 3;
        Stack_Push(gb,return_addr);
        uint8_t lsb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 1);
        uint8_t msb = System_Memory_Get(&gb->SYS_MEMORY,gb->Register.PC + 2);
        gb->Register.PC = (msb << 8) | lsb;
    }
    else
    {
        gb->Register.PC += 3;
    }
}

void ADD_A_n8(struct Gameboy *gb){
    uint8_t n8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    uint8_t ra_val = gb->Register.a;
    uint16_t result = (n8 + ra_val);
    gb->Register.a = (uint8_t)result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (n8 & 0x0F)) & 0x10) ? 1 : 0;
    gb->Register.C = (result > 0xFF) ? 1 : 0;
    gb->Register.PC ++;
}


void SUB_A_n8(struct Gameboy *gb){
    uint8_t n8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    uint8_t ra_val = gb->Register.a;
    gb->Register.a -= n8;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((n8 & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (n8 > ra_val) ? 1 : 0;
    gb->Register.PC++;

}

void AND_A_n8(struct Gameboy *gb){
    gb->Register.a &= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 1;
    gb->Register.C = 0;
    gb->Register.PC++;

}

void OR_A_n8(struct Gameboy *gb){
    gb->Register.a |= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;
}

void ADC_A_n8(struct Gameboy *gb){
    uint8_t n8 = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    uint8_t ra_val = gb->Register.a;
    uint8_t result = (uint8_t)(n8 + ra_val + gb->Register.C);
    gb->Register.a = result;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = (((ra_val & 0x0F) + (n8 & 0x0F) + gb->Register.C) & 0x10) ? 1 : 0;
    gb->Register.C = (result < n8 || result < ra_val) ? 1 : 0;
    gb->Register.PC++;
}

void SubC_A_n8(struct Gameboy *gb)
{
    uint16_t n8_plus_C_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++) + gb->Register.C;
    uint8_t ra_val = gb->Register.a;

    int16_t result_full = (int16_t)ra_val - n8_plus_C_val;
    gb->Register.a = (uint8_t)result_full;
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((n8_plus_C_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}

void XOR_A_n8(struct Gameboy *gb){
    gb->Register.a ^= System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    gb->Register.Z = (gb->Register.a == 0) ? 1 : 0;
    gb->Register.N = 0;
    gb->Register.H = 0;
    gb->Register.C = 0;
    gb->Register.PC++;

}


void CP_A_n8(struct Gameboy *gb)
{
    uint8_t n8_val = System_Memory_Get(&gb->SYS_MEMORY, gb->Register.PC++);
    uint8_t ra_val = gb->Register.a;
    int16_t result_full = (int16_t)ra_val - n8_val;

    gb->Register.Z = (result_full == 0) ? 1 : 0;
    gb->Register.N = 1;
    gb->Register.H = ((n8_val & 0x0F) > (ra_val & 0x0F)) ? 1 : 0;
    gb->Register.C = (result_full < 0) ? 1 : 0;
    gb->Register.PC++;
}

void RST_vec(struct Gameboy *gb, uint16_t vec){
    
    Stack_Push(gb, gb->Register.PC + 1);    
    gb->Register.PC = vec;
}


void LDH_a8_A(struct Gameboy *gb){

}


void CPU_exec(struct Gameboy *gb, uint8_t opcode)
{
    switch (opcode)
    {
    case 0x00:
        gb->Register.PC += 1;
        break;
    case 0x01: // LD BC
        LD_r16_n16(gb, &gb->Register.bc);
        break;
    case 0x02:
        LD_r16_A(gb, &gb->Register.bc); // L[BC],A
        break;
    case 0x03:
        INC_r16(gb, &gb->Register.bc);
        break;
    case 0x04:
        INC_r8(gb, &gb->Register.b);
        break;
    case 0x05:
        Dec_r8(gb, &gb->Register.b);
        break;
    case 0x06:
        LD_r8_n8(gb, &gb->Register.b);
        break;
    case 0x07:
        RLCA(gb);
        break;
    case 0x08:
        LD_a16_SP(gb);
        break;
    case 0x09:
        ADD_HL_r16(gb, &gb->Register.bc);
        break;
    case 0x0A:
        LD_A_r16(gb, &gb->Register.bc);
        break;
    case 0x0B:
        Dec_r16(gb, &gb->Register.bc);
        break;
    case 0x0C:
        INC_r8(gb, &gb->Register.c);
        break;
    case 0x0D:
        Dec_r8(gb, &gb->Register.c);
        break;
    case 0x0E:
        LD_r8_n8(gb, &gb->Register.c);
        break;
    case 0x0F:
        RRCA(gb);
        break;
    case 0x10:
        // Stop
        break;
    case 0x11:
        LD_r16_n16(gb, &gb->Register.de);
        break;
    case 0x12:
        LD_r16_A(gb, &gb->Register.de);
        break;
    case 0x13:
        INC_r16(gb, &gb->Register.de);
        break;
    case 0x14:
        INC_r8(gb, &gb->Register.d);
        break;
    case 0x15:
        Dec_r8(gb, &gb->Register.d);
        break;
    case 0x16:
        LD_r8_n8(gb, &gb->Register.d);
        break;
    case 0x17:
        RLA(gb);
        break;
    case 0x18:
        JR_e8(gb);
        break;
    case 0x19:
        ADD_HL_r16(gb, &gb->Register.de);
        break;
    case 0x1A:
        LD_A_r16(gb, &gb->Register.de);
        break;
    case 0x1B:
        Dec_r16(gb, &gb->Register.de);
        break;
    case 0x1C:
        INC_r8(gb, &gb->Register.e);
        break;
    case 0x1D:
        Dec_r8(gb, &gb->Register.e);
        break;
    case 0x1E:
        LD_r8_n8(gb, &gb->Register.e);
        break;
    case 0x1F:
        RRA(gb);
        break;
    case 0x20:
        JR_NZ_e8(gb);
        break;
    case 0x21:
        LD_r16_n16(gb, &gb->Register.hl);
        break;
    case 0x22:
        LD_HLI_r8(gb, &gb->Register.a);
        break;
    case 0x23:
        INC_r16(gb, &gb->Register.hl);
        break;
    case 0x24:
        INC_r8(gb, &gb->Register.h);
        break;
    case 0x25:
        Dec_r8(gb, &gb->Register.h);
        break;
    case 0x26:
        LD_r8_n8(gb, &gb->Register.h);
        break;
    case 0x27:
        DAA(gb);
        break;
    case 0x28:
        JR_Z_e8(gb);
        break;
    case 0x29:
        ADD_HL_r16(gb, &gb->Register.hl);
        break;
    case 0x2A:
        LD_r8_HLI(gb, &gb->Register.a);
        break;
    case 0x2B:
        Dec_r16(gb, &gb->Register.hl);
        break;
    case 0x2C:
        INC_r8(gb, &gb->Register.l);
        break;
    case 0x2D:
        Dec_r8(gb, &gb->Register.l);
        break;
    case 0x2E:
        LD_r8_n8(gb, &gb->Register.l);
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
        LD_HLD_r8(gb, &gb->Register.a);
        break;
    case 0x33:
        INC_r16(gb, &gb->Register.SP);
        break;
    case 0x34:
        INC_HL(gb);
        break;
    case 0x35:
        DEC_HL(gb);
        break;
    case 0x36:
        LD_HL_n8(gb);
        break;
    case 0x37:
        SCF(gb);
        break;
    case 0x38:
        JR_C_e8(gb);
        break;
    case 0x39:
        ADD_HL_r16(gb, &gb->Register.SP);
        break;
    case 0x3A:
        LD_r8_HLD(gb, &gb->Register.a);
        break;
    case 0x3B:
        Dec_r16(gb, &gb->Register.SP);
        break;
    case 0x3C:
        INC_r8(gb, &gb->Register.a);
        break;
    case 0x3D:
        Dec_r8(gb, &gb->Register.a);
        break;
    case 0x3E:
        LD_r8_n8(gb, &gb->Register.a);
        break;
    case 0x3F:
        CCF(gb);
        break;
    case 0x40:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.b);
        break;
    case 0x41:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.c);
        break;
    case 0x42:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.d);
        break;
    case 0x43:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.e);
        break;
    case 0x44:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.h);
        break;
    case 0x45:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.l);
        break;
    case 0x46:
        LD_r8_HL(gb, &gb->Register.b);
        break;
    case 0x47:
        LD_r8_r8(gb, &gb->Register.b, &gb->Register.a);
        break;
    case 0x48:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.b);
        break;
    case 0x49:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.c);
        break;
    case 0x4A:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.d);
        break;
    case 0x4B:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.e);
        break;
    case 0x4C:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.h);
        break;
    case 0x4D:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.l);
        break;
    case 0x4E:
        LD_r8_HL(gb, &gb->Register.c);
        break;
    case 0x4F:
        LD_r8_r8(gb, &gb->Register.c, &gb->Register.a);
        break;
    case 0x50:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.b);
        break;
    case 0x51:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.c);
        break;
    case 0x52:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.d);
        break;
    case 0x53:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.e);
        break;
    case 0x54:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.h);
        break;
    case 0x55:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.l);
        break;
    case 0x56:
        LD_r8_HL(gb, &gb->Register.d);
        break;
    case 0x57:
        LD_r8_r8(gb, &gb->Register.d, &gb->Register.a);
        break;
    case 0x58:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.b);
        break;
    case 0x59:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.c);
        break;
    case 0x5A:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.d);
        break;
    case 0x5B:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.e);
        break;
    case 0x5C:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.h);
        break;
    case 0x5D:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.l);
        break;
    case 0x5E:
        LD_r8_HL(gb, &gb->Register.e);
        break;
    case 0x5F:
        LD_r8_r8(gb, &gb->Register.e, &gb->Register.l);
        break;
    case 0x60:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.b);
        break;
    case 0x61:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.c);
        break;
    case 0x62:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.d);
        break;
    case 0x63:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.e);
        break;
    case 0x64:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.h);
        break;
    case 0x65:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.l);
        break;
    case 0x66:
        LD_r8_HL(gb, &gb->Register.h);
        break;
    case 0x67:
        LD_r8_r8(gb, &gb->Register.h, &gb->Register.a);
        break;
    case 0x68:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.b);
        break;
    case 0x69:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.c);
        break;
    case 0x6A:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.d);
        break;
    case 0x6B:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.e);
        break;
    case 0x6C:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.h);
        break;
    case 0x6D:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.l);
        break;
    case 0x6E:
        LD_r8_HL(gb, &gb->Register.l);
        break;
    case 0x6F:
        LD_r8_r8(gb, &gb->Register.l, &gb->Register.a);
        break;
    case 0x70:
        LD_HL_r8(gb, &gb->Register.b);
        break;
    case 0x71:
        LD_HL_r8(gb, &gb->Register.c);
        break;
    case 0x72:
        LD_HL_r8(gb, &gb->Register.d);
        break;
    case 0x73:
        LD_HL_r8(gb, &gb->Register.e);
        break;
    case 0x74:
        LD_HL_r8(gb, &gb->Register.h);
        break;
    case 0x75:
        LD_HL_r8(gb, &gb->Register.l);
        break;
    case 0x76:
        // halt
        break;
    case 0x77:
        LD_HL_r8(gb, &gb->Register.a);
        break;
    case 0x78:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.b);
        break;
    case 0x79:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.c);
        break;
    case 0x7A:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.d);
        break;
    case 0x7B:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.e);
        break;
    case 0x7C:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.h);
        break;
    case 0x7D:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.l);
        break;
    case 0x7E:
        LD_r8_HL(gb, &gb->Register.a);
        break;
    case 0x7F:
        LD_r8_r8(gb, &gb->Register.a, &gb->Register.a);
        break;
    case 0x80:
        ADD_A_r8(gb,&gb->Register.b);
        break;
    case 0x81:
        ADD_A_r8(gb,&gb->Register.c);
        break;
    case 0x82:
        ADD_A_r8(gb,&gb->Register.d);
        break;
    case 0x83:
        ADD_A_r8(gb,&gb->Register.e);
        break;
    case 0x84:
        ADD_A_r8(gb,&gb->Register.h);
        break;
    case 0x85:
        ADD_A_r8(gb,&gb->Register.l);
        break;
    case 0x86:
        ADD_A_hl(gb);
        break;
    case 0x87:
        ADD_A_r8(gb,&gb->Register.a);
        break;
    case 0x88:
        ADC_A_r8(gb,&gb->Register.b);
        break;
    case 0x89:
        ADC_A_r8(gb,&gb->Register.c);
        break;
    case 0x8A:
        ADC_A_r8(gb,&gb->Register.d);
        break;
    case 0x8B:
        ADC_A_r8(gb,&gb->Register.e);
        break;
    case 0x8C:
        ADC_A_r8(gb,&gb->Register.h);
        break;
    case 0x8D:
        ADC_A_r8(gb,&gb->Register.l);
        break;
    case 0x8E:
        ADC_A_hl(gb);
        break;
    case 0x8F:
        ADC_A_r8(gb,&gb->Register.a);
        break;
    case 0x90:
        Sub_A_r8(gb,&gb->Register.b);
        break;
    case 0x91:
        Sub_A_r8(gb,&gb->Register.c);
        break;
    case 0x92:
        Sub_A_r8(gb,&gb->Register.d);
        break;
    case 0x93:
        Sub_A_r8(gb,&gb->Register.e);
        break;
    case 0x94:
        Sub_A_r8(gb,&gb->Register.h);
        break;
    case 0x95:
        Sub_A_r8(gb,&gb->Register.l);
        break;
    case 0x96:
        Sub_A_hl(gb);
        break;
    case 0x97:
        Sub_A_r8(gb,&gb->Register.a);
        break;
    case 0x98:
        SubC_A_r8(gb,&gb->Register.b);
        break;
    case 0x99:
        SubC_A_r8(gb,&gb->Register.c);
        break;
    case 0x9A:
        SubC_A_r8(gb,&gb->Register.d);
        break;
    case 0x9B:
        SubC_A_r8(gb,&gb->Register.e);
        break;
    case 0x9C:
        SubC_A_r8(gb,&gb->Register.h);
        break;
    case 0x9D:
        SubC_A_r8(gb,&gb->Register.l);
        break;
    case 0x9E:
        SubC_A_hl(gb);
        break;
    case 0x9F:
        SubC_A_r8(gb,&gb->Register.a);        
        break;
    case 0xA0:
        And_A_r8(gb,&gb->Register.b); 
        break;
    case 0xA1:
        And_A_r8(gb,&gb->Register.c); 
        break;
    case 0xA2:
        And_A_r8(gb,&gb->Register.d); 
        break;
    case 0xA3:
        And_A_r8(gb,&gb->Register.e); 
        break;
    case 0xA4:
        And_A_r8(gb,&gb->Register.h); 
        break;
    case 0xA5:
        And_A_r8(gb,&gb->Register.l); 
        break;
    case 0xA6:
        And_A_Hl(gb);
        break;
    case 0xA7:
        And_A_r8(gb,&gb->Register.a); 
        break;
    case 0xA8:
        XOR_A_r8(gb,&gb->Register.b);
        break;
    case 0xA9:
        XOR_A_r8(gb,&gb->Register.c);
        break;
    case 0xAA:
        XOR_A_r8(gb,&gb->Register.d);
        break;
    case 0xAB:
        XOR_A_r8(gb,&gb->Register.e);
        break;
    case 0xAC:
        XOR_A_r8(gb,&gb->Register.h);
        break;
    case 0xAD:
        XOR_A_r8(gb,&gb->Register.l);
        break;
    case 0xAE:
        XOR_A_hl(gb);
        break;
    case 0xAF:
        XOR_A_r8(gb,&gb->Register.a);
        break;
    case 0xB0:
        OR_A_r8(gb,&gb->Register.b);
        break;
    case 0xB1:
        OR_A_r8(gb,&gb->Register.c);
        break;
    case 0xB2:
        OR_A_r8(gb,&gb->Register.d);
        break;
    case 0xB3:
        OR_A_r8(gb,&gb->Register.e);
        break;
    case 0xB4:
        OR_A_r8(gb,&gb->Register.h);
        break;
    case 0xB5:
        OR_A_r8(gb,&gb->Register.l);
        break;
    case 0xB6:
        OR_A_hl(gb);
        break;
    case 0xB7:
        OR_A_r8(gb,&gb->Register.a);
        break;
    case 0xB8:
        CP_A_r8(gb,&gb->Register.b);
        break;
    case 0xB9:
        CP_A_r8(gb,&gb->Register.c);
        break;
    case 0xBA:
        CP_A_r8(gb,&gb->Register.d);
        break;
    case 0xBB:
        CP_A_r8(gb,&gb->Register.e);
        break;
    case 0xBC:
        CP_A_r8(gb,&gb->Register.h);
        break;
    case 0xBD:
        CP_A_r8(gb,&gb->Register.l);
        break;
    case 0xBE:
        CP_A_hl(gb);
        break;
    case 0xBF:
        CP_A_r8(gb,&gb->Register.a);
        break;  
    case 0xC0:
        RET_NZ(gb);
        break;  
    case 0xC1:
        POP_R16(gb,&gb->Register.bc);
        break;  
    case 0xC2:
        JP_NZ_a16(gb);
        break;  
    case 0xC3:
        JP_a16(gb);
        break;  
    case 0xC4:
        CALL_NZ_a16(gb);
        break;  
    case 0xC5:
        PUSH_R16(gb,&gb->Register.bc);
        break;  
    case 0xC6:
        ADD_A_n8(gb);
        break;  
    case 0xC7:
        RST_vec(gb,0x00);  
        break;  
    case 0xC8:
        RET_Z(gb);
        break;  
    case 0xC9:
        RET(gb);
        break;  
    case 0xCA:
        JP_Z_a16(gb);
        break;  
    case 0xCB:
        //prefix
        break;  
    case 0xCC:
        CALL_Z_a16(gb);
        break;  
    case 0xCD:
        //call
        break;  
    case 0xCE:
        ADC_A_n8(gb);
        break;  
    case 0xCF:
        RST_vec(gb,0x08);
        break;  
    case 0xD0:
        RET_NC(gb);
        break;  
    case 0xD1:
        POP_R16(gb,&gb->Register.de);
        break;  
    case 0xD2:
        JP_NC_a16(gb);
        break;  
    case 0xD3:
        // sem instrução
        break; 
    case 0xD4:
        CALL_NC_a16(gb);
        break;
    case 0xD5:
        PUSH_R16(gb,&gb->Register.de);
        break;
    case 0xD6:
        SUB_A_n8(gb);
        break;
    case 0xD7:
        RST_vec(gb,0x10);
        break;
    case 0xD8:
        RET_C(gb);
        break;
    case 0xD9:
        // RETI
        break;
    case 0xDA:
        JP_C_a16(gb);
        break;
    case 0xDB:
        // sem instrução
        break;
    case 0xDC:
        CALL_C_a16(gb);
        break;
    case 0xDD:
        // call DD
        break;
    case 0xDE:
        SubC_A_n8(gb);
        break;
    case 0xDF:
        RST_vec(gb,0x18);
        break;
    case 0xE0:
        
        break;
    case 0xE1:
        break;
    case 0xE2:
        break;
    case 0xE3:
        break;
    case 0xE4:
        break;
    case 0xE5:
        break; 
    case 0xE6:
        break;
    case 0xE7:
        break;
    case 0xE8:
        break;
    case 0xE9:
        break;
    case 0xEA:
        break;
    case 0xEB:
        break;
    case 0xEC:
        break;
    case 0xED:
        break;
    case 0xEE:
        break;
    case 0xEF:
        break;
    case 0xF0:
        break;
    case 0xF1:
        break;
    case 0xF2:
        break;
    case 0xF3:
        break;
    case 0xF4:
        break;
    case 0xF5:
        break;
    case 0xF6:
        break;
    case 0xF7:
        break;
    case 0xF8:
        break;
    case 0xF9:
        break;
    case 0xFA:
        break;
    case 0xFB:
        break;
    case 0xFC:
        break;
    case 0xFD:
        break;
    case 0xFE:
        break;
    case 0xFF
        RST_vec(gb,0x38);
        break;
    
    }
}