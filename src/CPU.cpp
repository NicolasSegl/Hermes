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

    taken from: https://github.com/retrio/gb-test-roms/tree/master/instr_timing
*/
const Byte opcodeTicks[256] = {
	4,  12, 8, 8,  4,  4,  8,  4,  20,  8,  8,  8, 4, 4,  8, 4, // 0x0-0xF
	4,  12, 8, 8,  4,  4,  8,  4,   12, 8,  8,  8, 4, 4,  8, 4, // 0x10-0x1F
	0,  12, 8, 8,  4,  4,  8,  4,   0,  8,  8,  8, 4, 4,  8, 4, // 0x20-0x2F
	0,  12, 8, 8,  12, 12, 12, 4,   0,  8,  8,  8, 4, 4,  8, 4, // 0x30-0x3F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0x40-0x4F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0x50-0x5F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0x60-0x6F
	8,  8,  8, 8,  8,  8,  4,  8,   4,  4,  4,  4, 4, 4,  8, 4, // 0x70-0x7F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0x80-0x8F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0x90-0x9F
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0xA0-0xAF
	4,  4,  4, 4,  4,  4,  8,  4,   4,  4,  4,  4, 4, 4,  8, 4, // 0xB0-0xBF
	0,  12, 0, 16, 0,  16, 8,  16,  0,  16, 0,  0, 0, 24, 8, 16, // 0xC0-0xCF
	0,  12, 0, 0,  0,  16, 8,  16,  0,  16, 0,  0, 0, 0,  8, 16, // 0xD0-0xDF
	12, 12, 8, 0,  0,  16, 8,  16,  16, 4,  16, 0, 0, 0,  8, 16, // 0xE0-0xEF
	12, 12, 8, 4,  0,  16, 8,  16,  12, 8,  16, 4, 0, 0,  8, 16  // 0xF0-0xFF
};

// taken from https://github.com/retrio/gb-test-roms/tree/master/instr_timing
const Byte cbOpcodeTicks[256] = 
{
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x0-0xF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x10-0x1F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x20-0x2F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x30-0x3F
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x40-0x4F
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x50-0x5F
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x60-0x6F
	8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8, // 0x70-0x7F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x80-0x8F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0x90-0x9F
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0xA0-0xAF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0xB0-0xBF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0xC0-0xCF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0xD0-0xDF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, // 0xE0-0xEF
	8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8  // 0xF0-0xFF
};

// timer offsets
const DoubleByte DIV_REGISTER_OFFSET  = 0xFF04;
const DoubleByte TIMA_REGISTER_OFFSET = 0xFF05;
const DoubleByte TMA_REGISTER_OFFSET  = 0xFF06;
const DoubleByte TAC_REGISTER_OFFSET  = 0xFF07;

// initialize values for the CPU
CPU::CPU()
{
    mmu = new MMU;

    // set the ticks (as well as the ticks for the timers) to 0
    mTicks         = 0;
    mDivTimerTicks = 0;
    mTimerTicks    = 0;

    // reset all the registers
    mRegisters.reset();

    // initialize the MMU
    mmu->init(&mTicks, &mClockSpeed, &mClockEnabled);

    // initialize the PPU
    mPPU.init(mmu);
    
    // default values that the emulator assumes after the BIOS would have run
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
    mTicks += opcodeTicks[opcode];

    // if the opcode is going to go into the CB-prefixed opcode table, then add the number of ticks the CB-prefixed opcode will take
    if (opcode == 0xCB)
        mTicks += cbOpcodeTicks[(Byte)operand];

    handleOpcodes(opcode, operand);
                  
    // tick as well the ppu (telling it how many cycles the CPU has just used)
    mPPU.tick(mTicks - oldTicks);

    // update the clocks before the interrupts, because it is possible that a timer interrupt has occured after the previous opcode
    updateClocks(mTicks - oldTicks);

    mInterruptHandler.checkInterupts(opcode, &mRegisters, mmu);
}

void CPU::updateClocks(int deltaTicks)
{
    // the div register timer increments at 16384Hz (which is achieved with 1 increment every 256 ticksd)
    mDivTimerTicks += deltaTicks;
    if (mDivTimerTicks >= 256)
    {
        mDivTimerTicks -= 256;

        // the memory has to be manually updated like this (i.e., without using mmu->writeByte) because if the gameboy game attempts
        // to update the div register with any value, it will always be reset to 0, but we need to be incrementing it
        mmu->ramMemory[DIV_REGISTER_OFFSET - RAM_OFFSET]++;
    }

    // if the timer is actually enabled, then update the TIMA register
    if (mClockEnabled)
    {
        mTimerTicks += deltaTicks;

        // if enough ticks have gone by that we should increment the tick timer (according to the current clock speed,
        // which can be updated by writing to the timer control register)
        while (mTimerTicks >= mClockSpeed)
        {
            // read in the TIMA register and increment it
            Byte timerCounter = mmu->readByte(TIMA_REGISTER_OFFSET) + 1;
            mmu->writeByte(TIMA_REGISTER_OFFSET, timerCounter);

            // if the timer overflowed, then cause an interrupt and 
            // reset the timer counter to the value specified in the TMA register
            if (timerCounter == 0)
            {
                mmu->writeByte(TIMA_REGISTER_OFFSET, mmu->readByte(TMA_REGISTER_OFFSET));
                mmu->writeByte(INTERRUPT_OFFSET, mmu->readByte(INTERRUPT_OFFSET) | (Byte)Interrupts::TIMER);
            }

            // in case there was overflow (i.e., the cpu clock speed is 1 increment per 1024, but 1030 ticks went by, then we would set the current timer ticks to 4)
            mTimerTicks -= mClockSpeed;
        }
    }
}

// encode all of the current register values into a save file from addresses 0-0xB
void CPU::saveRegistersToFile(std::ofstream& file)
{
    // store all the value registers into the first 8 bytes
    file.write((char*)&mRegisters.A, 1);
    file.write((char*)&mRegisters.B, 1);
    file.write((char*)&mRegisters.C, 1);
    file.write((char*)&mRegisters.D, 1);
    file.write((char*)&mRegisters.E, 1);
    file.write((char*)&mRegisters.F, 1);
    file.write((char*)&mRegisters.H, 1);
    file.write((char*)&mRegisters.L, 1);

    // store the stack pointer and the pc
    file.write((char*)&mRegisters.sp, 2);
    file.write((char*)&mRegisters.pc, 2);
}

void CPU::saveInterruptDataToFile(std::ofstream& file)
{
    mInterruptHandler.saveDataToFile(file);
}

// load all of the registers in from a file from addresses 0-0xB
void CPU::setRegistersFromFile(std::ifstream& file)
{
    // 12 bytes in is the end of the registers (8 1-byte registers and 2 2-byte registers)
    char dataBuffer[12];

    // store the first 12 bytes of the file into dataBuffer
    file.read(dataBuffer, 12);
    
    mRegisters.A = dataBuffer[0];
    mRegisters.B = dataBuffer[1];
    mRegisters.C = dataBuffer[2];
    mRegisters.D = dataBuffer[3];
    mRegisters.E = dataBuffer[4];
    mRegisters.F = dataBuffer[5];
    mRegisters.H = dataBuffer[6];
    mRegisters.L = dataBuffer[7];

    // taking into account the little endianess (hence all the bit shifting)
    mRegisters.sp = (DoubleByte(Byte(dataBuffer[9])) << 8)  | Byte(dataBuffer[8]);
    mRegisters.pc = (DoubleByte(Byte(dataBuffer[11])) << 8) | Byte(dataBuffer[10]);
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