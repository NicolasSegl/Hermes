#pragma once

#include "Cartridge.h"
#include "Constants.h"
#include "MMU.h"
#include "Registers.h"

// the CPU class fetches opcodes and interprets them
class CPU
{
private:
    Registers mRegisters;

    // general opcode functions that are reusable
    Byte incByte(Byte val);

public:
    CPU();

    MMU mmu;

    void emulateCycle();
};