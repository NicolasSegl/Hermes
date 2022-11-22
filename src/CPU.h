#pragma once

#include "Cartridge.h"
#include "Constants.h"
#include "Registers.h"

// the CPU class fetches opcodes and interprets them
class CPU
{
private:
    Registers mRegisters;

public:
    CPU();

    void emulateCycle(Cartridge* cartridge);
};