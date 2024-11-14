//
// Created by amucz0 on 11/7/24.
//
#include "../inc/Chip8.h"

void Chip8::init() {
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

[[noreturn]] void Chip8::executionCycle() {
    while(true){

        // *FETCH* : Retrieves the next 16bit instruction in the program counter
        uint16_t inst = (memory[pc] << 8 | memory[pc+1]);
        pc+=2;
        // *DECODE*
        opcode = inst & 0xF000;
        uint8_t X = inst & 0x0F00;
        uint8_t Y = (inst & 0x00F0) >> 8;
        uint8_t N = (inst & 0x000F) >> 4;
        uint8_t NN = inst & 0x00FF;
        // *EXECUTE*
        switch (opcode) {
            case 0x0000:    //system calls

                break;
            case 0x1000:    //jump

                break;
            case 0x2000:    //subroutine

                break;
            case 0x3000:    //Conditional Skip

                break;
            case 0x4000:    //Conditional Skip

                break;
            case 0x5000:

                break;
            case 0x6000:    //Register Operations

                break;
            case 0x7000:    //Register Operations

                break;
            case 0x8000:    //Arithmetic And BitWise Operations

                break;
            case 0xA000:    //Set Index Register

                break;
            case 0xB000:    //Jump with Offset

                break;
            case 0xC000:    //Random Number (mask)

                break;
            case 0xD000:    //Draw Sprite

                break;
            case 0xE000:    //Key Operations

                break;
            case 0xF000:    //Timer Sound Memory Operations

                break;

            default:
                memDump();
                exit(0);
        }

    }
}

void Chip8::memDump() {
    const int width = 16;
    cout << "Memory Dump:" << endl;

    for (int j = 0; j < 4096; j += width) {
        cout << "0x" << setw(4) << setfill('0') << hex << j << " | ";
        for (int k = 0; k < width; ++k) {
            if (memory[j + k] == 0x00) {
                cout << BLUE;
            } else if (j + k >= 0x0200 && memory[j+k] != 0x00) {
                cout << GREEN;
            }else {
                cout << YELLOW;
            }
            cout << setw(2) << setfill('0') << (int)memory[j + k] << " ";
        }
        cout << RESET << "| " << setw(4)<< setfill('0') << dec << j << "-" << setw(4)<< setfill('0') << j+15;
        cout << endl;
    }
}

