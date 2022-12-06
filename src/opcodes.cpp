#include <iostream>

#include "CPU.h"


// general function for incrementing a byte (usually an 8-bit register) and checking to see if any flags should be set
Byte CPU::incByte(Byte val)
{
    // if the value in the register we are incrementing's lower bit is already equal to 0xF, then incrementing it will overflow it
    // meaning we'd have to set the half-carry flag, and otherwise we'll have to unset it
    if ((val & 0xF) == 0xF)
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
    mRegisters.maskFlag(NEGATIVE_FLAG);

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
    mRegisters.setFlag(NEGATIVE_FLAG);

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

    // check for overflow of the first 4 bits of the second byte of each value involved in the addition
    if ((a & 0xFFF) + (b & 0xFFF) > 0xFFF)
        mRegisters.setFlag(HALF_CARRY_FLAG);
    else
        mRegisters.maskFlag(HALF_CARRY_FLAG);

    // clear the subtraction flag
    mRegisters.maskFlag(NEGATIVE_FLAG);

    return a + b;
}

// general function for adding two 8-bit registers together and checking for flags
Byte CPU::addB(Byte a, Byte b)
{
    mRegisters.maskFlag(NEGATIVE_FLAG);

    DoubleByte result = a + b;

    // set the carry flag if a and b's addition would cause an overflow
    if (result & 0xFF00) mRegisters.setFlag(CARRY_FLAG);
    else                 mRegisters.maskFlag(CARRY_FLAG);

    // set the half carry flag if there is going to be a carry in the first 4 bits of the byte
    if ((a & 0xF) + (b & 0xF) > 0xF) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                             mRegisters.maskFlag(HALF_CARRY_FLAG);

    // set the zero flag if the result ends up being 0
    if ((result & 0xFF) == 0) mRegisters.setFlag(ZERO_FLAG);
    else            mRegisters.maskFlag(ZERO_FLAG);

    return result & 0xFF;
}

// general function for adding a and b together as well as the carry flag
void CPU::addBC(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG);

    Byte carry = mRegisters.isFlagSet(CARRY_FLAG);
    DoubleByte result = mRegisters.A + val + carry;

    // set the carry flag if a and b's addition would cause an overflow
    if (result & 0xFF00) mRegisters.setFlag(CARRY_FLAG);
    else                 mRegisters.maskFlag(CARRY_FLAG);

    // set the half carry flag if there is going to be a carry in the first 4 bits of the byte
    if (carry + (mRegisters.A & 0xF) + (val & 0xF) > 0xF) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                                  mRegisters.maskFlag(HALF_CARRY_FLAG);

    mRegisters.A += val + carry;

    // set the zero flag if the result ends up being 0
    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for subtracting an 8-bit value from register A and checking for flags
void CPU::sub(Byte val)
{
    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // if the subtraction would result in an overflow
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // if the lower bits of the register would overflow
    if ((val & 0xF) > (mRegisters.A & 0xF)) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                    mRegisters.maskFlag(HALF_CARRY_FLAG);

    // subtract the value from register A
    mRegisters.A -= val;

    // set the zero flag if register A is now zero
    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for subtracting a (value PLUS the carry flag) from register A
void CPU::sbc(Byte val)
{
    val += mRegisters.isFlagSet(CARRY_FLAG);

    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // set the carry flag if val is greater than A (as then subtracting them will cause an overflow)
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // set the zero flag if val is equal to A (as then subtracting them will equal 0)
    if (val == mRegisters.A) mRegisters.setFlag(ZERO_FLAG);
    else                     mRegisters.maskFlag(ZERO_FLAG);

    // if the first 4 bits of val are greater than the first 4 bits of A, set the half carry flag as subtracting them will cause overflow in the first 4 bits
    if ((val & 0xF) > (mRegisters.A & 0xF)) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                    mRegisters.maskFlag(HALF_CARRY_FLAG);

    // subtract the val from A
    mRegisters.A -= val;
}

// general function for comparing register A against a value, and setting various flags based on the comparison
void CPU::cp(Byte val)
{
    // set the negative flag
    mRegisters.setFlag(NEGATIVE_FLAG);

    // set or clear the zero flag
    if (mRegisters.A == val) mRegisters.setFlag(ZERO_FLAG);
    else                     mRegisters.maskFlag(ZERO_FLAG);

    // set or clear the carry flag
    if (val > mRegisters.A) mRegisters.setFlag(CARRY_FLAG);
    else                    mRegisters.maskFlag(CARRY_FLAG);

    // set or clear the half-carry flag
    if ((((mRegisters.A & 0xF) - (val & 0xF)) & 0x10) == 0x10) mRegisters.setFlag(HALF_CARRY_FLAG);
    else                                                       mRegisters.maskFlag(HALF_CARRY_FLAG);
}

// general function for xoring a byte, setting all flags but the zero flag to 0. it only sets the zero flag if the result is zero
void CPU::xorB(Byte val)
{
    mRegisters.maskFlag(CARRY_FLAG | HALF_CARRY_FLAG | NEGATIVE_FLAG);

    if ((mRegisters.A ^ val)) mRegisters.maskFlag(ZERO_FLAG);
    else         mRegisters.setFlag(ZERO_FLAG);

    mRegisters.A ^= val;
}

// general function for bitwise ORing a byte against register A
void CPU::orB(Byte val)
{
    // make all but the zero flags
    mRegisters.maskFlag(NEGATIVE_FLAG | CARRY_FLAG | HALF_CARRY_FLAG);

    mRegisters.A |= val;

    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}

// general function for bitwise ANDing against register A
void CPU::andB(Byte val)
{
    mRegisters.maskFlag(NEGATIVE_FLAG | CARRY_FLAG);
    mRegisters.setFlag(HALF_CARRY_FLAG);

    mRegisters.A &= val;
    
    if (mRegisters.A == 0) mRegisters.setFlag(ZERO_FLAG);
    else                   mRegisters.maskFlag(ZERO_FLAG);
}


// general function for calling the function at addr (and storing the current address to the stack)
void CPU::call(DoubleByte addr)
{
    // push the current address onto the stack
    mRegisters.sp -= 2;
    mmu.writeDoubleByte(mRegisters.sp, mRegisters.pc);

    // set the program counter equal to the address at the start of the subroutine
    mRegisters.pc = addr;
}

// general function for returning from a function call
void CPU::ret()
{
    mRegisters.pc = mmu.readDoubleByte(mRegisters.sp);
    mRegisters.sp += 2;
}

// general function for pushing the value onto the stack
void CPU::pushToStack(DoubleByte val) 
{
    mRegisters.sp -= 2;
    mmu.writeDoubleByte(mRegisters.sp, val);
}

// general function for popping the value from the top of the stack
DoubleByte CPU::popFromStack()
{
    mRegisters.sp += 2;
    return mmu.readDoubleByte(mRegisters.sp - 2);
}

// decode the instruction
// opcode table can be found here: https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
// with lots of information on each opcode here: https://rgbds.gbdev.io/docs/v0.6.0/gbz80.7/
void CPU::handleOpcodes(Byte opcode, DoubleByte operand)
{
    // because it is impossible for C++ to create variables in switch statements, this placeholder Double Byte must be declared outside of it 
    DoubleByte placeHolderW;

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
            mRegisters.maskFlag(ZERO_FLAG);
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

        case 0xF: // opcode 0xF, RLC_A: rotate register A right once, and set the carry flag if there was a wrap
            mRegisters.A = rrc(mRegisters.A);
            mRegisters.maskFlag(ZERO_FLAG);
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
            mRegisters.maskFlag(ZERO_FLAG);
            break;

        case 0x18: // opcode 0x18, JR_N: jump, relative to the current memory address, to the memory address N (which is a signed integer!)
            mRegisters.pc += (Signedbyte)operand;
            break;

        case 0x19: // opcode 0x19, ADD_HL_DE: add register DE to register HL
            mRegisters.HL = addW(mRegisters.HL, mRegisters.DE);
            break;

        case 0x1A: // opcode 0x1A, LD_A_(DE): store the value pointed to by DE into register A
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
            mRegisters.maskFlag(ZERO_FLAG);
            break;
        
        case 0x20: // opcode 0x20: JR_NZ_N: if the last result was not zero, then jump signed N bytes ahead in memory
            if (!mRegisters.isFlagSet(ZERO_FLAG))
            {
                mRegisters.pc += (Signedbyte)operand;
                mTicks += 12;
            }
            else
                mTicks += 8;

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
            placeHolderW = mRegisters.A;

            if (mRegisters.isFlagSet(NEGATIVE_FLAG))
            {
                if (mRegisters.isFlagSet(HALF_CARRY_FLAG))
                    placeHolderW = (placeHolderW - 0x06) & 0xFF;

                if (mRegisters.isFlagSet(CARRY_FLAG))
                    placeHolderW -= 0x60;
            }
            else
            {
                if (mRegisters.isFlagSet(HALF_CARRY_FLAG) || ((placeHolderW & 0xF) > 9))
                    placeHolderW += 0x6;
                
                if (mRegisters.isFlagSet(CARRY_FLAG) || (placeHolderW > 0x9F))
                    placeHolderW += 0x60;
            }

            // set register A to its now altered value
            mRegisters.A = placeHolderW;
            mRegisters.maskFlag(HALF_CARRY_FLAG);

            if (mRegisters.A == 0)
                mRegisters.setFlag(ZERO_FLAG);
            else
                mRegisters.maskFlag(ZERO_FLAG);

            if ((placeHolderW & 0x100) == 0x100)
                mRegisters.setFlag(CARRY_FLAG);

            break;

        case 0x28: // opcode 0x28 JR_Z_N, jump to the relative address of N (which is a signed integer! could mean we jump backwards) if the last operation resulted in a zero
            if (mRegisters.isFlagSet(ZERO_FLAG))
            {
                mRegisters.pc += (Signedbyte)operand;
                mTicks += 12;
            }
            else
                mTicks += 8;

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
            mRegisters.setFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);
            break;

        case 0x30: // opcode 0x30, JR_NC_N: relative jump to signed N if the last instruction resulted in no carry
            if (!mRegisters.isFlagSet(CARRY_FLAG))
            {
                mRegisters.pc += (Signedbyte)operand;
                mTicks += 12;
            }
            else
                mTicks += 8;

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

        case 0x37: // opcode 0x37, SCF: set the carry flag (and clear the negative and half carry flags)
            mRegisters.setFlag(CARRY_FLAG);
            mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);
            break;

        case 0x38: // opcode 0x38, JR_C_N: relative jump by signed N, if the last result resulted in the carry flag being set
            if (mRegisters.isFlagSet(CARRY_FLAG))
            {
                mRegisters.pc += (Signedbyte)operand;
                mTicks += 12;
            }
            else
                mTicks += 8;

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
        
        case 0x3F: // opcode 0x3F, CCF: flip the carry flag and clear the negative and half carry flags
            if (mRegisters.isFlagSet(CARRY_FLAG))
                mRegisters.maskFlag(CARRY_FLAG);
            else
                mRegisters.setFlag(CARRY_FLAG);

            mRegisters.maskFlag(NEGATIVE_FLAG | HALF_CARRY_FLAG);
            break;

        case 0x40: // opcode 0x40, LD_B_B: load the value of B into B (?)
            break;

        case 0x46: // opcode 0x46, LD_B_(HL); load the value in memory pointed to by HL into register B
            mRegisters.B = mmu.readByte(mRegisters.HL);
            break;

        case 0x47: // opcode 0x47, LD_B_A: load the value of register A into register B
            mRegisters.B = mRegisters.A;
            break;

        case 0x4E: // opcode 0x4E, LD_C_(HL): load the value in memory pointed to by HL into register C
            mRegisters.C = mmu.readByte(mRegisters.HL);
            break;

        case 0x4F: // opcode 0x4F, LD_C_A: load the value of register A into register C
            mRegisters.C = mRegisters.A;
            break;

        case 0x54: // opcode 0x54, LD_D_H: load the value of register H into register D
            mRegisters.D = mRegisters.H;
            break;

        case 0x56: // opcode 0x56, LD_D_(HL): load the value stored at the address pointed to by HL into register D
            mRegisters.D = mmu.readByte(mRegisters.HL);
            break;

        case 0x57: // opcode 0x57, LD_D_A: load the value of register A into register D
            mRegisters.D = mRegisters.A;
            break;

        case 0x5A: // opcode 0x5A, LD_E_D: load the value of register D into register E
            mRegisters.E = mRegisters.D;
            break;

        case 0x5D: // opcode 0x5D, LD_E_L: load the value of L into register E
            mRegisters.E = mRegisters.L;
            break;

        case 0x5E: // opcode 0x5E, LD_E_(HL): load the value stored at the address pointed to by HL into register A
            mRegisters.E = mmu.readByte(mRegisters.HL);
            break;

        case 0x5F: // opcode 0x5F, LD_E_A: store the value of register A into register E
            mRegisters.E = mRegisters.A;
            break;

        case 0x60: // opcode 0x60, LD_H_B: load the value of B into register H
            mRegisters.H = mRegisters.B;
            break;

        case 0x62: // opcode 0x62, LD_H_D: load the value of register D into register H
            mRegisters.H = mRegisters.D;
            break;

        case 0x65: // opcode 0x65, LD_H_L: load the value of register L into register H
            mRegisters.H = mRegisters.L;
            break;

        case 0x67: // opcode 0x67, LD_H_A: load the value of register A into register H
            mRegisters.H = mRegisters.A;
            break;

        case 0x69: // opcode 0x69, LD_L_C: load the value of register C into register L
            mRegisters.L = mRegisters.C;
            break;

        case 0x6B: // opcode 0x6B, LD_L_E: load the value of register E into register L
            mRegisters.L = mRegisters.E;
            break;

        case 0x6E: // opcode 0x6E, LD_L_(HL): load the value pointed to in memory by HL into L
            mRegisters.L = mmu.readByte(mRegisters.HL);
            break;

        case 0x6F: // opcode 0x6F, LD_L_A: load the value of register A into register L
            mRegisters.L = mRegisters.A;
            break;

        case 0x70: // opcode 0x70, LD_(HL)_B: store the value of register B into the memory address pointed to by HL
            mmu.writeByte(mRegisters.HL, mRegisters.B);
            break;

        case 0x71: // opcode 0x71, LD_(HL)_C: store the value of register C into the memory address pointed to by HL
            mmu.writeByte(mRegisters.HL, mRegisters.C);
            break;

        case 0x72: // opcode 0x72, LD_(HL)_D: store the value of register D into the memory address pointed to by HL
            mmu.writeByte(mRegisters.HL, mRegisters.D);
            break;

        case 0x73: // opcode 0x73, LD_(HL)_E: load the value of register E into the memory address pointed to by register HL
            mmu.writeByte(mRegisters.HL, mRegisters.E);
            break;

        case 0x77: // opcode 0x77 LD_(HL)_A: store the value of register A into the memory address pointed to by register HL
            mmu.writeByte(mRegisters.HL, mRegisters.A);
            break;

        case 0x78: // opcode 0x78, LD_A_B: store the value of register B into register A
            mRegisters.A = mRegisters.B;
            break;

        case 0x79: // opcode 0x79, LD_A_C: load the value of register C into register A
            mRegisters.A = mRegisters.C;
            break;

        case 0x7A: // opcode 0x7A, LD_A_D: load the value of register D into register A
            mRegisters.A = mRegisters.D;
            break;

        case 0x7B: // opcode 0x7B, LD_A_E: load the value of register E into register A
            mRegisters.A = mRegisters.E;
            break;

        case 0x7C: // opcode 0x7C, LD_A_H: load the value of register H into register A
            mRegisters.A = mRegisters.H;
            break;

        case 0x7D: // opcode 0x7D, LD_A_L: load the value of register L into register A
            mRegisters.A = mRegisters.L;
            break;

        case 0x7E: // opcode 0x7E, LD_A_(HL): load the value pointed to in memory by HL into register A
            mRegisters.A = mmu.readByte(mRegisters.HL);
            break;

        case 0x7F: // opcode 0x7F, LD_A_A: load A into A (do nothing)
            break;

        case 0x80: // opcode 0x80, ADD_B_A: add register B to register A
            mRegisters.A = addB(mRegisters.A, mRegisters.B);
            break;

        case 0x84: // opcode 0x84, ADD_A_L: add L to A
            mRegisters.A = addB(mRegisters.A, mRegisters.L);
            break;

        case 0x85: // opcode 0x85, ADD_A_L: add register L to register A
            mRegisters.A = addB(mRegisters.A, mRegisters.L);
            break;

        case 0x86: // opcode 0x86, ADD_A_(HL): add the value stored in Register A to the value pointed to by HL
            mRegisters.A = addB(mRegisters.A, mmu.readByte(mRegisters.HL));
            break;

        case 0x87: // opcode 0x87, ADD_A_A: add the value of register A to register A
            mRegisters.A = addB(mRegisters.A, mRegisters.A);
            break;

        case 0x89: // opcode 0x89, ADC_A_C: add register C and the carry flag to register A
            addBC(mRegisters.C);
            break;

        case 0x8E: // opcode 0x8E, ADC_A_(HL) add the value pointed to in memory by HL and the carry flag to register A
            addBC(mmu.readByte(mRegisters.HL));
            break;

        case 0x90: // opcode 0x90, SUB_A_B: subtract the value of register B from register A
            sub(mRegisters.B);
            break;

        case 0x91: // opcode 0x91, SUB_A_C: subtract C from A
            sub(mRegisters.C);
            break;

        case 0x95: // opcode 0x95, LD_E_C: load the value of C into E
            mRegisters.E = mRegisters.C;
            break;

        case 0x99: // opcode 0x99, SBC_A_C: subtract register C and the carry from register A
            sbc(mRegisters.C);
            break;

        case 0x9A: // opcode 0x9A, SBC_A_D: subtract D and the carry flag from register A
            sbc(mRegisters.D);
            break;

        case 0x9E: // opcode 0x9E, SBC_A_(HL): subtract the value pointed to be HL and the carry flag from register A
            sbc(mmu.readByte(mRegisters.HL));
            break;

        case 0x9F: // opcode 0x9F, SBC_A_A: subtract A, the carry flag, and A
            sbc(mRegisters.A);
            break;

        case 0xA1: // opcode 0xA1, AND_C: bitwise AND C against A
            andB(mRegisters.C);
            break;

        case 0xA7: // opcode 0xA7, AND_A: bitwise AND A against A
            andB(mRegisters.A);
            break;

        case 0xA9: // opcode 0xA9, XOR_C: bitwise XOR C against A
            xorB(mRegisters.C);
            break;

        case 0xAD: // opcode 0xAD, XOR_L: bitwise XOR L against A
            xorB(mRegisters.L);
            break;

        case 0xAE: // opcode 0xAE, XOR_(HL): bitwise XOR the byte in memory pointed to by HL against A
            xorB(mmu.readByte(mRegisters.HL));
            break;

        case 0xAF: // opcode 0xAF, XOR_A: bitwise XOR A against A
            xorB(mRegisters.A);
            break;
        
        case 0xB0: // opcode 0xB0, OR_B: bitwise OR B against A
            orB(mRegisters.B);
            break;

        case 0xB1: // opcode 0xB1, OR_C: bitwise OR C against A
            orB(mRegisters.C);
            break;

        case 0xB3: // opcode 0xB3, OR_E: bitwise OR E against A
            orB(mRegisters.E);
            break;

        case 0xB6: // opcode 0xB6, OR_(HL): bitwise the byte pointed to in memory by HL against A
            orB(mmu.readByte(mRegisters.HL));
            break;

        case 0xB7: // opcode 0xB7, OR_A: bitwise or A against A
            orB(mRegisters.A);
            break;

        case 0xB8: // opcode 0xB8, CP_B: compare B against A
            cp(mRegisters.B);
            break;

        case 0xB9: // opcode 0xB9, CP_C: compare C against A
            cp(mRegisters.C);
            break;

        case 0xBA: // opcode 0xBA, CP_D: compare D against A
            cp(mRegisters.D);
            break;

        case 0xBB: // opcode 0xBB, CP_E: compare E against A
            cp(mRegisters.E);
            break;

        case 0xBE: // opcode 0xBE, CP_(HL)_A: compare register A and the value pointed to by HL
            cp(mmu.readByte(mRegisters.HL));
            break;

        case 0xC0: // opcode 0xC0, RET_NZ: return if the last result was not 0
            if (!mRegisters.isFlagSet(ZERO_FLAG))
            {
                ret();
                mTicks += 20;
            }
            else
                mTicks += 8;

            break;

        case 0xC1: // opcode 0xC1, POP_BC: pop the value from the stack and put it onto register BC
            mRegisters.BC = popFromStack();
            break;

        case 0xC2: // opcode 0xC2, JP_NZ_NN: jump the the address NN if the last result was not zero
            if (!mRegisters.isFlagSet(ZERO_FLAG))
            {
                mRegisters.pc = operand;
                mTicks += 16;
            }
            else
                mTicks += 12;
            
            break;

        case 0xC3: // opcode 0xC3, JP_NN: jump to the address NN
            mRegisters.pc = operand;
            break;

        case 0xC4: // opcode 0xC4, CALL_NZ_NN: call the subroutine at NN if the last result was not zero
            if (!mRegisters.isFlagSet(ZERO_FLAG))
            {
                call(operand);
                mTicks += 24;
            }
            else
                mTicks += 12;

            break;

        case 0xC5: // opcode 0xC5, PUSH_BC: push the value of register BC onto the stack
            pushToStack(mRegisters.BC);
            break;

        case 0xC6: // opcode 0xC6, ADD_A_N: add N to A
            mRegisters.A = addB(mRegisters.A, (Byte)operand);
            break;

        case 0xC7: // opcode 0xC7, RST_0: call the subroutine at 0x0000
            call(0x0000);
            break;

        case 0xC8: // opcode 0xC8, RET_Z: return if the last result was zero
            if (mRegisters.isFlagSet(ZERO_FLAG))
            {
                ret();
                mTicks += 20;
            }
            else
                mTicks += 8;
            
            break;

        case 0xC9: // opcode 0xC9, RET: return to calling routine
            ret();
            break;

        case 0xCA: // opcode 0xCA, JP_Z_NN: if the last result was zero, jump to the address NN
            if (mRegisters.isFlagSet(ZERO_FLAG))
            {
                mRegisters.pc = operand;
                mTicks += 16;
            }
            else
                mTicks += 12;

            break;

        case 0xCB: // opcode 0xCB: an opcode of 0xCB means that we will index the extended opcodes table by the operand
            handleCBOpcodes((Byte)operand);
            break;

        case 0xCC: // opcode 0xCC, CALL_Z_NN: call the function at NN if the last operation resulted in a zero
            if (mRegisters.isFlagSet(ZERO_FLAG))
            {
                call(operand);
                mTicks += 24;
            }
            else
                mTicks += 12;

            break;

        case 0xCD: // opcode 0xCD, CALL_NN: call the subroutine at NN
            call(operand);
            break;

        case 0xCE: // opcode 0xCE, ADC_A_N: add N and the carry to A
            addBC((Byte)operand);
            break;

        case 0xCF: // opcode 0xCF, RST_8: call the subroutine at 0x0008
            call(0x0008);
            break;

        case 0xD0: // opcode 0xD0, RET_NC: return if the last result resulted in no carry
            if (!mRegisters.isFlagSet(CARRY_FLAG))
            {
                mTicks += 20;
                ret();
            }
            else
                mTicks += 8;

            break;

        case 0xD1: // opcode 0xD1, POP_DE: pop the value off the stack and store it into DE
            mRegisters.DE = popFromStack();
            break;

        case 0xD2: // opcode 0xD2, JP_NC_NN: jump to NN if the last operation resulted in no carry
            if (!mRegisters.isFlagSet(CARRY_FLAG))
            {
                mTicks += 16;
                mRegisters.pc = operand;
            }
            else
                mTicks += 12;

            break;

        case 0xD4: // opcode 0xD4, CALL_NC_NN: call the subroutine at NN if the last operation resulted in no carry
            if (!mRegisters.isFlagSet(CARRY_FLAG))
            {
                call(operand);
                mTicks += 24;
            }
            else
                mTicks += 12;

            break;
            
        case 0xD5: // opcode 0xD5, PUSH_DE: push the value stored at address DE onto the stack
            pushToStack(mRegisters.DE);
            break;

        case 0xD6: // opcode 0xD6, SUB_A_N: subtract N from A
            sub((Byte)operand);
            break;

        case 0xD7: // opcode 0xD7, RST_10: call the subroutine at 0x0010
            call(0x0010);
            break;

        case 0xD8: // opcode 0xD8, RET_C: return if the last operation resulted in a carry
            if (mRegisters.isFlagSet(CARRY_FLAG))
            {
                ret();
                mTicks += 20;
            }
            else
                mTicks += 8;

            break;

        case 0xD9: // opcode 0xD9, RETI: return to calling routine and enable interrupts
            ret();
            mInterruptHandler.enableInterrupts();
            break;

        case 0xDA: // opcode 0xDA, JP_C_NN: jump to NN if the last operation resulted in a carry
            if (mRegisters.isFlagSet(CARRY_FLAG))
            {
                mTicks += 16;
                mRegisters.pc = operand;
            }
            else
                mTicks += 12;

            break;
            
        case 0xDC: // opcode 0xDC, CALL_C_NN: call subroutine at NN if the last operation resulted in a carry
            if (mRegisters.isFlagSet(CARRY_FLAG))
            {
                call(operand);
                mTicks += 24;
            }
            else
                mTicks += 12;

            break;

        case 0xDF: // opcode 0xDF, RST18: run the subroutine at 0x18
            call(0x18);
            break;

        case 0xE0: // opcode 0xE0, LDH_N_A: save register A into the memory address pointed to by N + 0xFF00
            mmu.writeByte((Byte)operand + 0xFF00, mRegisters.A);
            break;

        case 0xE1: // opcode 0xE1, POP_HL: pop a value from the stack and store it into register HL
            mRegisters.HL = popFromStack();
            break;

        case 0xE2: // opcode 0xE2, LDH_C_A: save register A into the memory address pointed to by register C + 0xFF00
            mmu.writeByte(mRegisters.C + 0xFF00, mRegisters.A);
            break;

        case 0xE3: // opcode 0xE3, NO INSTRUCTION
            break;

        case 0xE5: // opcode 0xE5, PUSH_HL: push the value of HL onto the stack
            pushToStack(mRegisters.HL);
            break;

        case 0xE6: // opcode 0xE6, AND_N: bitwise AND N against register A (and store the result in register A)
            andB((Byte)operand);
            break;

        case 0xE7: // opcode 0xE7, RST_20: call the subroutine at 0x0020
            call(0x0020);
            break;

        case 0xE9: // opcode 0xE9, JP_(HL): jump to the address stored in the register HL. but many others have this as jumping to the address stored in the register HL?
            mRegisters.pc = mRegisters.HL; // mmu.readDoubleByte(mRegisters.HL);
            break;

        case 0xEA: // opcode 0xEA, LD_NN_A: store the value of register A into memory address NN
            mmu.writeByte(operand, mRegisters.A);
            break;

        case 0xEE: // opcode 0xEE, XOR_N: bitwise XOR N against A
            xorB((Byte)operand);
            break;

        case 0xEF: // opcode 0xEF, RST_28: call the subroutine at 0x0028
            call(0x28);
            break;

        case 0xF0: // opcode 0xF0, LDH_A_N: store the value of memory address N + 0xFF00 into register A
            mRegisters.A = mmu.readByte((Byte)operand + 0xFF00);
            break;
        
        case 0xF1: // opcode 0xF1, POP_AF: pop the value off the stack and store it into AF
            mRegisters.AF = popFromStack();

            // in case the flag register had its 4 lower bits set
            mRegisters.F &= ~0xF;
            break;

        case 0xF3: // opcode 0xF3, DI: disable interrupts
            mInterruptHandler.disableInterrupts();
            break;

        case 0xF5: // opcode 0xF5, PUSH_AF: push the value of register AF onto the stack
            pushToStack(mRegisters.AF);
            break;

        case 0xF6: // opcode 0xF6, OR_N: bitwise N against A
            orB((Byte)operand);
            break;

        case 0xF7: // opcode 0xF7, RST_30: call the subroutine at 0x0030
            call(0x0030);
            break;

        case 0xF9: // opcode 0xF9, LD_SP_HL: set the stack pointer to HL
            mRegisters.sp = mRegisters.HL;
            break;

        case 0xFA: // opcode 0xFA, LD_A_NN: load register A with the value pointed to in memory by NN
            mRegisters.A = mmu.readByte(operand);
            break;

        case 0xFB: // opcode 0xFB, EI: enalbe interrupts
            mInterruptHandler.enableInterrupts();
            break;

        case 0xFC: // opcode 0xFC: NO INSTRCTION
            break;

        case 0xFE: // opcode 0xFE, CP_N: compare the value of register A against N
            cp((Byte)operand);
            break;

        case 0xFF: // opcode 0xFF, RST_38: call the subroutine at 0x38 
            call(0x38);
            break;

        default: 
            std::cout << "unknown opcode: 0x" << std::hex << (int)opcode << std::endl;
            std::cout << "pc: " << mRegisters.pc << std::endl;
            exit(5);
    }
}