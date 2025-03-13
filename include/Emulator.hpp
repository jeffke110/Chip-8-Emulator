#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#pragma once

#include "Chip8.hpp"
#include "Graphics.hpp"
#include <chrono>
#include <iostream>

class Emulator
{
public:
    int emulate(int argc, char **argv);
};

#endif // EMULATOR_HPP