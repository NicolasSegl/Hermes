#pragma once

#include "Cartridge.h"
#include "Constants.h"
#include "InterruptHandler.h"
#include "MMU.h"
#include "PPU.h"
#include "Registers.h"

// the CPU class fetches opcodes and interprets them
class CPU
{
private:
    Registers mRegisters;
    int mTicks;

    // handles VBLANK interupts, and in the future LCD interupts and i/o interupts
    InterruptHandler mInterruptHandler;

    // the cpu has direct access to the picture processing unit (PPU)
    PPU mPPU;

    // general opcode functions that are reusable

    Byte incByte(Byte val);    // increment byte and check flags
    Byte decByte(Byte val);    // decrement byte and check flags

    Byte rlc(Byte val);        // rotate byte left and check carry flag
    Byte rl(Byte val);         // rotate byte left and carry through carry flag. checks for carry flag as well

    Byte rrc(Byte val);        // rotate byte right and check carry flag
    Byte rr(Byte val);         // rotate byte right and carry through carry flag. checks for carry flag as well

    Byte xorB(Byte a, Byte b); // xor a byte and set the appropriate flags

    void sbc(Byte val);        // subtract val + the carry flag from A
    void sub(Byte val);        // subtract 8-bit value from register A

    void cp(Byte val);         // compare val with register A and check for flags

    DoubleByte addW(DoubleByte a, DoubleByte b); // add 16-bit register a and 16-bit register b and check for flags

    // handles all non-prefixed opcodes
    void handleOpcodes(Byte opcode, DoubleByte operand);

    // this handles the 256 case switch statement for CB-prefixed opcodes
    // sometimes, the gameboy's instructions will have the opcode of CB, and the 1 byte operand the follows
    // will tell you which extended opcode it would like to execute
    void handleCBOpcodes(Byte opcode);

public:
    CPU();

    MMU mmu;

    void emulateCycle();
};