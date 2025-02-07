#include "CPU.hpp"
#include <algorithm>
#include <complex>

const uint16_t START_ADDRESS = 0x200;

CPU::CPU()
{
    // first 512 bytes (registers 0x000 to 0x200) are reserved for system use
    PC = START_ADDRESS;
    // load font data into memory
    for (int i = 0; i < FONT_SIZE; i++)
    {
        memory[START_ADDRESS + i] = font_data[i];
    }
}
void CPU::reset()
{
    // clear memory
    std::fill(std::begin(memory), std::end(memory), 0x00);
    std::fill(std::begin(V), std::end(V), 0x00);
    std::fill(std::begin(stack), std::end(stack), 0x0000);
    I = 0x0000;
    PC = START_ADDRESS;
    SP = 0x00;
    sound_timer = 0x00;
    delay_timer = 0x00;
    // load font data into memory
    for (int i = 0; i < FONT_SIZE; i++)
    {
        memory[START_ADDRESS + i] = font_data[i];
    }
}

void CPU::OP_00E0()
{
    //memset(video, 0, sizeof(video));

}

void CPU::OP_00EE()
{
}

/* Jump to location nnn: The interpreter sets the program counter to nnn. */
void CPU::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFF;
    PC = address;
}

void CPU::OP_2nnn()
{
}

void CPU::OP_3xkk()
{
}

void CPU::OP_4xkk()
{
}

void CPU::OP_5xy0()
{
}

void CPU::OP_6xkk()
{
}

void CPU::OP_7xkk()
{
}

void CPU::OP_8xy0()
{
}

void CPU::OP_8xy1()
{
}

void CPU::OP_8xy2()
{
}

void CPU::OP_8xy3()
{
}

void CPU::OP_8xy4()
{
}

void CPU::OP_8xy5()
{
}

void CPU::OP_8xy6()
{
}

void CPU::OP_8xy7()
{
}

void CPU::OP_8xyE()
{
}

void CPU::OP_9xy0()
{
}

void CPU::OP_Annn()
{
}

void CPU::OP_Bnnn()
{
}

void CPU::OP_Cxkk()
{
}

void CPU::OP_Dxyn()
{
}

void CPU::OP_Ex9E()
{
}

void CPU::OP_ExA1()
{
}

void CPU::OP_Fx07()
{
}

void CPU::OP_Fx0A()
{
}

void CPU::OP_Fx15()
{
}

void CPU::OP_Fx18()
{
}

void CPU::OP_Fx1E()
{
}

void CPU::OP_Fx29()
{
}

void CPU::OP_Fx33()
{
}

void CPU::OP_Fx55()
{
}

void CPU::OP_Fx65()
{
}
