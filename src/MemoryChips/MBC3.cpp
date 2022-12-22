#include "MBC3.h"

#include <iostream>

const DoubleByte ROM_BANK_SIZE = 0x4000;
const DoubleByte RAM_BANK_SIZE = 0x2000;

MBC3::MBC3(Byte* memory)
 : MemoryChip(memory)
{ 
    mSelectedROMBank = 1;
    mSelectedRAMBank = 0;

    mRAMEnabled = false;
    
    for (int ramBank = 0; ramBank < 4; ramBank++)
        mRAMBanks[ramBank] = new Byte[RAM_BANK_SIZE];
}

Byte MBC3::readByte(DoubleByte addr)
{
    // if trying to read a ROM memory bank that is NOT bank 0:
    if (addr >= 0x4000 && addr <= 0x7FFF)
        return mROMMemory[(addr - 0x4000) + ROM_BANK_SIZE * mSelectedROMBank];
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            return mRAMBanks[mSelectedRAMBank][addr - 0xA000];
        else
            return 0xFF;
    }

    return mROMMemory[addr];
}

void MBC3::writeByte(DoubleByte addr, Byte val)
{
    if (addr <= 0x1FFF)
    {
        // enable RAM if the first 4 bits of the value being writte equal 0xA. Otherwise, set it to false
        mRAMEnabled = (val & 0xF) == 0xA;
        if (mRAMEnabled)
            std::cout << "enabled ram\n";
        // else
         //    std::cout << "disabled ram\n";
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // only use the first 7 (not all 8) bits of the val to determine which rom bank is being selected
        mSelectedROMBank = val & 0x7F;

        // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
        if (mSelectedROMBank == 0)
            mSelectedROMBank = 1;
    }
    else if (addr >= 0x4000 && addr <= 0x5FFF)
    {
        if (val <= 0x3)
            mSelectedRAMBank = val;
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
        {
            std::cout << "enabled\n";
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
        }
        return;
    }
    else if (addr < 0x8000)
        std::cout << "some other value at addr: " << std::hex << addr << std::endl;
    else
        std::cout << "this should never print lol\n";
}