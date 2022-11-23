#include <stdio.h>
#include <iostream>

#include "CPU.h"

// each element can be indexed by the opcode, and it will tell the CPU how big (in bytes) the operand it needs to read in is
Byte OPCODE_OPERAND_SIZE[256] =
{
    0, 2, 1, 0, 0, 0, 1, 
};

CPU::CPU()
{
    mRegisters.reset();
}

// could define two arrays up here. one with a size of 256 that has the operand size of each opcode, and another than has the 

void CPU::emulateCycle()
{
    // fetch an instruction
    Byte opcode = mmu.readByte(mRegisters.pc);
    std::cout << "opcode: " << (int)opcode << std::endl;

    // increment the program counter to the next instruction
    mRegisters.pc++;

    // find the operand that is to be used in the opcode's instruction
    DoubleByte operand;

    // the following if statements ensure that we properly fetch the operand (without overflowing into the next Byte)
    if (OPCODE_OPERAND_SIZE[opcode] == 1)
        operand = (DoubleByte)mmu.readByte(mRegisters.pc);
    else if (OPCODE_OPERAND_SIZE[opcode] == 2)
        operand = mmu.readDoubleByte(mRegisters.pc);

    // increase the program counter by the number of bytes that the operand took up
    mRegisters.pc += OPCODE_OPERAND_SIZE[opcode];

    // decode the instruction
    // opcode table can be found here: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    // with lots of information on each opcode here: https://rgbds.gbdev.io/docs/v0.6.0/gbz80.7/
    switch (opcode)
    {
        // opcode 0x0, NOP: no operation (does nothing)
        case 0x0: 
            break;
        
        case 0x1: // opcode 0x1, LD_BC_NN: loads NN into the register BC
            mRegisters.BC = operand;
            break;

        case 0x2: // opcode 0x2: LD_BC_A: set the address that BC is pointing to to A
            mmu.writeByte(mRegisters.BC, mRegisters.A);
            break;

        case 0x3: // opcode 0x3: increment register BC  
            mRegisters.BC++; // note that for 16 bit registers, we don't set or clear any flags when incrementing
            break;
        
        case 0x4: // opcode 0x4: increment register B
            mRegisters.B = incByte(mRegisters.B);
            break;

        case 0x5: // opcode 0x5: decrement register B
            break;
    };
}

Byte CPU::incByte(Byte val)
{
    // if the value in the register we are incrementing's lower bit is already equal to 0xF, then incrementing it will overflow it
    // meaning we'd have to set the half-carry flag, and otherwise we'll have to unset it
    if (val & 0x0F)
        mRegisters.setFlag(HALF_CARRY_FLAG);
    else
        mRegisters.maskFlag(HALF_CARRY_FLAG);

    // actually increment the value
    val++;

    // set the ZERO flag in register F if the result ended in a zero
    if (val == 0)
        mRegisters.setFlag(ZERO_FLAG);
    else
        mRegisters.maskFlag(ZERO_FLAG);

    // clear the subtraction flag (as incrementing is addition not subtraction)
    mRegisters.maskFlag(SUBTRACTION_FLAG);

    return val;
}