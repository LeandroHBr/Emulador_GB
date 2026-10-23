#include <SDL3/SDL.h>
#include <stdio.h>
#include "config.h"
#include "registers.h"
#include "PPU.h"
#include "Gameboy.h"

int main(int argc, char **argv) {

    if(argc < 2){
        printf("voce precisa adicionar o diretorio da rom");
        return -1;
    }
    
    const char* room_name = argv[1]; 
    printf("%s\n",room_name);
    FILE* room = fopen(room_name,"rb");
    if(!room){
        printf("rom não encontrada");
        return -1;
    }
    fseek(room,0,SEEK_END);
    long size = ftell(room);
    fseek(room,0,SEEK_SET);
    
    char room_read[size];
    int res = fread(room_read,size,1,room);
    if(res!=1){
        printf("falha na leitura da room");
        return -1;
    }
    fclose(room);

    printf("Primeiros bytes: %02X %02X %02X %02X\n", 
    (unsigned char)room_read[0], 
    (unsigned char)room_read[1], 
    (unsigned char)room_read[2], 
    (unsigned char)room_read[3]);
    
    struct Gameboy gb;
    CPU_Init(&gb);
    CPU_load_Rom(&gb,room_read,size);
    
    gb.Register.PC = 0x0100;
    
    // Executa um número controlado de instruções usando o PC real do Game Boy
    for(int i = 0; i < 900; i++) {
        // 1. Obtém o opcode através do mapa de memória virtual do Game Boy no endereço do PC atual
        uint8_t opcode = System_Memory_Get(&gb.SYS_MEMORY, gb.Register.PC);
        
        printf("| PC: %04X | Opcode: %02X", gb.Register.PC, opcode);
        
        // 2. Executa a instrução (deixando a própria função cuidar de incrementar ou alterar o PC)
        CPU_exec(&gb, opcode);
    }


    for(int l=0x7fff;l<=0xFFFF;l++){
        printf("%x",System_Memory_Get(&gb.SYS_MEMORY,l));
    }

SDL_Init(SDL_INIT_VIDEO);
SDL_Window *window = SDL_CreateWindow(TITLE,
    WIDTH * MULTIPLIER,
    HEIGHT * MULTIPLIER,
    0);

SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
SDL_Event event;
struct PPU ppu;

PPU_Screen_Set(&ppu,0,0,0xff,0x40);

while (true)
{
    while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    goto exit;
                    break;
            }
        }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_Color Get_GB_Color(uint8_t color_id) {
    // Exemplo de paleta em tons de cinza/verde claro estilo Game Boy original
    switch (color_id) {
        case 0: return (SDL_Color){224, 248, 208, 255}; // Branco/Verde claro (Brilho máximo)
        case 1: return (SDL_Color){136, 192, 112, 255}; // Verde claro
        case 2: return (SDL_Color){ 52, 104,  86, 255}; // Verde escuro
        case 3: return (SDL_Color){  8,  24,  32, 255}; // Preto/Verde escuro (Quase preto)
        default: return (SDL_Color){0, 0, 0, 255};
    }
}


    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            int pixel = PPU_Get_Screen_Pixel(&ppu,j,i);
            SDL_Color rgb = Get_GB_Color(pixel);
            
        // 3. Configura a cor no SDL
            SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, rgb.a);
            SDL_FRect r;
            r.x = j * MULTIPLIER;
            r.y = i * MULTIPLIER;
            r.w = MULTIPLIER;
            r.h = MULTIPLIER;
            SDL_RenderFillRect(renderer,&r);
        }
    }
    SDL_RenderPresent(renderer);


}

exit:

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();


return 0;
}