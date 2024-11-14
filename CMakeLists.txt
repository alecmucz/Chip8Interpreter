cmake_minimum_required(VERSION 3.28)
project(Chip8Interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(Chip8Interpreter
        src/Chip8.cpp
        inc/Chip8.h
        main.cpp)
