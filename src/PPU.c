#include "config.h"
#include "PPU.h"
#include <assert.h>

void Assert_possicion(int x, int y){
assert(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
}

void PPU_Screen_Set(struct PPU *screen,int x, int y, uint8_t byte_low, uint8_t byte_high){
    Assert_possicion(x,y);
    uint16_t line_16_bytes = byte_high << 8 | byte_low; 
    uint16_t mask = 0b1100000000000000;
    uint8_t pixel;

    for(int i=0;i<8;i++){
        int bit_pos = 7 - i;
        int msb = (byte_high >> bit_pos) & 1;
        int lsb = (byte_low >> bit_pos) & 1;
        int pixel_color = (msb << 1) | lsb;
        
        int target_x = x + i;
        int target_y = y;

        if (target_x < WIDTH && target_y < HEIGHT) {
            screen->Screen[target_y][target_x] = pixel_color;
        }
    }
}

uint8_t PPU_Get_Screen_Pixel(struct PPU *screen,int x, int y){
    Assert_possicion(x,y);
    return screen->Screen[y][x];
}