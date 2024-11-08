//
// Created by amucz0 on 11/7/24.
//
#include "../inc/Chip8.h"

void Chip8::Initialize() {
    memset(memory,0,sizeof(memory));
    memset(display,0,sizeof(display));
    memset(keypad,0,sizeof(keypad));
    memset(gpr,0,sizeof(gpr));
    memset(stack,0,sizeof(stack));

    sp = 0;
    i = 0;
    opcode = 0;
    delayTimer = 0;
    soundTimer = 0;
    pc = 0x200;

    for (int j = 0; j < sizeof(fonts) / sizeof(fonts[0]); j++){
        memory[j + 0x050] = fonts[j];
    }
}