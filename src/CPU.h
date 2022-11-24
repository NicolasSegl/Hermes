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

    Byte incByte(Byte val); // increment byte and check flags
    Byte decByte(Byte val); // decrement byte and check flags

    Byte rlc(Byte val);     // rotate byte left and check carry flag
    Byte rl(Byte val);      // rotate byte left and carry through carry flag. checks for carry flag as well

    Byte rrc(Byte val);     // rotate byte right and check carry flag
    Byte rr(Byte val);      // rotate byte right and carry through carry flag. checks for carry flag as well

    DoubleByte addW(DoubleByte a, DoubleByte b); // add 16-bit register a and 16-bit register b and check for flags

public:
    CPU();

    MMU mmu;

    void emulateCycle();
};