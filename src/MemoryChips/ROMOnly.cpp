#include "ROMOnly.h"

ROMOnly::ROMOnly(Byte* romMemory, Byte numOfRamBanks)
 : MemoryChip(romMemory)
{ 
    mSupportsRam = numOfRamBanks;

    if (mSupportsRam)
        mRamMemory = new Byte[RAM_BANK_SIZE];
}

Byte ROMOnly::readByte(DoubleByte addr)
{
    return mROMMemory[addr];
}

void ROMOnly::writeByte(DoubleByte addr, Byte val)
{   
    // unless we are allowing ROM bank switching, do not allow the actual ROM (read only memory) to be changed!
    if (addr < 0x8000)
        return;

    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mSupportsRam)
            mRamMemory[addr - 0xA000] = val;

        return;
    }
    else
        mROMMemory[addr] = val;
}