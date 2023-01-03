#pragma once

#include "MBC.h"

class MBC2 : public MBC
{
public:
    MBC2(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks) : MBC(memory, numOfRomBanks, numOfRamBanks) {}

    virtual void writeByte(DoubleByte addr, Byte val);
    virtual Byte readByte(DoubleByte addr);
};