#include <iostream>

#include "CPU.h"

// general function for shifting val to the left one and keeping the sign (in case the sign (the 7th) bit gets shifted off, set the carry flag)
Byte CPU::sla(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);

    // if the 7th bit is set, shifting left once will cause overflow (this is what preserves the sign)
    if (val & 0x80) mRegisters.setFlag(CARRY_FLAG);
    else            mRegisters.maskFlag(CARRY_FLAG);

    val <<= 1;

    if (val == 0) mRegisters.setFlag(ZERO_FLAG);
    else          mRegisters.maskFlag(ZERO_FLAG);

    return val;
}

// general function for shifting val to the right one and keeping the sign
Byte CPU::sra(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);
    
    // if the 1st bit is set, shifting right once will cause overflow (this is what preserves the sign)
    if (val & 0x1) mRegisters.setFlag(CARRY_FLAG);
    else mRegisters.maskFlag(CARRY_FLAG);

    val = (val & 0x80) | (val >> 1);

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

        case 0x02: // opcode 0x02, RLC_D: rotate D left with carry
            mRegisters.D = rlc(mRegisters.D);
            break;

        case 0x03: // opcode 0x03, RLC_E: rotate E left with carry
            mRegisters.E = rlc(mRegisters.E);
            break;

        case 0x04: // opcode 0x04, RLC_H: rotate H left with carry
            mRegisters.H = rlc(mRegisters.H);
            break;

        case 0x05: // opcode 0x05, RLC_L: rotate L left with carry
            mRegisters.L = rlc(mRegisters.L);
            break;

        case 0x06: // opcode 0x06, RLC_(HL): rotate the value pointed to in memory by HL left with the carry
            mmu->writeByte(mRegisters.HL, rlc(mmu->readByte(mRegisters.HL)));
            break;

        case 0x07: // opcode 0x07, RLC_A: rotate A left with carry
            mRegisters.A = rlc(mRegisters.A);
            break;

        case 0x08: // opcode 0x08, RRC_B: rotate B right with carry
            mRegisters.B = rrc(mRegisters.B);
            break;

        case 0x09: // opcode 0x09, RRC_C: rotate C right with carry
            mRegisters.C = rrc(mRegisters.C);
            break;

        case 0x0A: // opcode 0x0A, RRC_D: rotate D with carry
            mRegisters.D = rrc(mRegisters.D);
            break;

        case 0x0B: // opcode 0x0B, RRC_E: rotate E right with carry
            mRegisters.E = rrc(mRegisters.E);
            break;

        case 0x0C: // opcode 0x0C, RRC_H: rotate H right with carry
            mRegisters.H = rrc(mRegisters.H);
            break;

        case 0x0D: // opcode 0x0D, RRC_L: rotate L right with carry
            mRegisters.L = rrc(mRegisters.L);
            break;

        case 0x0E: // opcode 0x0E, RRC_(HL): rotate the value in memory pointed to by HL right with carry
            mmu->writeByte(mRegisters.HL, rrc(mmu->readByte(mRegisters.HL)));
            break;

        case 0x0F: // opcode 0x0F, RRC_A: rotate A right with carry
            mRegisters.A = rrc(mRegisters.A);
            break;

        case 0x10: // opcode 0x10, RL_B: rotate B left
            mRegisters.B = rl(mRegisters.B);
            break;

        case 0x11: // opcode 0x11, RL_C: rotate register C left
            mRegisters.C = rl(mRegisters.C);
            break;

        case 0x12: // opcode 0x12, RL_D: rotate D left
            mRegisters.D = rl(mRegisters.D);
            break;

        case 0x13: // opcode 0x13, RL_E: rotate E left
            mRegisters.E = rl(mRegisters.E);
            break;

        case 0x14: // opcode 0x14, RL_H: rotate H left
            mRegisters.H = rl(mRegisters.H);
            break;

        case 0x15: // opcode 0x15, RL_L: rotate L left
            mRegisters.L = rl(mRegisters.L);
            break;

        case 0x16: // opcode 0x16, RL_(HL): rotate the value pointed to in memory by HL left 
            mmu->writeByte(mRegisters.HL, rl(mmu->readByte(mRegisters.HL)));
            break;

        case 0x17: // opcode 0x17, RL_A: rotate A left
            mRegisters.A = rl(mRegisters.A);
            break;

        case 0x18: // opcode 0x18, RR_B: rotate B right
            mRegisters.B = rr(mRegisters.B);
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

        case 0x1C: // opcode 0x1C, RR_H: rotate H right
            mRegisters.H = rr(mRegisters.H);
            break;

        case 0x1D: // opcode 0x1D, RR_L: rotate L right
            mRegisters.L = rr(mRegisters.L);
            break;

        case 0x1E: // opcode 0x1E, RR_(HL): rotate the value pointed to in memory by HL right
            mmu->writeByte(mRegisters.HL, rr(mmu->readByte(mRegisters.HL)));
            break;

        case 0x1F: // opcode 0x1F, RR_A: rotate A right
            mRegisters.A = rr(mRegisters.A);
            break;

        case 0x20: // opcode 0x20, SLA_B: shift B left, preserving the sign
            mRegisters.B = sla(mRegisters.B);
            break;

        case 0x21: // opcode 0x21, SLA_C: shift C left, preserving the sign
            mRegisters.C = sla(mRegisters.C);
            break;

        case 0x22: // opcode 0x22, SLA_D: shift D left, preserving the sign
            mRegisters.D = sla(mRegisters.D);
            break;

        case 0x23: // opcode 0x23, SLA_E: shift E left, preserving the sign
            mRegisters.E = sla(mRegisters.E);
            break;

        case 0x24: // opcode 0x24, SLA_H: shift H left, preserving the sign
            mRegisters.H = sla(mRegisters.H);
            break;

        case 0x25: // opcode 0x25, SLA_L: shift L left, preserving the sign
            mRegisters.L = sla(mRegisters.L);
            break;

        case 0x26: // opcode 0x24, SLA_(HL): shift the value pointed to in memory by HL left, preserving the sign
            mmu->writeByte(mRegisters.HL, sla(mmu->readByte(mRegisters.HL)));
            break;

        case 0x27: // opcode 0x27, SLA_A: shift A left, preserving the sign
            mRegisters.A = sla(mRegisters.A);
            break;

        case 0x28: // opcode 0x28, SRA_B: shift B right, preserving the sign
            mRegisters.B = sra(mRegisters.B);
            break;

        case 0x29: // opcode 0x29, SRC_C: shift C right, preserving the sign
            mRegisters.C = sra(mRegisters.C);
            break;

        case 0x2A: // opcode 0x2A, SRA_D: shift D right, preserving the sign
            mRegisters.D = sra(mRegisters.D);
            break;

        case 0x2B: // opcode 0x2B, SRA_E: shift E right, preserving the sign
            mRegisters.E = sra(mRegisters.E);
            break;

        case 0x2C: // opcode 0x2C, SRA_H: shift H right, preserving the sign
            mRegisters.H = sra(mRegisters.H);
            break;

        case 0x2D: // opcode 0x2D, SRA_L: shift L right, preserving the sign
            mRegisters.L = sra(mRegisters.L);
            break;

        case 0x2E: // opcode 0x2E, SRA_(HL): shift the value pointed to in memory by HL right, preserving the sign
            mmu->writeByte(mRegisters.HL, sra(mmu->readByte(mRegisters.HL)));
            break;

        case 0x2F: // opcode 0x2F, SRA_A: shift A right, preserving the sign
            mRegisters.A = sra(mRegisters.A);
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

        case 0x41: // opcode 0x41, BIT_0_C: test the 0th bit of C
            testBit(mRegisters.C, 0);
            break;

        case 0x47: // opcode 0x47, BIT_0_A: test the 0th bit of A
            testBit(mRegisters.A, 0);
            break;

        case 0x48: // opcode 0x48, BIT_1_B: test the 1st bit of B
            testBit(mRegisters.B, 1);
            break;

        case 0x50: // opcode 0x50, BIT_2_B: test the 2nd bit of B
            testBit(mRegisters.B, 2);
            break;

        case 0x57: // opcode 0x57, BIT_2_A: test the 2nd bit of A
            testBit(mRegisters.A, 2);
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

        case 0x61: // opcode 0x61, BIT_4_C: test the 4th bit of C
            testBit(mRegisters.C, 4);
            break;
    
        case 0x68: // opcode 0x68, BIT_5_B: test the 5th bit of B
            testBit(mRegisters.B, 5);
            break;

        case 0x69: // opcode 0x69, TEST_5_C: test the 5th bit of C
            testBit(mRegisters.C, 5);
            break;

        case 0x6F: // opcode 0x6F, BIT_5_A: test the 5th bit of A
            testBit(mRegisters.A, 5);
            break;

        case 0x70: // opcode 0x70, BIT_6_B: test the 6th bit of B
            testBit(mRegisters.B, 6);
            break;

        case 0x71: // opcode 0x71, BIT_6_C: test the 6th bit of C
            testBit(mRegisters.C, 6);
            break;

        case 0x77: // opcode 0x77, BIT_6_A: test the 6th bit of A
            testBit(mRegisters.A, 6);
            break; 

        case 0x78: // opcode 0x78, BIT_7_B: test the 7th bit of B
            testBit(mRegisters.B, 7);
            break;

        case 0x79: // opcode 0x79, BIT_7_C: test the 7th bit of C
            testBit(mRegisters.C, 7);
            break;

        case 0x7C: // opcode 0x7C, BIT_7_H: test the 7th bit of H
            testBit(mRegisters.H, 7);
            break;

        case 0x7E: // opcode 0x7E, BIT_7_(HL): test the 7th bit of the value pointed to in memory by HL
            testBit(mmu->readByte(mRegisters.HL), 7);
            break;

        case 0x7F: // opcode 0x7F, BIT_7_A: test the 7th bit of A
            testBit(mRegisters.A, 7);
            break;

        case 0x86: // opcode 0x86, RES_0_(HL): clear the 0th bit of the byte pointed to in memory by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 0));
            break;

        case 0x87: // opcode 0x87, RES_0_A: clear the 0th bit of A
            mRegisters.A &= ~(1 << 0);
            break;

        case 0x9E: // opcode 0x9E, RES_3_(HL): clear the 3d bit of the byte pointed to in memory by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 3));
            break;

        case 0xBE: // opcode 0xBE, RES_7_(HL): clear the 7th bit of the byte pointed to in memory by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 7));
            break;

        case 0xC7: // opcode 0xC7, SET_0_A: set bit 0 of register A
            mRegisters.A |= (1) << 0;
            break;

        case 0xDE: // opcode 0xDE, SET_3_(HL): set the 3rd bit of the value pointed to in memory by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 3));
            break;

        case 0xF6: // opcode 0xF6, SET_6_(HL): set the 6th bit of the value pointed to in memory by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 6));
            break;

        case 0xFE: // opcode 0xFE, SET_7_(HL): set the 7th bit of the value in memory pointed to by HL
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 7));
            break;

        default:
            std::cout << "unknown CB-prefixed opcode: 0x" << std::hex << (int)opcode << std::endl;
            std::cout << "pc: " << mRegisters.pc << std::endl;
            exit(5);
    }
}