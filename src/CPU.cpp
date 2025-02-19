#include "CPU.hpp"
#include <algorithm>
#include <complex>
#include <random>

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

uint8_t CPU::getRandomByte()
{
    static std::random_device rd;  // Seed generator
    static std::mt19937 gen(rd()); // Mersenne Twister PRNG
    static std::uniform_int_distribution<uint16_t> dist(0, 255);
    return static_cast<uint8_t>(dist(gen));
}

void CPU::OP_00E0()
{
    //memset(video, 0, sizeof(video));

}

void CPU::OP_00EE()
{
}

void CPU::OP_1nnn() //JP addr: Jump to location nnn.
{
    uint16_t address = opcode & 0x0FFFu;
    PC = address;
}

void CPU::OP_2nnn() // CALL addr: Call subroutine at nnn.
{
    uint16_t address = opcode & 0x0FFFu;
    stack[SP] = PC;
    ++PC;
    PC = address;
}

void CPU::OP_3xkk() // SE Vx, byte: Skip next instruction if Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Extract Register Vx
    uint8_t byte = (opcode & 0x00FFu);      // Extract kk
    if(V[Vx] == byte){
        PC+=2;
    }
}

void CPU::OP_4xkk() // SNE Vx, byte: Skip next instruction if Vx != kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    if(V[Vx] != byte){
        PC+=2;
    }
}

void CPU::OP_5xy0() //SE Vx, Vy: Skip next instruction if Vx = Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(V[Vx] == V[Vy]){
        PC+=2;
    }
}

void CPU::OP_6xkk() //LD Vx, byte : Set Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] = byte;
}

void CPU::OP_7xkk() // ADD Vx, byte : Set Vx = Vx + kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] += byte;
}

void CPU::OP_8xy0() //LD Vx, Vy: Set Vx = Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] = V[Vy];
}

void CPU::OP_8xy1() //OR Vx, Vy : Set Vx = Vx OR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] |= V[Vy];
}

void CPU::OP_8xy2() //AND Vx, Vy : Set Vx = Vx AND Vy
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] &= V[Vy];
}

void CPU::OP_8xy3() //XOR Vx, Vy: Set Vx = Vx XOR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] ^= V[Vy];
}

void CPU::OP_8xy4() //ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = V[Vx] + V[Vy];
    if(sum > 255u){
        V[0xF] = 1;
    }else{
        V[0xF] = 0;
    }
    V[Vx] = (sum & 0x00FFu);
}

void CPU::OP_8xy5() //SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = V[Vx] - V[Vy];
    if(V[Vx] > V[Vy]){
        V[0xF] = 1;
    }else{
        V[0xF] = 0;
    }
    V[Vx] = (sum & 0x00FFu);
}

void CPU::OP_8xy6() //SHR Vx {, Vy}: Set Vx = Vx SHR 1.
{ /*If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.*/
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[0xF] = V[Vx] & 1;
    V[Vx] =  V[Vx] >> 1;
}

void CPU::OP_8xy7() //SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = V[Vy] - V[Vx];
    if(V[Vx] < V[Vy]){
        V[0xF] = 1;
    }else{
        V[0xF] = 0;
    }
    V[Vx] = (sum & 0x00FFu);
}

void CPU::OP_8xyE() // SHL Vx {, Vy}: Set Vx = Vx SHL 1.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    //registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    V[0xF] = (V[Vx]>>7) & 1;
    V[Vx] =  V[Vx] << 1;

}

void CPU::OP_9xy0() //SNE Vx, Vy: Skip next instruction if Vx != Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(V[Vx] != V[Vy]){
        PC+=2;
    }

}

void CPU::OP_Annn() //LD I, addr: Set I = nnn.  
{
    uint8_t address = opcode & 0x0FFFu;
    I = address;
}

void CPU::OP_Bnnn() // JP V0, addr: Jump to location nnn + V0.
{
    uint8_t address = opcode & 0x0FFFu;
    PC = V[0] + address;
}

void CPU::OP_Cxkk() // RND Vx, byte: Set Vx = random byte AND kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] = getRandomByte() & byte;
}

void CPU::OP_Dxyn() //DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
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
