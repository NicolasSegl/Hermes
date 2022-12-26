#pragma once

#include <cstdint>
#include <fstream>

#include "Constants.h"
#include "MemoryChips/MemoryChip.h"

const DoubleByte RAM_MEMORY_SIZE = 0x8000;

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
    Byte ramMemory[RAM_MEMORY_SIZE];

    MemoryChip* memoryChip;

    void init(uint64_t* ticks);

    Byte readByte(DoubleByte addr);
    DoubleByte readDoubleByte(DoubleByte addr);

    void writeByte(DoubleByte addr, Byte val);
    void writeDoubleByte(DoubleByte addr, DoubleByte val);

    void saveRAMToFile(std::ofstream& file);
    void setRAMFromFile(std::ifstream& file);
};