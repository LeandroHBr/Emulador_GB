#ifndef PPU_H
#define PPU_H
#include <stdint.h>
#include "config.h"

struct PPU
{
    uint8_t Screen[HEIGHT][WIDTH];
};

void PPU_Screen_Set(struct PPU *screen,int x, int y, uint8_t byte_low, uint8_t byte_high);
uint8_t PPU_Get_Screen_Pixel(struct PPU *screen,int x, int y);
uint8_t Pixel_color_set(uint8_t pixel);

#endif