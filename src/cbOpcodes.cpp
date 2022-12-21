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

        case 0x30: // opcode 0x30, SWAP_B: swap the first 4 bits of B with the last 4 bits of B 
            mRegisters.B = swap(mRegisters.B);
            break;

        case 0x31: // opcode 0x31, SWAP_C: swap the first 4 bits of C with the last 4 bits of C
            mRegisters.C = swap(mRegisters.C);
            break;

        case 0x32: // opcode 0x32, SWAP_D: swap the first 4 bits of D with the last 4 bits of D
            mRegisters.D = swap(mRegisters.D);
            break;

        case 0x33: // opcode 0x33, SWAP_E: swap the first 4 bits of E with the last 4 bits of E
            mRegisters.E = swap(mRegisters.E);
            break;

        case 0x34: // opcode 0x34, SWAP_H: swap the first 4 bits of H with the last 4 bits of H
            mRegisters.H = swap(mRegisters.H);
            break;

        case 0x35: // opcode 0x35, SWAP_L: swap the first 4 bits of L with the last 4 bits of L
            mRegisters.L = swap(mRegisters.L);
            break;

        case 0x36: // opcode 0x36, SWAP_(HL): swap the first 4 bits of what HL is pointed at with the last 4 bits of what HL is pointing at
            mmu->writeByte(mRegisters.HL, swap(mmu->readByte(mRegisters.HL)));
            break;

        case 0x37: // opcode 0x37, SWAP_A: swap the first 4 bits of A and the last 4 bits of A
            mRegisters.A = swap(mRegisters.A);
            break;

        case 0x38: // opcode 0x38, SRL_B: shift B right once
            mRegisters.B = srl(mRegisters.B);
            break;

        case 0x39: // opcode 0x39, SRL_C: shift C right once
            mRegisters.C = srl(mRegisters.C);
            break;

        case 0x3A: // opcode 0x3A, SRL_D: shift D right once
            mRegisters.D = srl(mRegisters.D);
            break;

        case 0x3B: // opcode 0x3B, SRL_E: shift E right once
            mRegisters.E = srl(mRegisters.E);
            break;
        
        case 0x3C: // opcode 0x3C, SRL_H: shift H right once
            mRegisters.H = srl(mRegisters.H);
            break;

        case 0x3D: // opcode 0x3D, SRL_L: shift L right once
            mRegisters.L = srl(mRegisters.L);
            break;

        case 0x3E: // opcode 0x3E, SRL_(HL): shift what HL is pointing to in memory right once
            mmu->writeByte(mRegisters.HL, srl(mmu->readByte(mRegisters.HL)));
            break;

        case 0x3F: // opcode 0x3F, SRL_A: shift A right once
            mRegisters.A = srl(mRegisters.A);
            break;

        /*
            all opcodes from 0x40-0x7F test if a certain bit is set
        */

        case 0x40:
            testBit(mRegisters.B, 0);
            break;

        case 0x41:
            testBit(mRegisters.C, 0);
            break;

        case 0x42:
            testBit(mRegisters.D, 0);
            break;

        case 0x43:
            testBit(mRegisters.E, 0);
            break;

        case 0x44:
            testBit(mRegisters.H, 0);
            break;

        case 0x45:
            testBit(mRegisters.L, 0);
            break;

        case 0x46:
            testBit(mmu->readByte(mRegisters.HL), 0);
            break;

        case 0x47:
            testBit(mRegisters.A, 0);
            break;

        case 0x48:
            testBit(mRegisters.B, 1);
            break;

        case 0x49:
            testBit(mRegisters.C, 1);
            break;

        case 0x4a:
            testBit(mRegisters.D, 1);
            break;

        case 0x4b:
            testBit(mRegisters.E, 1);
            break;

        case 0x4c:
            testBit(mRegisters.H, 1);
            break;

        case 0x4d:
            testBit(mRegisters.L, 1);
            break;

        case 0x4e:
            testBit(mmu->readByte(mRegisters.HL), 1);
            break;

        case 0x4f:
            testBit(mRegisters.A, 1);
            break;

        case 0x50:
            testBit(mRegisters.B, 2);
            break;

        case 0x51:
            testBit(mRegisters.C, 2);
            break;

        case 0x52:
            testBit(mRegisters.D, 2);
            break;

        case 0x53:
            testBit(mRegisters.E, 2);
            break;

        case 0x54:
            testBit(mRegisters.H, 2);
            break;

        case 0x55:
            testBit(mRegisters.L, 2);
            break;

        case 0x56:
            testBit(mmu->readByte(mRegisters.HL), 2);
            break;

        case 0x57:
            testBit(mRegisters.A, 2);
            break;

        case 0x58:
            testBit(mRegisters.B, 3);
            break;

        case 0x59:
            testBit(mRegisters.C, 3);
            break;

        case 0x5a:
            testBit(mRegisters.D, 3);
            break;

        case 0x5b:
            testBit(mRegisters.E, 3);
            break;

        case 0x5c:
            testBit(mRegisters.H, 3);
            break;

        case 0x5d:
            testBit(mRegisters.L, 3);
            break;

        case 0x5e:
            testBit(mmu->readByte(mRegisters.HL), 3);
            break;

        case 0x5f:
            testBit(mRegisters.A, 3);
            break;

        case 0x60:
            testBit(mRegisters.B, 4);
            break;

        case 0x61:
            testBit(mRegisters.C, 4);
            break;

        case 0x62:
            testBit(mRegisters.D, 4);
            break;

        case 0x63:
            testBit(mRegisters.E, 4);
            break;

        case 0x64:
            testBit(mRegisters.H, 4);
            break;

        case 0x65:
            testBit(mRegisters.L, 4);
            break;

        case 0x66:
            testBit(mmu->readByte(mRegisters.HL), 4);
            break;

        case 0x67:
            testBit(mRegisters.A, 4);
            break;

        case 0x68:
            testBit(mRegisters.B, 5);
            break;

        case 0x69:
            testBit(mRegisters.C, 5);
            break;

        case 0x6a:
            testBit(mRegisters.D, 5);
            break;

        case 0x6b:
            testBit(mRegisters.E, 5);
            break;

        case 0x6c:
            testBit(mRegisters.H, 5);
            break;

        case 0x6d:
            testBit(mRegisters.L, 5);
            break;

        case 0x6e:
            testBit(mmu->readByte(mRegisters.HL), 5);
            break;

        case 0x6f:
            testBit(mRegisters.A, 5);
            break;

        case 0x70:
            testBit(mRegisters.B, 6);
            break;

        case 0x71:
            testBit(mRegisters.C, 6);
            break;

        case 0x72:
            testBit(mRegisters.D, 6);
            break;

        case 0x73:
            testBit(mRegisters.E, 6);
            break;

        case 0x74:
            testBit(mRegisters.H, 6);
            break;

        case 0x75:
            testBit(mRegisters.L, 6);
            break;

        case 0x76:
            testBit(mmu->readByte(mRegisters.HL), 6);
            break;

        case 0x77:
            testBit(mRegisters.A, 6);
            break;

        case 0x78:
            testBit(mRegisters.B, 7);
            break;

        case 0x79:
            testBit(mRegisters.C, 7);
            break;

        case 0x7a:
            testBit(mRegisters.D, 7);
            break;

        case 0x7b:
            testBit(mRegisters.E, 7);
            break;

        case 0x7c:
            testBit(mRegisters.H, 7);
            break;

        case 0x7d:
            testBit(mRegisters.L, 7);
            break;

        case 0x7e:
            testBit(mmu->readByte(mRegisters.HL), 7);
            break;

        case 0x7f:
            testBit(mRegisters.A, 7);
            break;

        /*
            the opcodes from 0x80-0xBF reset a certain bit in the register/memory address
        */

        case 0x80:
            mRegisters.B &= ~(1 << 0);
            break;

        case 0x81:
            mRegisters.C &= ~(1 << 0);
            break;

        case 0x82:
            mRegisters.D &= ~(1 << 0);
            break;

        case 0x83:
            mRegisters.E &= ~(1 << 0);
            break;

        case 0x84:
            mRegisters.H &= ~(1 << 0);
            break;

        case 0x85:
            mRegisters.L &= ~(1 << 0);
            break;

        case 0x86:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 0));
            break;

        case 0x87:
            mRegisters.A &= ~(1 << 0);
            break;

        case 0x88:
            mRegisters.B &= ~(1 << 1);
            break;

        case 0x89:
            mRegisters.C &= ~(1 << 1);
            break;

        case 0x8a:
            mRegisters.D &= ~(1 << 1);
            break;

        case 0x8b:
            mRegisters.E &= ~(1 << 1);
            break;

        case 0x8c:
            mRegisters.H &= ~(1 << 1);
            break;

        case 0x8d:
            mRegisters.L &= ~(1 << 1);
            break;

        case 0x8e:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 1));
            break;

        case 0x8f:
            mRegisters.A &= ~(1 << 1);
            break;

        case 0x90:
            mRegisters.B &= ~(1 << 2);
            break;

        case 0x91:
            mRegisters.C &= ~(1 << 2);
            break;

        case 0x92:
            mRegisters.D &= ~(1 << 2);
            break;

        case 0x93:
            mRegisters.E &= ~(1 << 2);
            break;

        case 0x94:
            mRegisters.H &= ~(1 << 2);
            break;

        case 0x95:
            mRegisters.L &= ~(1 << 2);
            break;

        case 0x96:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 2));
            break;

        case 0x97:
            mRegisters.A &= ~(1 << 2);
            break;

        case 0x98:
            mRegisters.B &= ~(1 << 3);
            break;

        case 0x99:
            mRegisters.C &= ~(1 << 3);
            break;

        case 0x9a:
            mRegisters.D &= ~(1 << 3);
            break;

        case 0x9b:
            mRegisters.E &= ~(1 << 3);
            break;

        case 0x9c:
            mRegisters.H &= ~(1 << 3);
            break;

        case 0x9d:
            mRegisters.L &= ~(1 << 3);
            break;

        case 0x9e:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 3));
            break;

        case 0x9f:
            mRegisters.A &= ~(1 << 3);
            break;

        case 0xa0:
            mRegisters.B &= ~(1 << 4);
            break;

        case 0xa1:
            mRegisters.C &= ~(1 << 4);
            break;

        case 0xa2:
            mRegisters.D &= ~(1 << 4);
            break;

        case 0xa3:
            mRegisters.E &= ~(1 << 4);
            break;

        case 0xa4:
            mRegisters.H &= ~(1 << 4);
            break;

        case 0xa5:
            mRegisters.L &= ~(1 << 4);
            break;

        case 0xa6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 4));
            break;

        case 0xa7:
            mRegisters.A &= ~(1 << 4);
            break;

        case 0xa8:
            mRegisters.B &= ~(1 << 5);
            break;

        case 0xa9:
            mRegisters.C &= ~(1 << 5);
            break;

        case 0xaa:
            mRegisters.D &= ~(1 << 5);
            break;

        case 0xab:
            mRegisters.E &= ~(1 << 5);
            break;

        case 0xac:
            mRegisters.H &= ~(1 << 5);
            break;

        case 0xad:
            mRegisters.L &= ~(1 << 5);
            break;

        case 0xae:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 5));
            break;

        case 0xaf:
            mRegisters.A &= ~(1 << 5);
            break;

        case 0xb0:
            mRegisters.B &= ~(1 << 6);
            break;

        case 0xb1:
            mRegisters.C &= ~(1 << 6);
            break;

        case 0xb2:
            mRegisters.D &= ~(1 << 6);
            break;

        case 0xb3:
            mRegisters.E &= ~(1 << 6);
            break;

        case 0xb4:
            mRegisters.H &= ~(1 << 6);
            break;

        case 0xb5:
            mRegisters.L &= ~(1 << 6);
            break;

        case 0xb6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 6));
            break;

        case 0xb7:
            mRegisters.A &= ~(1 << 6);
            break;

        case 0xb8:
            mRegisters.B &= ~(1 << 7);
            break;

        case 0xb9:
            mRegisters.C &= ~(1 << 7);
            break;

        case 0xba:
            mRegisters.D &= ~(1 << 7);
            break;

        case 0xbb:
            mRegisters.E &= ~(1 << 7);
            break;

        case 0xbc:
            mRegisters.H &= ~(1 << 7);
            break;

        case 0xbd:
            mRegisters.L &= ~(1 << 7);
            break;

        case 0xbe:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) & ~(1 << 7));
            break;

        case 0xbf:
            mRegisters.A &= ~(1 << 7);
            break;

        /*
            all opcodes from 0xC0-0xFF set a certain bit in the register or memory address
        */

        case 0xc0:
            mRegisters.B |= 1 << 0;
            break;

        case 0xc1:
            mRegisters.C |= 1 << 0;
            break;

        case 0xc2:
            mRegisters.D |= 1 << 0;
            break;

        case 0xc3:
            mRegisters.E |= 1 << 0;
            break;

        case 0xc4:
            mRegisters.H |= 1 << 0;
            break;

        case 0xc5:
            mRegisters.L |= 1 << 0;
            break;

        case 0xc6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 0));
            break;

        case 0xc7:
            mRegisters.A |= 1 << 0;
            break;

        case 0xc8:
            mRegisters.B |= 1 << 1;
            break;

        case 0xc9:
            mRegisters.C |= 1 << 1;
            break;

        case 0xca:
            mRegisters.D |= 1 << 1;
            break;

        case 0xcb:
            mRegisters.E |= 1 << 1;
            break;

        case 0xcc:
            mRegisters.H |= 1 << 1;
            break;

        case 0xcd:
            mRegisters.L |= 1 << 1;
            break;

        case 0xce:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 1));
            break;

        case 0xcf:
            mRegisters.A |= 1 << 1;
            break;

        case 0xd0:
            mRegisters.B |= 1 << 2;
            break;

        case 0xd1:
            mRegisters.C |= 1 << 2;
            break;

        case 0xd2:
            mRegisters.D |= 1 << 2;
            break;

        case 0xd3:
            mRegisters.E |= 1 << 2;
            break;

        case 0xd4:
            mRegisters.H |= 1 << 2;
            break;

        case 0xd5:
            mRegisters.L |= 1 << 2;
            break;

        case 0xd6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 2));
            break;

        case 0xd7:
            mRegisters.A |= 1 << 2;
            break;

        case 0xd8:
            mRegisters.B |= 1 << 3;
            break;

        case 0xd9:
            mRegisters.C |= 1 << 3;
            break;

        case 0xda:
            mRegisters.D |= 1 << 3;
            break;

        case 0xdb:
            mRegisters.E |= 1 << 3;
            break;

        case 0xdc:
            mRegisters.H |= 1 << 3;
            break;

        case 0xdd:
            mRegisters.L |= 1 << 3;
            break;

        case 0xde:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 3));
            break;

        case 0xdf:
            mRegisters.A |= 1 << 3;
            break;

        case 0xe0:
            mRegisters.B |= 1 << 4;
            break;

        case 0xe1:
            mRegisters.C |= 1 << 4;
            break;

        case 0xe2:
            mRegisters.D |= 1 << 4;
            break;

        case 0xe3:
            mRegisters.E |= 1 << 4;
            break;

        case 0xe4:
            mRegisters.H |= 1 << 4;
            break;

        case 0xe5:
            mRegisters.L |= 1 << 4;
            break;

        case 0xe6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 4));
            break;

        case 0xe7:
            mRegisters.A |= 1 << 4;
            break;

        case 0xe8:
            mRegisters.B |= 1 << 5;
            break;

        case 0xe9:
            mRegisters.C |= 1 << 5;
            break;

        case 0xea:
            mRegisters.D |= 1 << 5;
            break;

        case 0xeb:
            mRegisters.E |= 1 << 5;
            break;

        case 0xec:
            mRegisters.H |= 1 << 5;
            break;

        case 0xed:
            mRegisters.L |= 1 << 5;
            break;

        case 0xee:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 5));
            break;

        case 0xef:
            mRegisters.A |= 1 << 5;
            break;

        case 0xf0:
            mRegisters.B |= 1 << 6;
            break;

        case 0xf1:
            mRegisters.C |= 1 << 6;
            break;

        case 0xf2:
            mRegisters.D |= 1 << 6;
            break;

        case 0xf3:
            mRegisters.E |= 1 << 6;
            break;

        case 0xf4:
            mRegisters.H |= 1 << 6;
            break;

        case 0xf5:
            mRegisters.L |= 1 << 6;
            break;

        case 0xf6:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 6));
            break;

        case 0xf7:
            mRegisters.A |= 1 << 6;
            break;

        case 0xf8:
            mRegisters.B |= 1 << 7;
            break;

        case 0xf9:
            mRegisters.C |= 1 << 7;
            break;

        case 0xfa:
            mRegisters.D |= 1 << 7;
            break;

        case 0xfb:
            mRegisters.E |= 1 << 7;
            break;

        case 0xfc:
            mRegisters.H |= 1 << 7;
            break;

        case 0xfd:
            mRegisters.L |= 1 << 7;
            break;

        case 0xfe:
            mmu->writeByte(mRegisters.HL, mmu->readByte(mRegisters.HL) | (1 << 7));
            break;

        case 0xff:
            mRegisters.A |= 1 << 7;
            break;

        default:
            std::cout << "unknown CB-prefixed opcode: 0x" << std::hex << (int)opcode << std::endl;
            std::cout << "pc: " << mRegisters.pc << std::endl;
            exit(5);
    }
}