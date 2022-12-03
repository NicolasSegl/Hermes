#pragma once

#include "Constants.h"

const Byte ZERO_FLAG       = 0x80;
const Byte NEGATIVE_FLAG   = 0x40;
const Byte HALF_CARRY_FLAG = 0x20;
const Byte CARRY_FLAG      = 0x10;

/*
    the registers struct contains represetnations of the gameboy's 7 registers, 
    as well as the program counter, the stack pointer, and flag register

    it cleverly uses unions to unite some of the 8 bit registers into 16 bit registers
    this is because the gameboy's cpu will occasionally want to use 16 bit registers 
    (and does so by combining two 8 bit registers). because the gameboy's cpu formats
    memory as little endian, the variables in the struct's are sorted reversed alphabetically
*/
struct Registers
{
    // union for 8 bit registers A and F, as well as the 16 bit register AF
    union
    {
        struct 
        {
            /* 
                the flag register. on some resources, you may see each flag referred to as Z, N, H, or C
                the leftmost 4 bits being set can indicate:
                Z zero flag (0x80):        set if the last operation produced a result of 0
                N subtraction flag (0x40): set if the last operation was a subtraction
                H half-carry flag (0x20):  set if the result of the last operation caused the lower half of the byte to overflow past 15
                C carry flag (0x10):       set if the last operation produced a result over 255 (for additions) or under (0) for subtractions (so overflow)
            */
            Byte F;

            Byte A;
        };
        DoubleByte AF;
    };

    // union for 8 bit registers B and C, as well as the 16 bit register BC
    union
    {
        struct 
        {
            Byte C, B;
        };
        DoubleByte BC;
    };

    // union for 8 bit registers D and E, as well as the 16 bit register DE
    union
    {
        struct 
        {
            Byte E, D;
        };
        DoubleByte DE;
    };

    // union for 8 bit registers H and L, as well as the 16 bit register HL
    union
    {
        struct 
        {
            Byte L, H;
        };
        DoubleByte HL;
    };

    // program counter
    DoubleByte pc;

    // stack pointer
    DoubleByte sp;

    // function to set all the default values of the registers
    void reset();

    // functions for clearing or setting flags in the F register
    void setFlag(Byte flag);
    void maskFlag(Byte flag);

    // returns true if the flag is set, and false if otherwise
    bool isFlagSet(Byte flag);
};