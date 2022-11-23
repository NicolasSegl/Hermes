#include <stdio.h>
#include <iostream>

#include "CPU.h"

// pointer to the Registers instance held by the CPU
Registers* registers;

// opcode 0x0: no operation (does nothing)
void NOP() { printf("NOP"); };

// this structure represents the parts of the opcode relevant to the emulator
struct Instruction
{
    // pointer to the function that the opcode will call
    void (*function)();

    // the size of the operands. it can range from 0 bytes to 2 bytes
    Byte operandSize;
};

// this array contains all 256 opcodes that the cpu can use
Instruction OPCODES[256] = 
{
    {NOP, 0} // opcode 0x00: nop (no operation) will do nothing
};

CPU::CPU()
{
    // set the registers pointer to point at the CPU's Registers instance
    // this is done so that the registers are accessible by the individual opcode functions
    registers = &mRegisters;
    mRegisters.reset();
}

void CPU::emulateCycle(Cartridge* cartridge)
{
    // fetch an instruction
    Byte opcode = cartridge->mmu.readByte(mRegisters.pc);
    std::cout << "opcode: " << (int)opcode << std::endl;

    // increment the program counter to the next instruction
    mRegisters.pc++;

    // find the operand that is to be used in the opcode's instruction
    DoubleByte operand;

    // the following if statements ensure that we properly fetch the operand (without overflowing into the next Byte)
    if (OPCODES[opcode].operandSize == 1)
        operand = (DoubleByte)cartridge->mmu.readByte(mRegisters.pc);
    else if (OPCODES[opcode].operandSize == 2)
        operand = cartridge->mmu.readDoubleByte(mRegisters.pc);

    // increase the program counter by the number of bytes that the operand took up
    mRegisters.pc += OPCODES[opcode].operandSize;

    // decode the instruction
    // the size of the operand will determine the return type of the opcode (if there is one at all)
    switch (OPCODES[opcode].operandSize)
    {
        case 0: // no operand
			((void (*)(void))OPCODES[opcode].function)(); // convert the function pointer into a function and call it
    }
}