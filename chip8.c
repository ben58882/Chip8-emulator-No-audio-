#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

int loadRom(struct chip8 *C, const char *name){
    FILE* file = fopen(name, "rb");
    if(file == NULL){
        printf("Empty file");
        fclose(file);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if(size >= 4096 - 512){
        printf("File too big");
        fclose(file);
        return -1;
    }
    fseek(file, 0, SEEK_SET);
    fread(&(C -> memory[512]), 1, size, file);
    return fclose(file);
}

int fetch(struct chip8 *C, unsigned short *instructionRegister){
    unsigned short currPc = C->programCounter;
    uint8_t *mem = C->memory;
    *instructionRegister = (*(mem + currPc) << 8) | (*(mem + currPc + 1));
    C->programCounter += 2;
    return 0;
}

int decode(struct chip8 *C, unsigned short *instructionRegister){
    unsigned short func = (*instructionRegister & 0xF000) >> 12;
    C->table[func](C, *instructionRegister);
    return 0;
}

void intialise(struct chip8 *C){
    memset(C->memory, 0, 4096);
    memset(C->V_REGISTERS, 0, 16);
    memset(C->stack, 0, 16);
    memset(C->display, 0, 2048);
    memset(C->table, 0, 16);
    memset(C->keysDown, 0, 16);
    C->programCounter = 0x200;
    C->stackPointer = 0;
    C->delayTimer = 0;
    C->soundTimer = 0;
    C->I_REGISTER = 0;
    C->drawFlag = 0;
    C->released = 0;
    C->pressed = 0;
    C->table[0] = prefix0;
    C->table[1] = prefix1;
    C->table[2] = prefix2;
    C->table[3] = prefix3;
    C->table[4] = prefix4;
    C->table[5] = prefix5;
    C->table[6] = prefix6;
    C->table[7] = prefix7;
    C->table[8] = prefix8;
    C->table[9] = prefix9;
    C->table[0xA] = prefixA;
    C->table[0xB] = prefixB;
    C->table[0xC] = prefixC;
    C->table[0xD] = prefixD;
    C->table[0xE] = prefixE;
    C->table[0xF] = prefixF;
    unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; ++i) {
    C->memory[0x50 + i] = fontset[i];
    }
}


