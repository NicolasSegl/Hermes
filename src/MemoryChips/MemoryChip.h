#pragma once

#include <fstream>

#include "../Constants.h"

class MemoryChip
{
protected:
    // offsets
    const DoubleByte SPRITE_DATA_OFFSET  = 0xFE00;
    const DoubleByte OAM_DMA_OFFSET      = 0xFF46;
    const DoubleByte JOYPAD_OFFSET       = 0xFF00;

    // timer offsets
    const DoubleByte DIV_REGISTER_OFFSET  = 0xFF04;

    // palette offsets
    const DoubleByte BG_PALETTE_OFFSET = 0xFF47;
    const DoubleByte S0_PALETTE_OFFSET = 0xFF48;
    const DoubleByte S1_PALETTE_OFFSET = 0xFF49;

    Byte* mROMMemory;

public:
    MemoryChip(Byte* romMemory);

    virtual Byte readByte(DoubleByte addr)            = 0;
    virtual void writeByte(DoubleByte addr, Byte val) = 0;
    virtual void saveRAMToFile(std::ofstream& file)   = 0;
};