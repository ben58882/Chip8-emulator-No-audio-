#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"
#include <time.h>
#include <stdint.h>


void prefix0 (struct chip8 *C, unsigned short opcode){
    unsigned short instruc = opcode & 0x000F;
    switch(instruc){
        case 0:
            for(int i = 0; i < 2048; i++){
                C->display[i] = 0;
            }
            break;
        case 0xE:
            C->stackPointer -= 1;
            C->programCounter = C->stack[C->stackPointer];
            break;
    }
}

void prefix1 (struct chip8 *C, unsigned short opcode){
    unsigned short address = opcode & 0x0FFF;
    C->programCounter = address;
}

void prefix2 (struct chip8 *C, unsigned short opcode){
    unsigned short address = opcode & 0x0FFF;
    C->stack[C->stackPointer] = C->programCounter;
    C->stackPointer += 1;
    C->programCounter = address;
}

void prefix3 (struct chip8 *C, unsigned short opcode){
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short immediate = (opcode & 0x00FF);
    if(C->V_REGISTERS[regX] == immediate){
        C->programCounter += 2;
    }
}

void prefix4 (struct chip8 *C, unsigned short opcode){
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short immediate = (opcode & 0x00FF);
    if(C->V_REGISTERS[regX] != immediate){
        C->programCounter += 2;
    }
}

void prefix5 (struct chip8 *C, unsigned short opcode){
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short regY = (opcode & 0x00F0) >> 4;
    if(C->V_REGISTERS[regX] == C->V_REGISTERS[regY]){
        C->programCounter += 2;
    }
}

void prefix6 (struct chip8 *C, unsigned short opcode){
    unsigned short reg = (opcode & 0x0F00) >> 8;
    unsigned short immediate = opcode & 0x00FF;
    C->V_REGISTERS[reg] = immediate;
}

void prefix7 (struct chip8 *C, unsigned short opcode){
    unsigned short reg = (opcode & 0x0F00) >> 8;
    unsigned short immediate = opcode & 0x00FF;
    C->V_REGISTERS[reg] += immediate;
}

void prefix8 (struct chip8 *C, unsigned short opcode){
    unsigned tmp;
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short regY = (opcode & 0x00F0) >> 4;
    unsigned short instruc = opcode & 0x000F;
    uint8_t flag;
    switch(instruc){
        case 0:
            C->V_REGISTERS[regX] = C->V_REGISTERS[regY];
            break;
        case 1:
            C->V_REGISTERS[regX] |= C->V_REGISTERS[regY];
            break;
        case 2:
            C->V_REGISTERS[regX] &= C->V_REGISTERS[regY];
            break;
        case 3:
            C->V_REGISTERS[regX] ^= C->V_REGISTERS[regY];
            break;
        case 4:
            tmp = C->V_REGISTERS[regX] + C->V_REGISTERS[regY];
            C->V_REGISTERS[regX] = C->V_REGISTERS[regX] + C->V_REGISTERS[regY];
            if(tmp > 255){
                C->V_REGISTERS[0xF] = 1;
            }
            else{
                C->V_REGISTERS[0xF] = 0;
            }
            break;
        case 5:
            flag = C->V_REGISTERS[regX] >= C->V_REGISTERS[regY] ? 1 : 0;
            C->V_REGISTERS[regX] = C->V_REGISTERS[regX] - C->V_REGISTERS[regY];
            C->V_REGISTERS[0xF] = flag;
            break;
        case 6:
            tmp = C->V_REGISTERS[regX];
            C->V_REGISTERS[regX] >>= 1;
            C->V_REGISTERS[0xF] = tmp & 1;
            break;
        case 7:
            flag = C->V_REGISTERS[regY] >= C->V_REGISTERS[regX] ? 1 : 0;
            C->V_REGISTERS[regX] = C->V_REGISTERS[regY] - C->V_REGISTERS[regX];
            C->V_REGISTERS[0xF] = flag;
            break;
        case 0xE:
            tmp = C->V_REGISTERS[regX];
            C->V_REGISTERS[regX] <<= 1;
            C->V_REGISTERS[0xF] = (128 & tmp) >> 7;
            break;
    }

}

void prefix9 (struct chip8 *C, unsigned short opcode){
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short regY = (opcode & 0x00F0) >> 4;
    if(C->V_REGISTERS[regX] != C->V_REGISTERS[regY]){
        C->programCounter += 2;
    }
}

void prefixA (struct chip8 *C, unsigned short opcode){
    unsigned short address = opcode & 0x0FFF;
    C->I_REGISTER = address;
}

void prefixB (struct chip8 *C, unsigned short opcode){
    unsigned short address = (opcode & 0x0FFF) + C->V_REGISTERS[0];
    C->programCounter = address;
}

void prefixC (struct chip8 *C, unsigned short opcode){
    unsigned short immediate = opcode & 0x00FF;
    unsigned regX = (opcode & 0x0F00) >> 8;
    srand(time(NULL));
    int random = rand() % 256;
    C->V_REGISTERS[regX] = random & immediate;
}

void prefixD (struct chip8 *C, unsigned short opcode){
    unsigned short regX = (opcode & 0x0F00) >> 8;
    unsigned short regY = (opcode & 0x00F0) >> 4;
    unsigned short n = opcode & 0x000F;
    C->V_REGISTERS[0xF] = 0;
    int startX = C->V_REGISTERS[regX] % 64;
    int startY = C->V_REGISTERS[regY] % 32;
    long displayLocation = 64 * startY + startX;
    uint8_t *memoryLocation = &(C->memory[C->I_REGISTER]);
    for(int i = 0; i < n; i++){
        uint8_t tmp = *memoryLocation;
        int currLocation = displayLocation;
        if(currLocation >= 2048) break;
        for(int j = 7; j >= 0; j--){
            uint8_t currBit = (tmp >> j) & 1;
            if(currLocation % 64 == 0 && j != 7) break;
            C->V_REGISTERS[0xF] |= currBit & C->display[currLocation];
            C->display[currLocation] = currBit ^ C->display[currLocation];
            currLocation += 1;
        }
        memoryLocation += 1;
        displayLocation += 64;
    }
    C->drawFlag = 1;
}

void prefixE (struct chip8 *C, unsigned short opcode){
    unsigned short instruc = opcode & 0x00FF;
    unsigned short xReg = (opcode & 0x0F00) >> 8;
    unsigned short key = C->V_REGISTERS[xReg];
    if(key >= 16) return;
    switch(instruc){
        case 0x9E:
            if(C->keysDown[key]){
                C->programCounter += 2;
            }
            break;
        case 0xA1:
            if(!(C->keysDown[key])){
                C->programCounter += 2;
            }
            break;
    }
}

void prefixF (struct chip8 *C, unsigned short opcode){
    unsigned short instruct = opcode & 0x00FF;
    unsigned short xReg = (opcode & 0x0F00) >> 8;
    char tmp = 'p';
    unsigned int tmp2 = 16;
    unsigned int num;
    unsigned int ones;
    unsigned int tens;
    unsigned int hundreds;
    switch(instruct){
        case 0x07:
            C->V_REGISTERS[xReg] = C->delayTimer;
            break;
        case 0x0A:
            if(!C->pressed){
                for(int i = 0; i < 16; i++){
                    if(C->keysDown[i]){
                        C->V_REGISTERS[xReg] = i;
                        C->pressed = 1;
                        break;
                    }
                }   
            }
            if(C->pressed && !C->keysDown[C->V_REGISTERS[xReg]]){
               C->released = 1;
            }
            if(!C->pressed || !C->released){
                C->programCounter -= 2;
            }
            else{
                C->pressed = 0;
                C->released = 0;
            }
            break;
        case 0x15:
            C->delayTimer = C->V_REGISTERS[xReg];
            break;
        case 0x18:
            C->soundTimer = C->V_REGISTERS[xReg];
            break;
        case 0x1E:
            C->I_REGISTER += C->V_REGISTERS[xReg];
            break;
        case 0x29:
            C->I_REGISTER = 0x50 + C->V_REGISTERS[xReg] * 5;
            break;
        case 0x33:
            num = C->V_REGISTERS[xReg];
            ones = num % 10;
            num /= 10;
            tens = num % 10;
            num /= 10;
            hundreds = num % 10;
            C->memory[C->I_REGISTER] = hundreds;
            C->memory[C->I_REGISTER + 1] = tens;
            C->memory[C->I_REGISTER + 2] = ones;
            break;
        case 0x55:
            for(int i = 0; i <= xReg; i++){
                C->memory[C->I_REGISTER + i] = C->V_REGISTERS[i];
            }
            break;
        case 0x65:
            for(int i = 0; i <= xReg; i++){
                C->V_REGISTERS[i] = C->memory[C->I_REGISTER + i];
            }
            break;
    }
}

unsigned int keyToNum(char character){
    switch(character){
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 0xC;
        case 'q':
        case'Q':
            return 4;
        case 'w':
        case 'W':
            return 5;
        case 'e':
        case 'E':
            return 6;
        case 'r':
        case'R':
            return 0xD;
        case 'a':
        case'A':
            return 7;
        case 's':
        case 'S':
            return 8;
        case 'd':
        case 'D':
            return 9;
        case 'f':
        case 'F':
            return 0xE;
        case 'z':
        case 'Z':
            return 0xA;
        case 'x':
        case 'X':
            return 0;
        case 'c':
        case 'C':
            return 0xB;
        case 'v':
        case 'V':
            return 0xF;
    }
    return 16;
}

