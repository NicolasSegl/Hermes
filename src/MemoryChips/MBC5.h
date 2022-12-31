#pragma once

#include "MBC.h"

class MBC5 : public MBC
{
public:
    MBC5(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks) : MBC(memory, numOfRomBanks, numOfRamBanks) {}

    virtual void writeByte(DoubleByte addr, Byte val);
};