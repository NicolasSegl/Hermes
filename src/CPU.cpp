#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "CPU.h"

// each element can be indexed by the opcode, and it will tell the CPU how big (in bytes) the operand it needs to read in is
Byte OPCODE_OPERAND_SIZE[256] =
{
    0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 1, 0, // 0x0-0xF
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x10-0x1F
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x20-0x2F
    1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x30-0x3F
    0, 0
};

CPU::CPU()
{
    mRegisters.reset();
}

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

    // because it is impossible for C++ to create variables in switch statements, this placeholder Double Byte must be declared outside of it 
    DoubleByte placeHolderW;

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

        case 0x3: // opcode 0x3, INC_BC: increment register BC  
            mRegisters.BC++; // note that for 16 bit registers, we don't set or clear any flags when incrementing
            break;
        
        case 0x4: // opcode 0x4, INC_B: increment register B
            mRegisters.B = incByte(mRegisters.B);
            break;

        case 0x5: // opcode 0x5, DEC_B: decrement register B
            mRegisters.B = decByte(mRegisters.B);
            break;

        case 0x6: // opcode 0x6, LD_B_N: load N into register B
            mRegisters.B = (Byte)operand;
            break;

        case 0x7: // opcode 0x7, RLC_A: rotate register A left once, and set the carry flag if there was a wrap
            mRegisters.A = rlc(mRegisters.A);
            break;

        case 0x8: // opcode 0x8, LD_NN_SP: store the stack pointer's value at memory address NN
            mmu.writeDoubleByte(operand, mRegisters.sp);
            break;

        case 0x9: // opcode 0x9, ADD_HL_BC: add register BC to register HL
            mRegisters.HL = addW(mRegisters.HL, mRegisters.BC);
            break;

        case 0xA: // opcode 0xA, LD_A_BC: load the value that BC is pointing to into register A
            mRegisters.A = mmu.readByte(mRegisters.BC);
            break;

        case 0xB: // opcode 0xB, DEC_BC: decrement the 16-bit register BC
            mRegisters.BC--; // 16-bit registers don't require checking for flags when decrementing
            break;

        case 0xC: // opcode 0xC, INC_C: increment the register C
            mRegisters.C = incByte(mRegisters.C);
            break;

        case 0xD: // opcode 0xD, DEC_C: decrement the register C
            mRegisters.C = decByte(mRegisters.C);
            break;

        case 0xE: // opcode 0xE, LD_C_N: load N into register C
            mRegisters.C = (Byte)operand;
            break;

        case 0xF: // opcode 0xF, RPC_A: rotate register A right once, and set the carry flag if there was a wrap
            mRegisters.A = rrc(mRegisters.A);
            break;

        case 0x10: // opcode 0x10, STOP: 
            std::cout << "STOP opcode called, 0x10\n";
            break;

        case 0x11: // opcode 0x11, LD_DE_NN: load the value NN into register DE
            mRegisters.DE = operand;
            break;

        case 0x12: // opcode 0x12, LD_DE_A: store the value of register A into the memory address pointed to by register DE
            mmu.writeByte(mRegisters.DE, mRegisters.A);
            break;

        case 0x13: // opcode 0x13, INC_DE: increment 16-bit register DE
            mRegisters.DE++;
            break;

        case 0x14: // opcode 0x14, IND_C: increment register D
            mRegisters.D = incByte(mRegisters.D);
            break;

        case 0x15: // opcode 0x15, DEC_D: decrement register D
            mRegisters.D = decByte(mRegisters.D);
            break;

        case 0x16: // opcode 0x16, LD_D_N: set register D to N
            mRegisters.D = (Byte)operand;
            break;

        case 0x17: // opcode 0x17, RL_A: rotate A left, WITHOUT checking for the carry flag
            mRegisters.A = rl(mRegisters.A);
            break;

        case 0x18: // opcode 0x18, JR_N: jump, relative to the current memory address, to the memory address N (which is a signed integer!)
            mRegisters.pc += (Signedbyte)operand;
            break;

        case 0x19: // opcode 0x19, ADD_HL_DE: add register DE to register HL
            mRegisters.HL = addW(mRegisters.HL, mRegisters.DE);
            break;

        case 0x1A: // opcode 0x1A, LD_A_DE: store the value pointed to by DE into register A
            mRegisters.A = mmu.readByte(mRegisters.DE);
            break;

        case 0x1B: // opcode 0x1B, DEC_DE: decrement register DE
            mRegisters.DE--;
            break;
        
        case 0x1C: // opcode 0x1C, INC_E: increment register E
            mRegisters.E = incByte(mRegisters.E);
            break;

        case 0x1D: // opcode 0x1D, DEC_E: decrement register E
            mRegisters.E = decByte(mRegisters.E);
            break;
            
        case 0x1E: // opcode 0x1E, LD_E_N: load the value of N into register E
            mRegisters.E = (Byte)operand;
            break;

        case 0x1F: // opcode 0x1F, RR_A: rotate register A right once, rotating through the carry
            mRegisters.A = rr(mRegisters.A);
            break;
        
        case 0x20: // opcode 0x20: JR_NZ_N: if the last result was not zero, then jump signed N bytes ahead in memory
            if (!(mRegisters.F & ZERO_FLAG))
                mRegisters.pc += (Signedbyte)operand;

            break;

        case 0x21: // opcode 0x21, LD_HL_NN: load the value of NN into register HL
            mRegisters.HL = operand;
            break;

        case 0x22: // opcode 0x22, LDI_HL_A: store the value of Register A into the memory address pointed to by HL, and then increment HL
            mmu.writeByte(mRegisters.HL, mRegisters.A);
            mRegisters.HL++;
            break;

        case 0x23: // opcode 0x23, INC_HL: increment register HL
            mRegisters.HL++;
            break;

        case 0x24: // opcode 0x24, INC_H: increment register H
            mRegisters.H = incByte(mRegisters.H);
            break;
        
        case 0x25: // opcode 0x25, DEC_H, decrement register H
            mRegisters.H = decByte(mRegisters.H);
            break;

        case 0x26: // opcode 0x26, LD_H_N: load the value of N into register H
            mRegisters.H = (Byte)operand;
            break;

        case 0x27: // opcode 0x27, DAA: adjust register A so that the BCD (binary coded decimal) representation is accurate after an arithmetic operation has occurred

            // i fail to understand exactly what this operation is doing. to my knowledge, it is doing some hardware specific
            // flag checking, and trying to remove ambiguity in some way?

            placeHolderW = mRegisters.A;

            if (mRegisters.F & SUBTRACTION_FLAG)
            {
                if (mRegisters.F & HALF_CARRY_FLAG)
                    placeHolderW = (placeHolderW - 0x06) & 0xFF;

                if (mRegisters.F & CARRY_FLAG)
                    placeHolderW -= 0x60;
            }
            else
            {
                if ((mRegisters.F & HALF_CARRY_FLAG) || (placeHolderW & 0xF) > 9)
                    placeHolderW += 0x6;
                
                if ((mRegisters.F & CARRY_FLAG) || placeHolderW > 0x9F)
                    placeHolderW += 0x60;
            }

            // set register A to its now altered value
            mRegisters.A = placeHolderW;
            mRegisters.maskFlag(HALF_CARRY_FLAG);

            if (mRegisters.A == 0)
                mRegisters.setFlag(ZERO_FLAG);
            else
                mRegisters.maskFlag(ZERO_FLAG);

            if (mRegisters.A >= 0x100)
                mRegisters.setFlag(CARRY_FLAG);

            break;

        case 0x28: // opcode 0x28 JR_Z_N, jump to the relative address of N (which is a signed integer! could mean we jump backwards) if the last operation resulted in a zero
            if (mRegisters.F & ZERO_FLAG)
                mRegisters.pc += (Signedbyte)operand;

            break;

        case 0x29: // opcode 0x29, ADD_HL_HL: add HL to itself (times it by 2)
            mRegisters.HL = addW(mRegisters.HL, mRegisters.HL);
            break;

        case 0x2A: // opcode 0x2A, LDI_A_HL: load the value stored in memory that is pointed to by HL into register A, then increment HL
            mRegisters.A = mmu.readByte(mRegisters.HL);
            mRegisters.HL++;
            break;

        case 0x2B: // opcode 0x2B, DEC_HL: decrement register HL
            mRegisters.HL--;
            break;

        case 0x2C: // opcode 0x2C, INC_L: increment register L
            mRegisters.L = incByte(mRegisters.L);
            break;

        case 0x2D: // opcode 0x2D, DEC_L: decrement register L
            mRegisters.L = decByte(mRegisters.L);
            break;
        
        case 0x2E: // opcode 0x2E, LD_L_N: load the value of N into register L
            mRegisters.L = (Byte)operand;
            break;

        case 0x2F: // opcode 0x2F, CPL: logical not register A
            mRegisters.A = ~mRegisters.A;
            mRegisters.setFlag(SUBTRACTION_FLAG | HALF_CARRY_FLAG);
            break;

        case 0x30: // opcode 0x30, JR_NC_N: relative jump to signed N if the last instruction resulted in no carry
            if (!(mRegisters.F & CARRY_FLAG))
                mRegisters.pc += (Signedbyte)operand;

            break;

        case 0x31: // opcode 0x31, LD_SP_NN: set the stack pointer equal to NN
            mRegisters.sp = operand;
            break;

        case 0x32: // opcode 0x32, LDD_HL_A: save the value of register A into the memory address pointed to by HL, and then decrement HL
            mmu.writeByte(mRegisters.HL, mRegisters.A);
            mRegisters.HL--;
            break;

        case 0x33: // opcode 0x33, INC_SP: increment the stack pointer
            mRegisters.sp++;
            break;

        case 0x34: // opcode 0x34, INC_(HL): increment the value that HL is pointed at
            mmu.writeByte(mRegisters.HL, incByte(mmu.readByte(mRegisters.HL)));
            break;

        case 0x35: // opcode 0x35, DEC_(HL): decrement the value that HL is pointed at
            mmu.writeByte(mRegisters.HL, decByte(mmu.readByte(mRegisters.HL)));
            break;

        case 0x36: // opcode 0x36, LD_(HL)_N: load the value of N into the memory address that HL is pointed at
            mmu.writeByte(mRegisters.HL, (Byte)operand);
            break;

        case 0x37: // opcode 0x37, SCF: set the carry flag
            mRegisters.setFlag(CARRY_FLAG);
            break;

        case 0x38: // opcode 0x38, JR_C_N: relative jump by signed N, if the last result resulted in the carry flag being set
            if (mRegisters.F & CARRY_FLAG)
                mRegisters.pc += (Signedbyte)operand;

            break;

        case 0x39: // opcode 0x39, ADD_HL_SP: add the value of the stack pointer to HL
            mRegisters.HL = addW(mRegisters.HL, mRegisters.sp);
            break;

        case 0x3A: // opcode 0x3A, LDD_A_(HL): load the value of the memory address pointed to by HL into register A, and then decrement HL
            mRegisters.A = mmu.readByte(mRegisters.HL);
            mRegisters.HL--;
            break;

        case 0x3B: // opcode 0x3B, DEC_SP: decrement the stack pointer
            mRegisters.sp--;
            break;

        case 0x3C: // opcode 0x3C, INC_A: increment the register A
            mRegisters.A = incByte(mRegisters.A);
            break;

        case 0x3D: // opcode 0x3D, DEC_A: decrement the register A
            mRegisters.A = decByte(mRegisters.A);
            break;

        case 0x3E: // opcode 0x3E, LD_A_N: load the value of N into register A
            mRegisters.A = (Byte)operand;
            break;
        
        case 0x3F: // opcode 0x3F, CCF: clear the carry flag
            mRegisters.maskFlag(CARRY_FLAG);
            break;

        // case 0xCB: another switch?

    };
}

// general function for incrementing a byte (usually an 8-bit register) and checking to see if any flags should be set
Byte CPU::incByte(Byte val)
{
    // if the value in the register we are incrementing's lower bit is already equal to 0xF, then incrementing it will overflow it
    // meaning we'd have to set the half-carry flag, and otherwise we'll have to unset it
    if (val & 0x0F)
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
    mRegisters.maskFlag(SUBTRACTION_FLAG);

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
    mRegisters.setFlag(SUBTRACTION_FLAG);

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
    mRegisters.maskFlag(SUBTRACTION_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

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
    mRegisters.maskFlag(SUBTRACTION_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

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
    mRegisters.maskFlag(SUBTRACTION_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

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
    mRegisters.maskFlag(SUBTRACTION_FLAG | ZERO_FLAG | HALF_CARRY_FLAG);

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
    mRegisters.maskFlag(SUBTRACTION_FLAG);

    return a + b;
}