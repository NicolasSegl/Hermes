#pragma once

#include "MBC.h"

class MBC1 : public MBC
{
public:
    MBC1(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks) : MBC(memory, numOfRomBanks, numOfRamBanks) {}

    virtual void writeByte(DoubleByte addr, Byte val);
};