#pragma once

#include "Cartridge.h"
#include "CPU.h"

class Emulator
{
private:
    Cartridge mCartridge;
    CPU mCPU;

public:
    void run(const char* romName);
};