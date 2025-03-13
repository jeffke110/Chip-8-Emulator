#ifndef CHIP8_HPP
#define CHIP8_HPP

#pragma once

#include <cstdint>
#include <chrono>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <complex>
#include <random>
#include <string>
#include <iostream>


const unsigned int MEMORY_SIZE{4096};
const unsigned int REGISTER_COUNT{16};
const unsigned int STACK_LEVELS{16};
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const uint16_t START_ADDRESS{0x200};
const uint8_t FONT_SIZE{80};

class Chip8
{
public:
    Chip8();
    uint8_t getRandomByte();
    void LoadROM(char const *filename);
    void Cycle();

    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};

    //GETTERS
    uint8_t * getRegisters();
    uint16_t * getStack();
    uint16_t getIndex();
    uint16_t getPC();
    uint8_t getSP();
    uint16_t getOpcode();
    uint8_t getSoundTimer();
    uint8_t getDelayTimer();
    uint8_t *getMemory();
    std::string getInstruction();

private:
    uint8_t memory[MEMORY_SIZE]{};
    uint8_t registers[REGISTER_COUNT]{};
    uint16_t stack[STACK_LEVELS]{};
    uint16_t index{};
    uint16_t PC{};
    uint8_t SP{};
    uint16_t opcode{};
    uint8_t sound_timer{};
    uint8_t delay_timer{};
    std::string instruction = "";
    const uint8_t font_data[FONT_SIZE] = {
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

    // instructions
    void OP_00E0(); // 1  CLS: Clear the display.
    void OP_00EE(); // 2  RET: Return from a subroutine.
    void OP_1nnn(); // 3  JP addr: Jump to location nnn.
    void OP_2nnn(); // 4  CALL addr: Call subroutine at nnn. (Research)
    void OP_3xkk(); // 5  SE Vx, byte: Skip next instruction if Vx = kk. (Research)
    void OP_4xkk(); // 6  SNE Vx, byte: Skip next instruction if Vx != kk. (Research)
    void OP_5xy0(); // 7  SE Vx, Vy: Skip next instruction if Vx = Vy. (Research)
    void OP_6xkk(); // 8  LD Vx, byte: Set Vx = kk.
    void OP_7xkk(); // 9  ADD Vx, byte: Set Vx = Vx + kk.
    void OP_8xy0(); // 10 LD Vx, Vy: Set Vx = Vy.
    void OP_8xy1(); // 11 OR Vx, Vy: Set Vx = Vx OR Vy.
    void OP_8xy2(); // 12 AND Vx, Vy: Set Vx = Vx AND Vy.
    void OP_8xy3(); // 13 XOR Vx, Vy: Set Vx = Vx XOR Vy.
    void OP_8xy4(); // 14 ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
    void OP_8xy5(); // 15 SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
    void OP_8xy6(); // 16 SHR Vx {, Vy}: Set Vx = Vx SHR 1.
    void OP_8xy7(); // 17 SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
    void OP_8xyE(); // 18 SHL Vx {, Vy}: Set Vx = Vx SHL 1.
    void OP_9xy0(); // 19 SNE Vx, Vy: Skip next instruction if Vx != Vy.
    void OP_Annn(); // 20 LD I, addr: Set I = nnn.
    void OP_Bnnn(); // 21 JP V0, addr: Jump to location nnn + V0.
    void OP_Cxkk(); // 22 RND Vx, byte: Set Vx = random byte AND kk.
    void OP_Dxyn(); // 23 DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    void OP_Ex9E(); // 24 SKP Vx: Skip next instruction if key with the value of Vx is pressed.
    void OP_ExA1(); // 25 SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
    void OP_Fx07(); // 26 LD Vx, DT: Set Vx = delay timer value.
    void OP_Fx0A(); // 27 LD Vx, K: Wait for a key press, store the value of the key in Vx.
    void OP_Fx15(); // 28 LD DT, Vx: Set delay timer = Vx.
    void OP_Fx18(); // 29 LD ST, Vx: Set sound timer = Vx.
    void OP_Fx1E(); // 30 ADD I, Vx: Set I = I + Vx.
    void OP_Fx29(); // 31 LD F, Vx: Set I = location of sprite for digit Vx.
    void OP_Fx33(); // 32 LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
    void OP_Fx55(); // 33 LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
    void OP_Fx65(); // 34 LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
    void OP_NULL();

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    void setup_table();
};

#endif // CHIP8_HPP