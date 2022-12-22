#include <iostream>
#include <sstream>
#include <fstream>
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

// timer offsets
const DoubleByte DIV_REGISTER_OFFSET  = 0xFF04;
const DoubleByte TIMA_REGISTER_OFFSET = 0xFF05;
const DoubleByte TMA_REGISTER_OFFSET  = 0xFF06;
const DoubleByte TAC_REGISTER_OFFSET  = 0xFF07;

FILE* debugFile;

// initialize values for the CPU
CPU::CPU()
{
    mmu = new MMU;

    // set the ticks (as well as the ticks for the timers) to 0
    mTicks         = 0;
    mDivTimerTicks = 0;

    // reset all the registers
    mRegisters.reset();

    // initialize the MMU
    mmu->init(&mTicks);

    // initialize the PPU
    mPPU.init(mmu);

    finishedBios = false;    
    
    debugFile = fopen("debugLog.txt", "w");

    // default values when bios isn't run
    mRegisters.pc = 0x100;
    mRegisters.A = 0x1;
    mRegisters.F = 0xB0;
    mRegisters.B = 0x0;
    mRegisters.C = 0x13;
    mRegisters.D = 0;
    mRegisters.E = 0xD8;
    mRegisters.H = 0x01;
    mRegisters.L = 0x4D;
    mRegisters.sp = 0xfffe;
}

// emulates a single opcode from the cpu
void CPU::emulateCycle()
{
    /*fprintf(debugFile, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        mRegisters.A, mRegisters.F, mRegisters.B, mRegisters.C, mRegisters.D, mRegisters.E, mRegisters.H, mRegisters.L,
        mRegisters.sp, mRegisters.pc, mmu->readByte(mRegisters.pc), mmu->readByte(mRegisters.pc + 1),
        mmu->readByte(mRegisters.pc + 2), mmu->readByte(mRegisters.pc + 3));
      */  

    // fetch an instruction
    Byte opcode = mmu->readByte(mRegisters.pc);
    
    // increment the program counter to the next instruction
    mRegisters.pc++;

    // find the operand that is to be used in the opcode's instruction
    DoubleByte operand;

    // the following if statements ensure that we properly fetch the operand (without overflowing into the next Byte)
    if (OPCODE_OPERAND_SIZE[opcode] == 1)
        operand = mmu->readByte(mRegisters.pc);
    else if (OPCODE_OPERAND_SIZE[opcode] == 2)
        operand = mmu->readDoubleByte(mRegisters.pc);

    // increase the program counter by the number of bytes that the operand took up
    mRegisters.pc += OPCODE_OPERAND_SIZE[opcode];

    // record the old number of ticks (used to accurately update the number of ticks that have passed to the PPU,
    // as sometimes the number of ticks that an instruction takes is dependent on various conditions)
    uint64_t oldTicks = mTicks;

    // adds the number of ticks the opcode took
    mTicks += opcodeTicks[opcode] * 2;

    // if the opcode is going to go into the CB-prefixed opcode table, then add the number of ticks the CB-prefixed opcode will take
    if (opcode == 0xCB)
        mTicks += cbOpcodeTicks[(Byte)operand];

    handleOpcodes(opcode, operand); // issue in the operand size table?
                              
    // tick as well the ppu (telling it how many cycles the CPU has just used)
    mPPU.tick(mTicks - oldTicks, mmu);

    if (mRegisters.pc >= 0x100 && !finishedBios)
    {
        // mRegisters.pc = 0;
        finishedBios = true;
    }

    mInterruptHandler.checkInterupts(&mRegisters, mmu);

    // the div register timer increments at 16384Hz
    mDivTimerTicks += mTicks - oldTicks;
    if (mDivTimerTicks >= 16384)
    {
        mDivTimerTicks = 0;

        // the memory has to be manually updated like this (i.e., without using mmu->writeByte) because if the gameboy game attempts
        // to update the div register with any value, it will always be reset to 0, but we need to be incrementing it
        mmu->ramMemory[DIV_REGISTER_OFFSET - 0x8000]++;
    }
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
    Byte carry = mRegisters.isFlagSet(CARRY_FLAG);

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
    Byte carry = mRegisters.isFlagSet(CARRY_FLAG);

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