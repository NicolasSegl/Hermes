#pragma once

#include "MemoryChip.h"

const int NUM_OF_RAM_BANKS = 4;

class MBC3 : public MemoryChip
{
private:
    Byte mSelectedROMBank;
    Byte mSelectedRAMBank;

    bool mRAMEnabled;
    Byte* mRAMBanks[NUM_OF_RAM_BANKS];

public:
    MBC3(Byte* memory);

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val);

    // for save file saving/loading
    virtual void saveRAMToFile(std::ofstream& file);
    virtual void setRAMFromFile(std::ifstream& file);
};