#include <SDL2/SDL.h>
#include "chip8.h"

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int SCALE_FACTOR = 10; 
const int SCREEN_WIDTH = CHIP8_WIDTH * SCALE_FACTOR;
const int SCREEN_HEIGHT = CHIP8_HEIGHT * SCALE_FACTOR;

SDL_Window* createWindow(){
     return SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
}

SDL_Renderer* createRender(SDL_Window* window){
    SDL_Renderer* res = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(res, CHIP8_WIDTH, CHIP8_HEIGHT);
    return res;
}

void updateScreen(chip8 *C, SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int y = 0; y < 32; y++){
        for(int x = 0; x < 64; x++){
            if(C->display[y * 64 + x]){
                SDL_Rect rect;
                rect.x = x;
                rect.y = y;
                rect.h = 1;
                rect.w = 1;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int updateKeys(chip8 *C, SDL_Event *event){
    while(SDL_PollEvent(event)){
        if(event->type == SDL_QUIT){
            return 0;
        }
        char c = event->key.keysym.sym;
        int idx = keyToNum(c);
        if(idx >= 16){
            continue;
        }
        if(event->type == SDL_KEYDOWN){
            C->keysDown[idx] = 1;
        }
        else if(event->type == SDL_KEYUP){
            C->keysDown[idx] = 0;
        }
    }
    return 1;
}