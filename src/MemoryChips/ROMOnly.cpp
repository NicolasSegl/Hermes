#include "ROMOnly.h"

#include "../Display.h"

Byte ROMOnly::readByte(DoubleByte addr)
{
    return mROMMemory[addr];
}

void ROMOnly::writeByte(DoubleByte addr, Byte val)
{   
    // unless we are allowing ROM bank switching, do not allow the actual ROM (read only memory) to be changed!
    if (addr < 0x8000)
        return;

    mROMMemory[addr] = val;
}