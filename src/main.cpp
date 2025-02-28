#include <Emulator.hpp>


int main(int argc, char** argv)
{
    Emulator chip8;
    chip8.Emulate(argc, argv);
}
