#include <iostream>

#include "CPU.h"

// general function for shifting val to the left one and keeping the sign (in case the sign (the 7th) bit gets shifted off, set the carry flag)
Byte CPU::sla(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

    // if the 7th bit is set, shifting left once will cause overflow
    if (val & 0x80) mRegisters.setFlag(CARRY_FLAG);
    else            mRegisters.maskFlag(CARRY_FLAG);

    val <<= 1;

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    return val;
}

void CPU::handleCBOpcodes(Byte opcode)
{
    switch (opcode)
    {
        case 0x11: // opcode 0x11, RL_C: rotate register C left
            mRegisters.C = rl(mRegisters.C);
            break;

        case 0x27: // opcode 0x27, SLA_A: shift A left, preserving the sign
            mRegisters.A = sla(mRegisters.A);
            break;

        case 0x37: // opcode 0x37, SWAP_A: swap the first 4 bits of A and the last 4 bits of A
            mRegisters.A = swap(mRegisters.A);
            break;

        case 0x50: // opcode 0x50, BIT_2_B: test the 2nd bit of B
            testBit(mRegisters.B, 2);
            break;

        case 0x58: // opcode 0x58, TEST_3_B:
            testBit(mRegisters.B, 3);
            break;

        case 0x60: // opcode 0x60, BIT_4_B: test the 4th bit of B
            testBit(mRegisters.B, 4);
            break;
    
        case 0x68: // opcode 0x68, BIT_5_B: test the 5th bit of B
            testBit(mRegisters.B, 5);
            break;

        case 0x6F: // opcode 0x6F, BIT_5_A: test the 5th bit of A
            testBit(mRegisters.A, 5);
            break;

        case 0x77: // opcode 0x77, BIT_6_A: test the 6th bit of A
            testBit(mRegisters.A, 6);
            break; 

        case 0x7C: // opcode 0x7C, BIT_7_H: test the 7th bit of H
            testBit(mRegisters.H, 7);
            break;

        case 0x87: // opcode 0x87, RES_0_A: clear the 1st bit of A
            mRegisters.A &= ~(1 << 0);
            break;

        case 0xC7: // opcode 0xC7, SET_0_A: set bit 0 of register A
            mRegisters.A |= (1) << 0;
            break;

        case 0xFE: // opcode 0xFE, SET_7_(HL): set the 7th bit of the value in memory pointed to by HL
            mmu.writeByte(mRegisters.HL, mmu.readByte(mRegisters.HL) | (1 << 7));
            break;

        default:
            std::cout << "unknown CB-prefixed opcode: 0x" << std::hex << (int)opcode << std::endl;
            std::cout << "pc: " << mRegisters.pc << std::endl;
            exit(5);
    }
}