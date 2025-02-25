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
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = V[Vx] % VIDEO_WIDTH;
	uint8_t yPos = V[Vy] % VIDEO_HEIGHT;

	V[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[I + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					V[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void CPU::OP_Ex9E() //SKP Vx: Skip next instruction if key with the value of Vx is pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = V[Vx];
	if (keypad[key])
	{
		PC += 2;
	}
}

void CPU::OP_ExA1() //SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = V[Vx];
	if (!keypad[key])
	{
		PC += 2;
	}
}

void CPU::OP_Fx07() //LD Vx, DT: Set Vx = delay timer value.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] = delay_timer;
}

void CPU::OP_Fx0A() //LD Vx, K: Wait for a key press, store the value of the key in Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t key = 0; key < 16; ++key) {
        if (keypad[key]) {
            V[Vx] = key;
            return;
        }
    }
    PC -= 2;
}

void CPU::OP_Fx15() //LD DT, Vx: Set delay timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delay_timer = V[Vx];
}

void CPU::OP_Fx18() //LD ST, Vx: Set sound timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    sound_timer = V[Vx];
}

void CPU::OP_Fx1E() //ADD I, Vx: Set I = I + Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    I += V[Vx];
}

void CPU::OP_Fx29() //LD F, Vx: Set I = location of sprite for digit Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = V[Vx];
	I = FONT_SIZE + (5 * digit);
}

void CPU::OP_Fx33() //LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = V[Vx];

    memory[I + 2] = value % 10;
    value /= 10;

    memory[I + 1] = value % 10;
    value /= 10;

    memory[I] = value % 10;
}

void CPU::OP_Fx55() //LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= Vx; ++i){
        memory[I + i] = V[i];
    }
}

void CPU::OP_Fx65() //LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= Vx; ++i){
        V[i] = memory[I + i];
    }
}
