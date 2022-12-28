#pragma once

#include "MBC.h"

class MBC3 : public MBC
{
public:
    MBC3(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks) : MBC(memory, numOfRomBanks, numOfRamBanks) {}

    virtual void writeByte(DoubleByte addr, Byte val);
};