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

// general function for shifting val to the right (and not rotating it or keeping the sign)
Byte CPU::srl(Byte val)
{
    if (val & 0x1) mRegisters.setFlag(CARRY_FLAG);
    else           mRegisters.maskFlag(CARRY_FLAG);

    val >>= 1;

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

    return val;
} 

// general function for swapping the first and last 4 bits of val
Byte CPU::swap(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | CARRY_FLAG | HALF_CARRY_FLAG);

    Byte result = (val >> 4) | (val << 4);

    if (result == 0) mRegisters.setFlag(ZERO_FLAG);
    else             mRegisters.maskFlag(ZERO_FLAG);

    return result;
}

// general function for testing the given bit in the given value and checking for flags
void CPU::testBit(Byte val, Byte bit)
{
    if ((val & (1 << bit)) == 0) mRegisters.setFlag(ZERO_FLAG);
    else                         mRegisters.maskFlag(ZERO_FLAG);

    mRegisters.maskFlag(NEGATIVE_FLAG);
    mRegisters.setFlag(HALF_CARRY_FLAG);
}

void CPU::handleCBOpcodes(Byte opcode)
{
    switch (opcode)
    {
        case 0x00: // opcode 0x00, RLC_B: rotate register B left with the carry
            mRegisters.B = rlc(mRegisters.B);
            break;

        case 0x01: // opcode 0x01, RLC_C: rotate register C left with the carry
            mRegisters.C = rlc(mRegisters.C);
            break;

        case 0x02: // opcode 0x002, RLC_D: rotate D left with carry
            mRegisters.D = rlc(mRegisters.D);
            break;

        case 0x11: // opcode 0x11, RL_C: rotate register C left
            mRegisters.C = rl(mRegisters.C);
            break;

        case 0x19: // opcode 0x19, RR_C: rotate C right
            mRegisters.C = rr(mRegisters.C);
            break;

        case 0x1A: // opcode 0x1A, RR_D: rotate D right
            mRegisters.D = rr(mRegisters.D);
            break;

        case 0x1B: // opcode 0x1B, RR_E: rotate E right
            mRegisters.E = rr(mRegisters.E);
            break;

        case 0x27: // opcode 0x27, SLA_A: shift A left, preserving the sign
            mRegisters.A = sla(mRegisters.A);
            break;

        case 0x33: // opcode 0x33, SWAP_E: swap the first 4 bits of E with the last 4 bits of E
            mRegisters.E = swap(mRegisters.E);
            break;

        case 0x37: // opcode 0x37, SWAP_A: swap the first 4 bits of A and the last 4 bits of A
            mRegisters.A = swap(mRegisters.A);
            break;

        case 0x38: // opcode 0x38, SRL_B: shift B right once
            mRegisters.B = srl(mRegisters.B);
            break;

        case 0x3F: // opcode 0x3F, SRL_A: shift A right once
            mRegisters.A = srl(mRegisters.A);
            break;

        case 0x40: // opcode 0x40, BIT_0_B: test the 0th bit of B
            testBit(mRegisters.B, 0);
            break;

        case 0x50: // opcode 0x50, BIT_2_B: test the 2nd bit of B
            testBit(mRegisters.B, 2);
            break;

        case 0x58: // opcode 0x58, TEST_3_B: test the 3rd bit of B
            testBit(mRegisters.B, 3);
            break;

        case 0x5F: // opcode 0x5F, BIT_3_A: test the 3rd bit of A
            testBit(mRegisters.A, 3);
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

        case 0x7E: // opcode 0x7E, BIT_7_(HL): test the 7th bit of the value pointed to in memory by HL
            testBit(mmu.readByte(mRegisters.HL), 7);
            break;

        case 0x7F: // opcode 0x7F, BIT_7_A: test the 7th bit of A
            testBit(mRegisters.A, 7);
            break;

        case 0x86: // opcode 0x86, RES_0_(HL): clear the 0th bit of the byte pointed to in memory by HL
            mmu.writeByte(mRegisters.HL, mmu.readByte(mRegisters.HL) & ~(1 << 0));
            break;

        case 0x87: // opcode 0x87, RES_0_A: clear the 0th bit of A
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