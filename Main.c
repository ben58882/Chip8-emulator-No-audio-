#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>

void debug_draw(struct chip8 *C);

int main(int argc, char **argv){
    struct chip8 cpu;
    unsigned short instructionReg = 0;
    intialise(&cpu);
    loadRom(&cpu, argv[1]); 
    SDL_Window* window = createWindow();
    SDL_Renderer* renderer = createRender(window);
    SDL_Event event;

    int running = 1;
    uint32_t lastTime = SDL_GetTicks();
    while (running) {
        
        running = updateKeys(&cpu, &event);
        fetch(&cpu, &instructionReg);

        decode(&cpu, &instructionReg);

        if(cpu.drawFlag) {
            updateScreen(&cpu, renderer);
            cpu.drawFlag = 0; 
        }
        uint32_t currTime = SDL_GetTicks();
        if(currTime - lastTime >= 16){
            if(cpu.delayTimer > 0){
                cpu.delayTimer -= 1;
            }
            if(cpu.soundTimer > 0){
                cpu.soundTimer -= 1;
            }
            lastTime = currTime;
        }

        SDL_Delay(2);
        //usleep(2000); 
    }

    return 0;
}



void debug_draw(struct chip8 *C) {
    // This command clears the terminal screen so the next frame 
    // prints in the same spot instead of scrolling down.
    printf("\033[H\033[J"); 

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            // Calculate the 1D index from 2D coordinates
            if (C->display[x + (y * 64)] == 1) {
                printf("#"); // Pixel is ON
            } else {
                printf(" "); // Pixel is OFF
            }
        }
        printf("\n"); // Move to the next row
    }
}