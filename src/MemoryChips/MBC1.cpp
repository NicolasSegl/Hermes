#include "MBC1.h"

void MBC1::writeByte(DoubleByte addr, Byte val)
{
    if (addr <= 0x1FFF)
        // enable RAM if the first 4 bits of the value being writte equal 0xA. Otherwise, set it to false
        mRAMEnabled = (val & 0xF) == 0xA;
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // only use the first 5 (not all 8) bits of the val to determine which rom bank is being selected
        mSelectedROMBank = val & 0x1F;

        // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
        if (mSelectedROMBank == 0 || mSelectedROMBank == 0x20 || mSelectedROMBank == 0x40 || mSelectedROMBank == 0x60)
            mSelectedROMBank++;
    }
    else if (addr >= 0x4000 && addr <= 0x5FFF)
    {
        if (val <= 0x3)
            mSelectedRAMBank = val;
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
    }
}