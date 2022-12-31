#include "MBC5.h"

void MBC5::writeByte(DoubleByte addr, Byte val)
{
    if (addr <= 0x1FFF)
        // enable RAM if the first 4 bits of the value being writte equal 0xA. Otherwise, set it to false
        mRAMEnabled = (val & 0xF) == 0xA;
    else if (addr >= 0x2000 && addr <= 0x2FFF)
        // set the 8 least significant bits of the selected ROM bank
        mSelectedROMBank = (mSelectedROMBank & 0xFF00) | val;
    else if (addr >= 0x3000 && addr <= 0x3FFF)
        // use all 8 bits of the value passed in 
        mSelectedROMBank = (mSelectedRAMBank & 0xFF) | ((val & 1) << 8);
    else if (addr >= 0x4000 && addr <= 0x5FFF)
        mSelectedRAMBank = val & 0xF;
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
    }
}