#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct chip8 chip8;
typedef void(*instructions)(struct chip8 *C, unsigned short opcode);

struct chip8{
    //ram
    uint8_t memory[4096];

    unsigned short programCounter;
    uint8_t stackPointer;

    //I register
    unsigned short I_REGISTER;

    //special registers
    uint8_t delayTimer;
    uint8_t soundTimer;

    //stack
    unsigned short stack[16];

    //normal registers
    uint8_t V_REGISTERS[16];

    //display
    uint8_t display[64 * 32];

    //opcodes
    instructions table[16];

    //drawflag
    uint8_t drawFlag;

    //keys down
    uint8_t keysDown[16];

    //for 0xFX0A
    int pressed;
    int released;
};

int loadRom(struct chip8* C, const char *name);
int fetch(struct chip8 *C, unsigned short *instructionRegister);
int decode(struct chip8 *C, unsigned short *instructionRegister);
void intialise(struct chip8 *C);
unsigned int keyToNum(char character);
void prefix0 (struct chip8 *C, unsigned short opcode);
void prefix1 (struct chip8 *C, unsigned short opcode);
void prefix2 (struct chip8 *C, unsigned short opcode);
void prefix3 (struct chip8 *C, unsigned short opcode);
void prefix4 (struct chip8 *C, unsigned short opcode);
void prefix5 (struct chip8 *C, unsigned short opcode);
void prefix6 (struct chip8 *C, unsigned short opcode);
void prefix7 (struct chip8 *C, unsigned short opcode);
void prefix8 (struct chip8 *C, unsigned short opcode);
void prefix9 (struct chip8 *C, unsigned short opcode);
void prefixA (struct chip8 *C, unsigned short opcode);
void prefixB (struct chip8 *C, unsigned short opcode);
void prefixC (struct chip8 *C, unsigned short opcode);
void prefixD (struct chip8 *C, unsigned short opcode);
void prefixE (struct chip8 *C, unsigned short opcode);
void prefixF (struct chip8 *C, unsigned short opcode);

SDL_Window* createWindow();
SDL_Renderer* createRender(SDL_Window* window);
void updateScreen(chip8 *C, SDL_Renderer* renderer);
int updateKeys(chip8 *C, SDL_Event *event);