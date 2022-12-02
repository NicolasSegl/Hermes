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

/* 
    each element represents the number of ticks (clock cycles) that each instruction takes
    when an element has a value of 0, it means that the number of ticks the opcode takes
    is dependent on its operands

    taken from the emulator Cinoop: https://github.com/CTurt/Cinoop/blob/master/source/cb.c
    (for some reason Cinoop's array contains the number of ticks each opcode takes divided by 2?)
*/
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

// taken from the emulator Cinoop: https://github.com/CTurt/Cinoop/blob/master/source/cb.c
const Byte cbOpcodeTicks[256] = 
{
    8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x0-0xF
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x10-0x1F
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x20-0x2F
	8, 8, 8, 8, 8,  8, 16, 8,  8, 8, 8, 8, 8, 8, 16, 8, // 0x30-0x3F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x40-0x4F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x50-0x5F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x60-0x6F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x70-0x7F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x80-0x8F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0x90-0x9F
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xA0-0xAF
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xB0-0xBF
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xC0-0xCF
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xD0-0xDF
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8, // 0xE0-0xEF
	8, 8, 8, 8, 8,  8, 12, 8,  8, 8, 8, 8, 8, 8, 12, 8  // 0xF0-0xFF
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

    // if the opcode is going to go into the CB-prefixed opcode table, then add the number of ticks the CB-prefixed opcode will take
    if (opcode == 0xCB)
        mTicks += cbOpcodeTicks[(Byte)operand];

    handleOpcodes(opcode, operand);

    // tick as well the ppu (telling it how many cycles the CPU has just used)
    mPPU.tick(mTicks - oldTicks, &mmu);

    if (mRegisters.pc >= 0x100 && !finishedBios)
    {
        mRegisters.pc = 0;
        finishedBios = true;
    }

    mInterruptHandler.checkInterupts(&mRegisters, &mmu);
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
        val += 1;
        mRegisters.setFlag(CARRY_FLAG);
    }
    else
    {
        val <<= 1;
        mRegisters.maskFlag(CARRY_FLAG);
    }

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for rotating a byte left and setting the rightmost bit if the carry flag was already set. it also checks to see if the carry flag should be set
Byte CPU::rl(Byte val)
{
    // set the following variable to 1 if the carry flag is set, and 0 otherwise
    Byte carry = (mRegisters.F & CARRY_FLAG) >> 4;

    // if the leftmost bit of val is set
    if (val & 0x80)
        mRegisters.setFlag(CARRY_FLAG);
    else
        mRegisters.maskFlag(CARRY_FLAG);

    // left shift val and apply the carry
    val <<= 1;
    val += carry;

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

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

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

    return val;
}

// general function for rotating a byte right and setting the leftmost bit if the carry flag was already set. it also checks to see if the carry flag should be set
Byte CPU::rr(Byte val)
{
    // set the following variable to 1 if the carry flag is set, and 0 otherwise
    Byte carry = (mRegisters.F & CARRY_FLAG) >> 4;

    // if the leftmost bit of val is set
    if (val & 0x1)
        mRegisters.setFlag(CARRY_FLAG);
    else
        mRegisters.maskFlag(CARRY_FLAG);

    // left shift val and apply the carry
    val >>= 1;
    val |= carry << 7;

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    // clear all the other flags
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

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

    // if the first 12 bits of a plus the first 12 bits of b result in an overflow, set the half-carry flag to true
    if ((a & 0x0FFF) + (b & 0x0FFF) > 0x0FFF)
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
    mRegisters.maskFlag(NEGATIVE_FLAG);

    DoubleByte result = a + b;

    // set the carry flag if a and b's addition would cause an overflow
    if (result & 0xFF00) mRegisters.setFlag(CARRY_FLAG);
    else                 mRegisters.maskFlag(CARRY_FLAG);

    // set the half carry flag if there is going to be a carry in the first 4 bits of the byte
    if ((a & 0xF) + (b & 0xF) > 0xF) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                             mRegisters.setFlag(HALF_CARRY_FLAG);

    // set the zero flag if the result ends up being 0
    if (result == 0) mRegisters.setFlag(ZERO_FLAG);
    else             mRegisters.setFlag(ZERO_FLAG);

    return result & 0xFF;
}

// general function for adding a and b together as well as the carry flag
Byte CPU::addBC(Byte a, Byte b)
{
    mRegisters.maskFlag(NEGATIVE_FLAG);

    DoubleByte result = a + b;
    Byte carry = (mRegisters.F & CARRY_FLAG) ? 1 : 0;
    result += carry;

    // set the carry flag if a and b's addition would cause an overflow
    if (result & 0xFF00) mRegisters.setFlag(CARRY_FLAG);
    else                 mRegisters.maskFlag(CARRY_FLAG);

    // set the half carry flag if there is going to be a carry in the first 4 bits of the byte
    if (carry + (a & 0xF) + (b & 0xF) > 0xF) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                     mRegisters.setFlag(HALF_CARRY_FLAG);

    // set the zero flag if the result ends up being 0
    if ((result & 0xFF) == 0) mRegisters.setFlag(ZERO_FLAG);
    else                      mRegisters.setFlag(ZERO_FLAG);

    return result & 0xFF;
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
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // set or clear the half-carry flag
    if ((((mRegisters.A & 0xF) - (val & 0xF)) & 0x10) == 0x10) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                                       mRegisters.maskFlag(HALF_CARRY_FLAG);
}

// general function for xoring a byte, setting all flags but the zero flag to 0. it only sets the zero flag if the result is zero
Byte CPU::xorB(Byte a, Byte b)
{
    mRegisters.maskFlag(CARRY_FLAG | HALF_CARRY_FLAG | NEGATIVE_FLAG);

    if ((a ^ b)) mRegisters.maskFlag(ZERO_FLAG);
    else         mRegisters.setFlag(ZERO_FLAG);

    return a ^ b;
}

// general function for bitwise ORing a byte against register A
void CPU::orB(Byte val)
{
    // make all but the zero flags
    mRegisters.maskFlag(NEGATIVE_FLAG | CARRY_FLAG | HALF_CARRY_FLAG);

    mRegisters.A |= val;

    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for bitwise ANDing against register A
void CPU::andB(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | CARRY_FLAG);
    mRegisters.setFlag(HALF_CARRY_FLAG);

    mRegisters.A &= val;
    
    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for calling the function at addr (and storing the current address to the stack)
void CPU::call(DoubleByte addr)
{
    // push the current address onto the stack
    mRegisters.sp -= 2;
    mmu.writeDoubleByte(mRegisters.sp, mRegisters.pc);

    // set the program counter equal to the address at the start of the subroutine
    mRegisters.pc = addr;
}

// general function for returning from a function call
void CPU::ret()
{
    mRegisters.pc = mmu.readDoubleByte(mRegisters.sp);
    mRegisters.sp += 2;
}