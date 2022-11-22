#include "Registers.h"

void Registers::reset()
{
    // set all the registers, stack pointer, and program counter to 0
    // note that this will also set all the 8 bit registers to 0 as well
    AF = 0;
    BC = 0;
    DE = 0;
    LH = 0;
    pc = 0;
    sp = 0;
}