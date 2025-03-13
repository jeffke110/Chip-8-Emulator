# CHIP-8 Emulator in C++

## Overview
This is a simple **CHIP-8 emulator** written in C++. The CHIP-8 is a virtual machine that was originally used in the 1970s for running small games on systems like the COSMAC VIP and Telmac 1800. This emulator allows you to load and play CHIP-8 ROMs by interpreting CHIP-8 opcodes and simulating their execution.

## Features
- **CPU Emulation:** Executes CHIP-8 instructions.
- **Memory Management:** 4KB of memory, including reserved system memory and loaded ROMs.
- **Graphics Rendering:** 64x32 monochrome display.
- **Input Handling:** Simulated CHIP-8 keypad.
- **Timers:** Implements delay and sound timers.
- **ROM Loading:** Loads and executes CHIP-8 programs.

## Getting Started

### Prerequisites
To build and run the emulator, you need:
- **C++17 or later**
- **SDL3** (for graphics and input handling)

### Building the Emulator (Windows)
1. Clone the repository:
   ```sh
   git clone https://github.com/your-repo/chip8-emulator.git
   cd chip8-emulator
   ```
2. Compile using the makefile:
   ```sh
   make
   ```
3. Run the emulator (first argument: cycle delay, second argument: CHIP-8 ROM):
   ```sh
   ./output/chip8 10 ./games/Pong.ch8
   ```

## Emulator Structure

### `Chip8.cpp`
Handles instruction execution, registers, and timers.
Manages CHIP-8's 4KB memory, including fonts and ROM loading.

### `Graphics.cpp`
Uses SDL3 to render the 64x32 monochrome display.
Handles the CHIP-8 16-key keypad input.

## Supported CHIP-8 Instructions
The emulator implements all standard CHIP-8 opcodes, including:
- `0x00E0` – Clear screen
- `0x1NNN` – Jump to address
- `0x6XNN` – Set register VX
- `0xDXYN` – Draw sprite at VX, VY

## Credits
This project was developed by Jeffrey Kedda. Inspired by various CHIP-8 emulation resources and open-source projects.
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0nnn

## License
This project is licensed under the MIT License.

