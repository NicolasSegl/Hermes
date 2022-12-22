#pragma once

#include <cstdint>

#include "Constants.h"
#include "MemoryChips/MemoryChip.h"

/* 
    the memory management unit (MMU) struct is responsible for handling all the memory of the cartridge
    it uses a union so that we can reference the memory using different names for convienience
*/
struct MMU
{
private:
    uint64_t* mTicks;

public:
    Byte* romMemory;

    // this includes vram, hram, i/o registers, etc. just any memory that is not related to the ROM memory 
    Byte ramMemory[0x8000];

    MemoryChip* memoryChip;

    void init(uint64_t* ticks);

    Byte readByte(DoubleByte addr);
    DoubleByte readDoubleByte(DoubleByte addr);

    void writeByte(DoubleByte addr, Byte val);
    void writeDoubleByte(DoubleByte addr, DoubleByte val);
};

// the bios is loaded into the cartridge's memory. place is already allocated for it in Rom Bank 0s