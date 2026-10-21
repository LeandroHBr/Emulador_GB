#ifndef REGISTERS_H
#define REGISTERS_H
#include <stdint.h>


struct Registers{

    union {
        struct { uint8_t f, a; }; // Ordem little-endian comum (F e A formam AF)
        uint16_t af;
    };
    union {
        struct { uint8_t c, b; };
        uint16_t bc;
    };
    union {
        struct { uint8_t e, d; };
        uint16_t de;
    };
    union {
        struct { uint8_t l, h; };
        uint16_t hl;
    };

    uint16_t SP;
    uint16_t PC;
    uint8_t Z;
    uint8_t N;
    uint8_t H;
    uint8_t C;
    
};




#endif