//
// Created by amucz0 on 11/7/24.
//
#include "../inc/Chip8.h"

void Chip8::init(const char* path) {

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip8 Emulator", 640, 320, 0);
    if (window == nullptr) SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
    //SDL_Delay(15000);

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

    srand(time(nullptr)); // NOLINT(*-msc51-cpp)

    loadRom(path);

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
        uint8_t x = (inst & 0x0F00) >> 8;
        uint8_t y = (inst & 0x00F0) >> 4;
        uint8_t n = inst & 0x000F;
        uint8_t nn = inst & 0x00FF;
        uint8_t nnn = inst & 0x0FFF;
        // *EXECUTE*
        switch (opcode) {
            case 0x0000:    //system calls
                switch (nn) {
                    case 0x00E0:    // CLS : Clear the Display
                        memset(display,0,sizeof(display));
                        break;
                    case 0x00EE:    // RET : Return from a Subroutine
                        pc = stack[sp--];
                        break;
                    default:
                        memDump(1);
                }
                break;
            case 0x1000:    // JP : Jump to Location nnn
                pc = nnn;
                break;
            case 0x2000:    //CALL : Call Subroutine at nnn
                stack[sp++] = pc;
                pc = nnn;
                break;
            case 0x3000:    //Conditional Skip
                if (gpr[x] == gpr[nn]){
                    pc+=2;
                }
                break;
            case 0x4000:    //Conditional Skip
                if (gpr[x] != gpr[nn]){
                    pc+=2;
                }
                break;
            case 0x5000:
                if (gpr[x] == gpr[y]){
                    pc+=2;
                }
                break;
            case 0x6000:    //Register Operations
                gpr[x] = nn;
                break;
            case 0x7000:    //Add Operation
                gpr[x] += nn;
                break;
            case 0x8000:    //Arithmetic And BitWise Operations
                switch (n) {
                    case 0x0000:    //8xy0
                        gpr[x] = gpr[y];
                        break;
                    case 0x0001:    //8xy1
                        gpr[x] = gpr[x] | gpr[y];
                        break;
                    case 0x0002:    //8xy2
                        gpr[x] = gpr[x] & gpr[y];
                        break;
                    case 0x0003:    //8xy3
                        gpr[x] = gpr[x] ^ gpr[y];
                        break;
                    case 0x0004:    //8xy4
                        gpr[x] = gpr[x] + gpr[y];
                        break;
                    case 0x0005:    //8xy5
                        gpr[x] = gpr[x] - gpr[y];
                        break;
                    case 0x0006:    //8xy6
                        gpr[x] = gpr[x] + gpr[y];
                        break;
                    case 0x0007:    //8xy7
                        gpr[x] = gpr[y] - gpr[x];
                        break;
                    case 0x000E:    //8xy8
                        gpr[x] = gpr[x] + gpr[y];
                        break;
                    default:
                        memDump(1);
                }
                break;
            case 0xA000:    //Set Index Register
                i = nnn;
                break;
            case 0xB000:    //Jump with Offset
                pc = nnn + gpr[0];
                break;
            case 0xC000:    //Random Number (mask)
                gpr[x] = uint8_t(rand()) & nn;
                break;
            case 0xD000:    //Draw Sprite

                break;
            case 0xE000:    //Key Operations
                break;
            case 0xF000:    //Timer Sound Memory Operations
                switch (nn) {
                    case 0x0007:
                        gpr[x] = delayTimer;
                        break;
                    case 0x000A:    //implement busy waiting for a keypress and store the value in gpr[x] ; shit implementation, needs fixing

                        break;
                    case 0x0015:
                        delayTimer = gpr[x];
                        break;
                    case 0x0018:
                        soundTimer = gpr[x];
                        break;
                    case 0x001E:
                        i += gpr[x];
                        break;
                    case 0x0055:
                        for(int index = 0; index <= x; index++){
                            memory[i + index] = gpr[index];
                        }
                        break;
                    case 0x0065:
                        for(int index = 0; index <= x; index++){
                            gpr[index] = memory[i + index];
                        }
                        break;
                    default:
                        memDump(1);
                }
                break;
            default:
                memDump(1);

        }
    }
}

void Chip8::memDump(int exitFlag) {

    cout << "Memory Dump:" << endl;

    for (int j = 0; j < 4096; j += 16) {
        cout << "0x" << setw(4) << setfill('0') << hex << j << " | ";
        for (int k = 0; k < 16; ++k) {
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
    if (exitFlag > 0){
        exit(exitFlag);
    }
}

void Chip8::loadRom(const string& path) {
    ifstream inFile;

    inFile.open(path, ios::binary | ios::in);
    if (!inFile) {
        std::cerr << "Failed to open config file: " << path << std::endl;
        return;
    }

    inFile.seekg(0, ios::end);
    streamsize fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    inFile.read(reinterpret_cast<char *>(&memory[0x200]), fileSize);

}
