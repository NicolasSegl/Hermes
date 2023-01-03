#include "MBC2.h"

void MBC2::writeByte(DoubleByte addr, Byte val)
{
    // the 0x0000-0x3FFF address range is used both for enabling RAM and setting the ROM bank number
    // the 8th bit (so the first bit/least significant bit) of the address determines whether or not the
    // write is changing the RAM enable or the ROM bank number
    if (addr >= 0x0000 && addr <= 0x3FFF)
    {
        if (addr & 0x100)
        {
            mSelectedROMBank = val & 0xF;

            if (mSelectedROMBank == 0)
                mSelectedROMBank++;
        }
        else // if the 8th bit is clear then we are enabling/disabling RAM
            mRAMEnabled = (val & 0xF) == 0xA;
    }
}

Byte MBC2::readByte(DoubleByte addr)
{
    // if trying to read a ROM memory bank that is NOT bank 0:
    if (addr >= 0x4000 && addr <= 0x7FFF)
        return mROMMemory[(addr - 0x4000) + ROM_BANK_SIZE * mSelectedROMBank];
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            // the selected RAM bank is always 0, and only the first 512 bytes are used
            // the RAM memory "echoes" (repeats) all the way until 0xBFFF, so using 
            // modulu is appropriate for our case, as any value exceeding 0xA1FF
            // will simply loop back around
            return mRAMBanks[0][addr % 0x1FF];
        else
            return 0xFF;
    }

    return mROMMemory[addr];
}