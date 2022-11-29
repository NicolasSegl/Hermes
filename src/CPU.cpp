#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "CPU.h"

// each element can be indexed by the opcode, and it will tell the CPU how big (in bytes) the operand it needs to read in is
const Byte OPCODE_OPERAND_SIZE[256] =
{
    0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 1, 0, // 0x0-0xF
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x10-0x1F
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x20-0x2F
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x30-0x3F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40-0x4F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50-0x5F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x60-0x6F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x70-0x7F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80-0x8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90-0x9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xA0-0xAF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0-0xBF
    0, 0, 2, 2, 2, 0, 1, 0, 0, 0, 2, 1, 2, 2, 1, 0, // 0xC0-0xCF
    0, 0, 2, 0, 2, 0, 1, 0, 0, 0, 2, 0, 2, 0, 1, 0, // 0xD0-0xDF
    1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 1, 0, // 0xE0-0xEF
    1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 0, 0, 0, 1, 0, // 0xF0-0xFF
};

// each element represents the number of ticks (clock cycles) that each instruction takes
// when an element has a value of 0, it means that the number of ticks the opcode takes
// is dependent on its operands
const Byte opcodeTicks[256] = {
	2, 6, 4, 4, 2, 2, 4, 4, 10, 4, 4, 4, 2, 2, 4, 4, // 0x0-0xF
	2, 6, 4, 4, 2, 2, 4, 4,  4, 4, 4, 4, 2, 2, 4, 4, // 0x10-0x1F
	0, 6, 4, 4, 2, 2, 4, 2,  0, 4, 4, 4, 2, 2, 4, 2, // 0x20-0x2F
	4, 6, 4, 4, 6, 6, 6, 2,  0, 4, 4, 4, 2, 2, 4, 2, // 0x30-0x3F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x40-0x4F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x50-0x5F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x60-0x6F
	4, 4, 4, 4, 4, 4, 2, 4,  2, 2, 2, 2, 2, 2, 4, 2, // 0x70-0x7F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x80-0x8F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0x90-0x9F
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0xA0-0xAF
	2, 2, 2, 2, 2, 2, 4, 2,  2, 2, 2, 2, 2, 2, 4, 2, // 0xB0-0xBF
	0, 6, 0, 6, 0, 8, 4, 8,  0, 2, 0, 0, 0, 6, 4, 8, // 0xC0-0xCF
	0, 6, 0, 0, 0, 8, 4, 8,  0, 8, 0, 0, 0, 0, 4, 8, // 0xD0-0xDF
	6, 6, 4, 0, 0, 8, 4, 8,  8, 2, 8, 0, 0, 0, 4, 8, // 0xE0-0xEF
	6, 6, 4, 2, 0, 8, 4, 8,  6, 4, 8, 2, 0, 0, 4, 8  // 0xF0-0xFF
};

/* 
    the BIOS contains the instructions that the gameboy uses to boot up. 
    the assembly instructions for the hexadecimal below can be found here:
    https://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM#The_DMG_bootstrap
*/
const Byte BIOS[256] = 
{
    0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
    0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
    0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
    0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
    0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
    0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
    0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
    0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
    0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
    0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
    0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
    0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x00, 0x00, 0x23, 0x7D, 0xFE, 0x34, 0x20,
    0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x00, 0x00, 0x3E, 0x01, 0xE0, 0x50
};

// initialize values for the CPU
CPU::CPU()
{
    // set the ticks to 0
    mTicks = 0;

    // reset all the registers
    mRegisters.reset();

    // initialize the MMU
    mmu.init();

    // initialize the PPU
    mPPU.init(&mmu);

    finishedBios = false;
}

// emulates a single opcode from the cpu
void CPU::emulateCycle()
{
    // fetch an instruction
    Byte opcode = mmu.readByte(mRegisters.pc);

    // increment the program counter to the next instruction
    mRegisters.pc++;

    // find the operand that is to be used in the opcode's instruction
    DoubleByte operand;

    // the following if statements ensure that we properly fetch the operand (without overflowing into the next Byte)
    if (OPCODE_OPERAND_SIZE[opcode] == 1)
        operand = mmu.readByte(mRegisters.pc);
    else if (OPCODE_OPERAND_SIZE[opcode] == 2)
        operand = mmu.readDoubleByte(mRegisters.pc);

    // increase the program counter by the number of bytes that the operand took up
    mRegisters.pc += OPCODE_OPERAND_SIZE[opcode];

    // record the old number of ticks (used to accurately update the number of ticks that have passed to the PPU,
    // as sometimes the number of ticks that an instruction takes is dependent on various conditions)
    int oldTicks = mTicks;

    // adds the number of ticks the opcode took
    mTicks += opcodeTicks[opcode] * 2;

    handleOpcodes(opcode, operand);

    // tick as well the ppu (telling it how many cycles the CPU has just used)
    mPPU.tick(mTicks - oldTicks, &mmu);

    if (mRegisters.pc >= 0x100 && !finishedBios)
    {
        mRegisters.pc = 0;
        finishedBios = true;
    }
}

// general function for incrementing a byte (usually an 8-bit register) and checking to see if any flags should be set
Byte CPU::incByte(Byte val)
{
    // if the value in the register we are incrementing's lower bit is already equal to 0xF, then incrementing it will overflow it
    // meaning we'd have to set the half-carry flag, and otherwise we'll have to unset it
    if ((val & 0xF) == 0xF)
        mRegisters.setFlag(HALF_CARRY_FLAG);
    else
        mRegisters.maskFlag(HALF_CARRY_FLAG);

    val++;

    // set the ZERO flag in register F if the result ended in a zero
    if (val == 0)
        mRegisters.setFlag(ZERO_FLAG);
    else
        mRegisters.maskFlag(ZERO_FLAG);

    // clear the subtraction flag (as incrementing is addition not subtraction)
    mRegisters.maskFlag(NEGATIVE_FLAG);

    return val;
}

// general function for decrementing a byte (usually an 8-bit register) and checking to see if any flags should be set
Byte CPU::decByte(Byte val)
{
    // check if any of the first 4 bits of the byte are set. if any of them are set, then decrementing will not cause a borrow of any kind
    // if none of them are set, however, then decremting will borrow from the more significant digits, which means we need to set the half-carry flag
    if (val & 0x0F)
        mRegisters.maskFlag(HALF_CARRY_FLAG); // no borrowing
    else
        mRegisters.setFlag(HALF_CARRY_FLAG);  // borrowing

    val--;

    if (val == 0)
        mRegisters.setFlag(ZERO_FLAG);
    else
        mRegisters.maskFlag(ZERO_FLAG);

    // set the subtraction flag (as decrementing is a subtraction)
    mRegisters.setFlag(NEGATIVE_FLAG);

    return val;
}

// general function for rotating a byte left (usually an 8-bit register), checking to see if the carry flag should be set, and clearing all other flags
Byte CPU::rlc(Byte val)
{
    // if the leftmost bit is set, then shifting once to the left will cause an overflow (which will set the carry flag)
    if (val & 0x80)
    {
        // shift the value once to the left and then wrap the bit around to the front
        val <<= 1;
        val |= 1;
        mRegisters.setFlag(CARRY_FLAG);
    }
    else
    {
        val <<= 1;
        mRegisters.maskFlag(CARRY_FLAG);
    }

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for rotating a byte left and setting the rightmost bit if the carry flag was already set. it also checks to see if the carry flag should be set
Byte CPU::rl(Byte val)
{
    // set the following variable to 1 if the carry flag is set, and 0 otherwise
    Byte carry = mRegisters.F & CARRY_FLAG >> 4;

    // if the leftmost bit of val is set
    if (val & 0x80)
        mRegisters.setFlag(CARRY_FLAG);
    else
        mRegisters.maskFlag(CARRY_FLAG);

    // left shift val and apply the carry
    val <<= 1;
    val |= carry;

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for rotating a byte right and checking flags
Byte CPU::rrc(Byte val)
{
    // if the right most bit is set, wrap said bit to the left side of val
    if (val & 0x1)
    {
        val >>= 1;
        val |= (1) << 7;
        mRegisters.setFlag(CARRY_FLAG);
    }
    else
    {
        val >>= 1;
        mRegisters.maskFlag(CARRY_FLAG);
    }

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for rotating a byte right and setting the leftmost bit if the carry flag was already set. it also checks to see if the carry flag should be set
Byte CPU::rr(Byte val)
{
    // set the following variable to 1 if the carry flag is set, and 0 otherwise
    Byte carry = mRegisters.F & CARRY_FLAG >> 4;

    // if the leftmost bit of val is set
    if (val & 0x1)
        mRegisters.setFlag(CARRY_FLAG);
    else
        mRegisters.maskFlag(CARRY_FLAG);

    // left shift val and apply the carry
    val >>= 1;
    val |= carry << 7;

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for adding two 16-bit registers together and checks for subtraction flag, half-carry flag, and carry flag
DoubleByte CPU::addW(DoubleByte a, DoubleByte b)
{
    // define result as 32 bit integer so that it can contain the potential overflow
    uint32_t result = a + b;

    // if the 32 bit result of adding the two numbers together is greater than 0xFFFF, then it means an overflow would have occured in the 16-bit addition
    if (result & 0xFFFF0000)
        mRegisters.setFlag(CARRY_FLAG);
    else
        mRegisters.maskFlag(CARRY_FLAG);

    // if the first 4 bits of a plus the first 4 bits of b result in an overflow, set the half-carry flag to true
    if ((a & 0xF) + (b & 0xF) > 0xF)
        mRegisters.setFlag(HALF_CARRY_FLAG);
    else
        mRegisters.maskFlag(HALF_CARRY_FLAG);

    // clear the subtraction flag
    mRegisters.maskFlag(NEGATIVE_FLAG);

    return a + b;
}

// general function for adding two 8-bit registers together and checking for flags
Byte CPU::addB(Byte a, Byte b)
{
    // clear the negative flag
    mRegisters.maskFlag(NEGATIVE_FLAG);

    DoubleByte result = a + b;

    // set the carry flag if a and b's addition would cause an overflow
    if (result & 0xFF00) mRegisters.setFlag(CARRY_FLAG);
    else              mRegisters.maskFlag(CARRY_FLAG);

    // set the half carry flag if there is going to be a carry in the first 4 bits of the byte
    if ((a & 0xF) + (b & 0xF) > 0xF) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                             mRegisters.setFlag(HALF_CARRY_FLAG);

    // set the zero flag if the result ends up being 0
    if (result == 0) mRegisters.setFlag(ZERO_FLAG);
    else            mRegisters.setFlag(ZERO_FLAG);

    return result;
}

// general function for subtracting an 8-bit value from register A and checking for flags
void CPU::sub(Byte val)
{
    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // if the subtraction would result in an overflow
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // if the lower bits of the register would overflow
    if ((val & 0xF) > (mRegisters.A & 0xF)) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                    mRegisters.maskFlag(HALF_CARRY_FLAG);

    // subtract the value from register A
    mRegisters.A -= val;

    // set the zero flag if register A is now zero
    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for subtracting a (value PLUS the carry flag) from register A
void CPU::sbc(Byte val)
{
    if (mRegisters.F & CARRY_FLAG) val += 1;

    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // set the carry flag if val is greater than A (as then subtracting them will cause an overflow)
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // set the zero flag if val is equal to A (as then subtracting them will equal 0)
    if (val == mRegisters.A) mRegisters.setFlag(ZERO_FLAG);
    else                     mRegisters.maskFlag(ZERO_FLAG);

    // if the first 4 bits of val are greater than the first 4 bits of A, set the half carry flag as subtracting them will cause overflow in the first 4 bits
    if ((val & 0xF) > (mRegisters.A & 0xF)) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                    mRegisters.maskFlag(HALF_CARRY_FLAG);

    // subtract the val from A
    mRegisters.A -= val;
}

void CPU::cp(Byte val)
{
    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // set or clear the zero flag
    if (mRegisters.A == val) mRegisters.setFlag(ZERO_FLAG);
    else                     mRegisters.maskFlag(ZERO_FLAG);

    // set or clear the carry flag
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                          mRegisters.maskFlag(CARRY_FLAG);

    // set or clear the half-carry flag
    if ((((mRegisters.A & 0xF) - (val & 0xF)) & 0x10) == 0x10) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                                       mRegisters.maskFlag(HALF_CARRY_FLAG);
}

// general function for xoring a byte, setting all flags but the zero flag to 0. it only sets the zero flag if the result is zero
Byte CPU::xorB(Byte a, Byte b)
{
    mRegisters.setFlag(CARRY_FLAG | HALF_CARRY_FLAG | NEGATIVE_FLAG);

    if ((a ^ b)) mRegisters.maskFlag(ZERO_FLAG);
    else         mRegisters.setFlag(ZERO_FLAG);

    return a ^ b;
}

void CPU::handleCBOpcodes(Byte opcode)
{
    switch (opcode)
    {
        case 0x11: // opcode 0x11, rotate register C left
            mRegisters.C = rl(mRegisters.C);
            break;

        case 0xC7: // opcode 0xC7, set bit 0 of register A
            mRegisters.A |= (1) << 0;
            break;
    }
}