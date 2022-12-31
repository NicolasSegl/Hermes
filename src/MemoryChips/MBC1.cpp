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

        // if we are in ROM mode, then we want to use the extra 2 bits supplied by the program when it writes to addresses 0x6000-0x7FFF
        if (mMemoryMode == MEMORY_MODE::ROM_MODE)
            // shift the upper 2 bits left 5 times because the first 5 bits are occupied by val (the value just written to 0x2000-0x3FFF)
            mSelectedROMBank |= (mUpperRomBankBits << 5);

        // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
        if (mSelectedROMBank == 0 || mSelectedROMBank == 0x20 || mSelectedROMBank == 0x40 || mSelectedROMBank == 0x60)
            mSelectedROMBank++;
    }
    else if (addr >= 0x4000 && addr <= 0x5FFF)
    {
        if (mMemoryMode == MEMORY_MODE::RAM_MODE)
            mSelectedRAMBank = val & 0b11;
       else
            mUpperRomBankBits = val & 0b11;
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF) // this address is written to when switching the rom/ram mode
    {
        // we are only concerned with the first 2 bits of the value
        // value of 1 is written when selecting RAM mode
        if (val == 1)
            mMemoryMode = MEMORY_MODE::RAM_MODE;
        else // a value of zero for ROM mode
        {
            // can only use RAM bank 0 when in ROM mode
            mSelectedRAMBank = 0;

            mMemoryMode = MEMORY_MODE::ROM_MODE;
        }
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
    }
}