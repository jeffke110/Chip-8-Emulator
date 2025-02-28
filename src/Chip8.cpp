#include "Chip8.hpp"
#include <algorithm>
#include <complex>
#include <random>

const uint16_t START_ADDRESS = 0x200;

Chip8::Chip8()
{
    // first 512 bytes (registers 0x000 to 0x200) are reserved for system use
    PC = START_ADDRESS;
    // load font data into memory
    for (int i = 0; i < FONT_SIZE; i++)
    {
        memory[START_ADDRESS + i] = font_data[i];
    }
    setupTable();
}
void Chip8::reset()
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
    setupTable();
}

void Chip8::setupTable()
{
    // Set up function pointer table
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


void Chip8::LoadROM(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];
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


uint8_t Chip8::getRandomByte()
{
    static std::random_device rd;  // Seed generator
    static std::mt19937 gen(rd()); // Mersenne Twister PRNG
    static std::uniform_int_distribution<uint16_t> dist(0, 255);
    return static_cast<uint8_t>(dist(gen));
}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
	--SP;
	PC = stack[SP];
}

void Chip8::OP_1nnn() //JP addr: Jump to location nnn.
{
    uint16_t address = opcode & 0x0FFFu;
    PC = address;
}

void Chip8::OP_2nnn() // CALL addr: Call subroutine at nnn.
{
    uint16_t address = opcode & 0x0FFFu;
    stack[SP] = PC;
    ++PC;
    PC = address;
}

void Chip8::OP_3xkk() // SE Vx, byte: Skip next instruction if Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //Extract Register Vx
    uint8_t byte = (opcode & 0x00FFu);      // Extract kk
    if(V[Vx] == byte){
        PC+=2;
    }
}

void Chip8::OP_4xkk() // SNE Vx, byte: Skip next instruction if Vx != kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    if(V[Vx] != byte){
        PC+=2;
    }
}

void Chip8::OP_5xy0() //SE Vx, Vy: Skip next instruction if Vx = Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(V[Vx] == V[Vy]){
        PC+=2;
    }
}

void Chip8::OP_6xkk() //LD Vx, byte : Set Vx = kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] = byte;
}

void Chip8::OP_7xkk() // ADD Vx, byte : Set Vx = Vx + kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] += byte;
}

void Chip8::OP_8xy0() //LD Vx, Vy: Set Vx = Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] = V[Vy];
}

void Chip8::OP_8xy1() //OR Vx, Vy : Set Vx = Vx OR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] |= V[Vy];
}

void Chip8::OP_8xy2() //AND Vx, Vy : Set Vx = Vx AND Vy
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] &= V[Vy];
}

void Chip8::OP_8xy3() //XOR Vx, Vy: Set Vx = Vx XOR Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] ^= V[Vy];
}

void Chip8::OP_8xy4() //ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry.
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

void Chip8::OP_8xy5() //SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow.
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

void Chip8::OP_8xy6() //SHR Vx {, Vy}: Set Vx = Vx SHR 1.
{ /*If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.*/
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[0xF] = V[Vx] & 1;
    V[Vx] =  V[Vx] >> 1;
}

void Chip8::OP_8xy7() //SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow.
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

void Chip8::OP_8xyE() // SHL Vx {, Vy}: Set Vx = Vx SHL 1.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    //registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    V[0xF] = (V[Vx]>>7) & 1;
    V[Vx] =  V[Vx] << 1;

}

void Chip8::OP_9xy0() //SNE Vx, Vy: Skip next instruction if Vx != Vy.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(V[Vx] != V[Vy]){
        PC+=2;
    }

}

void Chip8::OP_Annn() //LD I, addr: Set I = nnn.  
{
    uint8_t address = opcode & 0x0FFFu;
    I = address;
}

void Chip8::OP_Bnnn() // JP V0, addr: Jump to location nnn + V0.
{
    uint8_t address = opcode & 0x0FFFu;
    PC = V[0] + address;
}

void Chip8::OP_Cxkk() // RND Vx, byte: Set Vx = random byte AND kk.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode && 0x00FFu);
    V[Vx] = getRandomByte() & byte;
}

void Chip8::OP_Dxyn() //DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
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

void Chip8::OP_Ex9E() //SKP Vx: Skip next instruction if key with the value of Vx is pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = V[Vx];
	if (keypad[key])
	{
		PC += 2;
	}
}

void Chip8::OP_ExA1() //SKNP Vx: Skip next instruction if key with the value of Vx is not pressed.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = V[Vx];
	if (!keypad[key])
	{
		PC += 2;
	}
}

void Chip8::OP_Fx07() //LD Vx, DT: Set Vx = delay timer value.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] = delay_timer;
}

void Chip8::OP_Fx0A() //LD Vx, K: Wait for a key press, store the value of the key in Vx.
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

void Chip8::OP_Fx15() //LD DT, Vx: Set delay timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delay_timer = V[Vx];
}

void Chip8::OP_Fx18() //LD ST, Vx: Set sound timer = Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    sound_timer = V[Vx];
}

void Chip8::OP_Fx1E() //ADD I, Vx: Set I = I + Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    I += V[Vx];
}

void Chip8::OP_Fx29() //LD F, Vx: Set I = location of sprite for digit Vx.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = V[Vx];
	I = FONT_SIZE + (5 * digit);
}

void Chip8::OP_Fx33() //LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = V[Vx];

    memory[I + 2] = value % 10;
    value /= 10;

    memory[I + 1] = value % 10;
    value /= 10;

    memory[I] = value % 10;
}

void Chip8::OP_Fx55() //LD [I], Vx: Store registers V0 through Vx in memory starting at location I.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= Vx; ++i){
        memory[I + i] = V[i];
    }
}

void Chip8::OP_Fx65() //LD Vx, [I]: Read registers V0 through Vx from memory starting at location I.
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= Vx; ++i){
        V[i] = memory[I + i];
    }
}

void Chip8::OP_NULL()
{
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

