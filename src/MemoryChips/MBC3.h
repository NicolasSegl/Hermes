#pragma once

#include "MemoryChip.h"

class MBC3 : public MemoryChip
{
private:
    Byte mSelectedROMBank;
    Byte mSelectedRAMBank;

    bool mRAMEnabled;
    Byte* mRAMBanks[4];

public:
    MBC3(Byte* memory);

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val);
};