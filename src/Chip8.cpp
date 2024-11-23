//
// Created by amucz0 on 11/7/24.
//
#include "../inc/Chip8.h"

void Chip8::init(const char* path) {

    InitWindow(640, 320, "Chip8Interpreter");
    SetTargetFPS(60);
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

void Chip8::executionCycle() {
    while (!WindowShouldClose()) {

        // *FETCH* : Retrieves the next 16bit instruction in the program counter
        uint16_t inst = (memory[pc] << 8 | memory[pc + 1]);
        pc += 2;
        // *DECODE*
        opcode = inst & 0xF000;
        uint8_t X = (inst & 0x0F00) >> 8;
        uint8_t Y = (inst & 0x00F0) >> 4;
        uint8_t N = inst & 0x000F;
        uint8_t NN = inst & 0x00FF;
        uint8_t NNN = inst & 0x0FFF;
        // *EXECUTE*
        switch (opcode) {
            case 0x0000:    //system calls
                switch (NN) {
                    case 0x00E0:    // CLS : Clear the Display
                        memset(display, 0, sizeof(display));
                        break;
                    case 0x00EE:    // RET : Return from a Subroutine
                        pc = stack[sp--];
                        break;
                    default:
                        memDump(1);
                }
                break;
            case 0x1000:    // JP : Jump to Location NNN
                pc = NNN;
                break;
            case 0x2000:    //CALL : Call Subroutine at NNN
                stack[sp++] = pc;
                pc = NNN;
                break;
            case 0x3000:    //Conditional Skip
                if (gpr[X] == gpr[NN]) {
                    pc += 2;
                }
                break;
            case 0x4000:    //Conditional Skip
                if (gpr[X] != gpr[NN]) {
                    pc += 2;
                }
                break;
            case 0x5000:
                if (gpr[X] == gpr[Y]) {
                    pc += 2;
                }
                break;
            case 0x6000:    //Register Operations
                gpr[X] = NN;
                break;
            case 0x7000:    //Add Operation
                gpr[X] += NN;
                break;
            case 0x8000:    //Arithmetic And BitWise Operations
                switch (N) {
                    case 0x0000:    //8xy0
                        gpr[X] = gpr[Y];
                        break;
                    case 0x0001:    //8xy1
                        gpr[X] = gpr[X] | gpr[Y];
                        break;
                    case 0x0002:    //8xy2
                        gpr[X] = gpr[X] & gpr[Y];
                        break;
                    case 0x0003:    //8xy3
                        gpr[X] = gpr[X] ^ gpr[Y];
                        break;
                    case 0x0004:    //8xy4
                        gpr[X] = gpr[X] + gpr[Y];
                        break;
                    case 0x0005:    //8xy5
                        gpr[X] = gpr[X] - gpr[Y];
                        break;
                    case 0x0006:    //8xy6
                        gpr[X] = gpr[X] + gpr[Y];
                        break;
                    case 0x0007:    //8xy7
                        gpr[X] = gpr[Y] - gpr[X];
                        break;
                    case 0x000E:    //8xy8
                        gpr[X] = gpr[X] + gpr[Y];
                        break;
                    default:
                        memDump(1);
                }
                break;
            case 0xA000:    //Set Index Register
                i = NNN;
                break;
            case 0xB000:    //Jump with Offset
                pc = NNN + gpr[0];
                break;
            case 0xC000:    //Random Number (mask)
                gpr[X] = uint8_t(rand()) & NN;
                break;
            case 0xD000: {  //Draw Sprite
                uint8_t x_cord = gpr[X] & 63;
                uint8_t y_cord = gpr[Y] & 31;
                gpr[15] = 0;
                for (int row = 0; row < N; row++) {
                    uint8_t sprite_byte = memory[i + row];
                    for (int col = 0; col < 8; col++) {
                        uint8_t sprite_bit = sprite_byte & (0x80 >> col);
                        if (sprite_bit) {
                            int pixel_index = ((y_cord + row) % 32) * 64 + ((x_cord + col) % 64);
                            if (display[pixel_index]) {
                                gpr[15] = 1;
                            }
                            display[pixel_index] ^= 1;
                        }
                    }
                }
                break;
            }
            case 0xE000:    //Key Operations
                break;
            case 0xF000:    //Timer Sound Memory Operations
                switch (NN) {
                    case 0x0007:
                        gpr[X] = delayTimer;
                        break;
                    case 0x000A:    //implement busy waiting for a keypress and store the value in gpr[X] ;

                        break;
                    case 0x0015:
                        delayTimer = gpr[X];
                        break;
                    case 0x0018:
                        soundTimer = gpr[X];
                        break;
                    case 0x001E:
                        i += gpr[X];
                        break;
                    case 0x0055:
                        for (int index = 0; index <= X; index++) {
                            memory[i + index] = gpr[index];
                        }
                        break;
                    case 0x0065:
                        for (int index = 0; index <= X; index++) {
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

        BeginDrawing();
        int scale = 10;
        ClearBackground(WHITE);

        for (int x = 0; x < 64; x++) {
            for (int y = 0; y < 32; y++) {
                if (display[y * 64 + x]) {
                    DrawRectangle(x * scale, y * scale, scale, scale, BLACK);
                }
            }
        }
        EndDrawing();
    }
}

void Chip8::memDump(int exitFlag) {

    cout << "Memory Dump:" << endl;
    cout << "Opcode: " << setw(4) << setfill('0') << hex << opcode << endl;
    cout << "Program Counter:" << setw(4) << setfill('0') << hex << pc << endl;

    for (int j = 0; j < 4096; j += 16) {
        cout << "0x" << setw(4) << setfill('0') << hex << j << " | ";
        for (int k = 0; k < 16; ++k) {
            if (memory[j + k] == 0x00) {
                cout << MY_BLUE;
            } else if (j + k >= 0x0200) {
                cout << MY_GREEN;
            }else {
                cout << MY_YELLOW;
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

    if (fileSize > 4096 - 0x200){
        cerr << "ROM File is too Large for Address Space" << endl;
    }

    inFile.read(reinterpret_cast<char *>(&memory[0x200]), fileSize);

    if (!inFile){
        cerr << "Error Occurred while reading File" << endl;
    } else {
        cout << "Successfully Loaded ROM File" << endl;
    }

}
