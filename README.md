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
- **CMake** (optional, for easy project setup)

### Building the Emulator
1. Clone the repository:
   ```sh
   git clone https://github.com/your-repo/chip8-emulator.git
   cd chip8-emulator
   ```
2. Compile using g++ (assuming SDL3 is installed):
   ```sh
   g++ -std=c++17 -o chip8 main.cpp chip8.cpp -lSDL3
   ```
3. Run the emulator:
   ```sh
   ./chip8 path/to/rom.ch8
   ```

### Running a ROM
To load a CHIP-8 ROM, pass the file path as an argument:
```sh
./chip8 roms/PONG.ch8
```

## Emulator Structure

### `CPU`
Handles instruction execution, registers, and timers.

### `Memory`
Manages CHIP-8's 4KB memory, including fonts and ROM loading.

### `Display`
Uses SDL3 to render the 64x32 monochrome display.

### `Input`
Handles the CHIP-8 16-key keypad input.

## Supported CHIP-8 Instructions
The emulator implements all standard CHIP-8 opcodes, including:
- `0x00E0` – Clear screen
- `0x1NNN` – Jump to address
- `0x6XNN` – Set register VX
- `0xDXYN` – Draw sprite at VX, VY

## Future Improvements
- **Super CHIP-8 Support** (extended display mode and instructions)
- **Save states** for pausing/resuming games
- **Sound support** using SDL3 audio

## Credits
This project was developed by Jeffrey Kedda. Inspired by various CHIP-8 emulation resources and open-source projects.
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0nnn

## License
This project is licensed under the MIT License.

