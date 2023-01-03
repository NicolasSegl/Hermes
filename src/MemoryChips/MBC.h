#pragma once

#include "MemoryChip.h"

// constants
const DoubleByte ROM_BANK_SIZE = 0x4000;

// the offset at which the data for memory bank controllers in the emulator's customly encoded save files starts
const DoubleByte SAVE_FILE_MBC_OFFSET = 0x800D;

class MBC : public MemoryChip
{
protected:
    DoubleByte mSelectedROMBank;
    Byte mSelectedRAMBank;
    DoubleByte mNumOfRomBanks;
    DoubleByte mNumOfRamBanks;

    bool mRAMEnabled;
    Byte** mRAMBanks;

public:
    MBC(Byte* memory, DoubleByte numOfRomBanks, DoubleByte numOfRamBanks);

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val) = 0;

    // for save file saving/loading
    virtual void saveRAMToFile(std::ofstream& file);
    virtual void setRAMFromFile(std::ifstream& file);
};