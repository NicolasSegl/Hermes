#include <stdio.h>

#include "CPU.h"

CPU::CPU()
{
    mRegisters.reset();
}

// this structure represents the parts of the opcode
// the instruction points to a function that will carry out the opcode's instruction with the operands

void CPU::emulateCycle(Cartridge* cartridge)
{
    // fetch an instruction
    // use the pc to fetch the instruction

    Byte opcode = cartridge->mmu.readByte(mRegisters.pc);
    printf("opcode: %d", opcode);

    // decode the instruction
}

// a nop has no operands, and therefor an operand length of 0
// other opcodes will have lengths of 1, or 2
// we always increment the program counter by 1
// and as well, increment the program counter by the operand length 