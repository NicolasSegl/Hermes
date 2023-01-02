#pragma once

#include "MBC.h"

class MBC1 : public MBC
{
private:
    /*
        the MBC1 can select either ROM mode, allowing it to access an extra 2 bits for ROM bank selection,
        or it can select RAM mode, allowing it to access up to 4 RAM banks
    */
    enum class MEMORY_MODE 
    {
        ROM_MODE = 0,
        RAM_MODE = 1,
    };

    MEMORY_MODE mMemoryMode;
    Byte mUpperRomBankBits;

public:
    MBC1(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks);

    virtual void writeByte(DoubleByte addr, Byte val);
    virtual void saveRAMToFile(std::ofstream& file);
    virtual void setRAMFromFile(std::ifstream& file);
};