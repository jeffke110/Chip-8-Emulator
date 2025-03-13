#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>

uint8_t fontset[FONTSET_SIZE] =
    {
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
uint8_t Chip8::getRandomByte()
{
    static std::random_device rd;  // Seed generator
    static std::mt19937 gen(rd()); // Mersenne Twister PRNG
    static std::uniform_int_distribution<uint16_t> dist(0, 255);
    return static_cast<uint8_t>(dist(gen));
}
Chip8::Chip8()
{
    // initialize PC (0x200)
    PC = START_ADDRESS;

    // load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
    // set up function pointer table
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}
void Chip8::LoadROM(char const *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        for (long i = 0; i < size; ++i)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }
        delete[] buffer;
    }
}

void Chip8::Cycle()
{
    // Fetch
    opcode = (memory[PC] << 8u) | memory[PC + 1];

    // Increment the PC before we execute anything
    PC += 2;
    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Decrement the delay timer if it's been set
    if (delay_timer > 0)
    {
        --delay_timer;
    }

    // Decrement the sound timer if it's been set
    if (sound_timer > 0)
    {
        --sound_timer;
    }
}
uint8_t *Chip8::getRegisters()
{
    return registers;
}
uint16_t *Chip8::getStack()
{
    return stack;
}
uint16_t Chip8::getIndex()
{
    return index;
}
uint16_t Chip8::getPC()
{
    return PC;
}
uint8_t Chip8::getSP()
{
    return SP;
}
uint16_t Chip8::getOpcode()
{
    return opcode;
}
uint8_t Chip8::getSoundTimer()
{
    return sound_timer;
}
uint8_t Chip8::getDelayTimer()
{
    return delay_timer;
}
uint8_t *Chip8::getMemory()
{
    return memory;
}
std::string Chip8::getInstruction()
{
    return instruction;
}
void Chip8::OP_NULL()
{
    instruction = "NULL";
}

void Chip8::OP_00E0() //clear the display
{
    memset(video, 0, sizeof(video)); //set all the bytes in the video variable to 0.
    instruction = "Clear the display";
}

void Chip8::OP_00EE() //RET: Return from a subroutine.
{
    --SP;
    PC = stack[SP];
    instruction = "Return from a subroutine";
}

void Chip8::OP_1nnn() //JP addr: Jump to location nnn.
{
    uint16_t address = opcode & 0x0FFFu;

    PC = address;
    instruction = "JP addr " + std::to_string(address);
}

void Chip8::OP_2nnn() // CALL addr: Call subroutine at nnn.
{
    uint16_t address = opcode & 0x0FFFu;

    stack[SP] = PC;
    ++SP;
    PC = address;
    instruction = "CALL addr " + std::to_string(address);
}

void Chip8::OP_3xkk() // SE Vx, byte: Skip next instruction if Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
    {
        PC += 2;
    }
    instruction = "SE Vx, byte " + std::to_string(Vx) + ", " + std::to_string(byte);
}

void Chip8::OP_4xkk() // SNE Vx, byte: Skip next instruction if Vx != kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
    {
        PC += 2;
    }
    instruction = "SNE Vx, byte " + std::to_string(Vx) + ", " + std::to_string(byte);
}

void Chip8::OP_5xy0() //SE Vx, Vy: Skip next instruction if Vx = Vy.
{ 
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
    {
        PC += 2;
    }
    instruction = "SE Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
}

void Chip8::OP_6xkk() //LD Vx, byte : Set Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
    instruction = "LD Vx, byte " + std::to_string(Vx) + ", " + std::to_string(byte);
}

void Chip8::OP_7xkk() // ADD Vx, byte : Set Vx = Vx + kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
    instruction = "ADD Vx, byte " + std::to_string(Vx) + ", " + std::to_string(byte);
}

void Chip8::OP_8xy0() //LD Vx, Vy: Set Vx = Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
    instruction = "LD Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
}

void Chip8::OP_8xy1() //OR Vx, Vy : Set Vx = Vx OR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
    instruction = "OR Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
    
}

void Chip8::OP_8xy2() //AND Vx, Vy : Set Vx = Vx AND Vy
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
    instruction = "AND Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
}
 
void Chip8::OP_8xy3() //XOR Vx, Vy: Set Vx = Vx XOR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
    instruction = "XOR Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
}

void Chip8::OP_8xy4() //ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255U)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    instruction = "ADD Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5() //SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    instruction = "SUB Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6() //SHR Set Vx = Vx SHR 1.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save LSB in VF
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
    instruction = "SHR Vx " + std::to_string(Vx);
}

void Chip8::OP_8xy7() //SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    instruction = "SUBN Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE() // SHL Set Vx = Vx SHL 1.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
    instruction = "SHL Vx " + std::to_string(Vx);
}

void Chip8::OP_9xy0() //SNE Vx, Vy: Skip next instruction if Vx != Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        PC += 2;
    }
    instruction = "SNE Vx, Vy " + std::to_string(Vx) + ", " + std::to_string(Vy);
}

void Chip8::OP_Annn() //LD I, addr: Set I = nnn.
{
    uint16_t address = opcode & 0x0FFFu;
    instruction = "LD I, addr " + std::to_string(index)  + ", " + std::to_string(address);
    index = address;
    
}

void Chip8::OP_Bnnn() // JP V0, addr: Jump to location nnn + V0.
{
    uint16_t address = opcode & 0x0FFFu;

    PC = registers[0] + address;
    instruction = "JP V0, addr " + std::to_string(registers[0]) + ", " + std::to_string(address);
}

void Chip8::OP_Cxkk() // RND Vx, byte: Set Vx = random byte AND kk.
{ 
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = getRandomByte() & byte;
    instruction = "RND Vx, byte " + std::to_string(Vx) + ", " + std::to_string(byte);
}

void Chip8::OP_Dxyn() //DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
    instruction = "DRW Vx, Vy " + std::to_string(Vx) + " " + std::to_string(Vy);
}

void Chip8::OP_Ex9E() //SKP Vx: Skip next instruction if key with the value of Vx is pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if (keypad[key])
    {
        PC += 2;
    }
    instruction = "SKP Vx " + std::to_string(Vx);
}

void Chip8::OP_ExA1() //SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if (!keypad[key])
    {
        PC += 2;
    }
    instruction = "SKNP Vx " + std::to_string(Vx);
}

void Chip8::OP_Fx07() //LD Vx, DT: Set Vx = delay timer value.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delay_timer;

    instruction = "LD Vx " + std::to_string(Vx) + ", DT";
}

void Chip8::OP_Fx0A() //LD Vx, K: Wait for a key press, store the value of the key in Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t key = 0; key < 16; ++key) {
        if (keypad[key]) {
            registers[Vx] = key;
            return;
        }
    }
    PC -= 2;
    instruction = "LD Vx " + std::to_string(Vx) + ", K";

}

void Chip8::OP_Fx15()  //LD DT, Vx: Set delay timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delay_timer = registers[Vx];
    instruction = "LD DT, Vx " + std::to_string(Vx);

}

void Chip8::OP_Fx18() //LD ST, Vx: Set sound timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    sound_timer = registers[Vx];
    instruction = "LD ST, Vx " + std::to_string(Vx);

}

void Chip8::OP_Fx1E() //ADD I, Vx: Set I = I + Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index += registers[Vx];
    instruction = "ADD I, Vx " + std::to_string(Vx);

}

void Chip8::OP_Fx29()  //LD F, Vx: Set I = location of sprite for digit Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];
    index = FONTSET_START_ADDRESS + (5 * digit);
    instruction = "LD F, Vx " + std::to_string(Vx);

}

void Chip8::OP_Fx33() //LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];
    // Ones-place
    memory[index + 2] = value % 10;
    value /= 10;
    // Tens-place
    memory[index + 1] = value % 10;
    value /= 10;
    // Hundreds-place
    memory[index] = value % 10;
    instruction = "LD B, V " + std::to_string(Vx);

}

void Chip8::OP_Fx55() //LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
{ 
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
    instruction = "LD [I], V " + std::to_string(Vx);

}

void Chip8::OP_Fx65() //LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
    }
    instruction = "LD V " + std::to_string(Vx) + ", [I]";
}

void Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}
void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}
void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}
void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}