#include "MMU.h"

#include <iostream>

// reads a single bye from memory
Byte MMU::readByte(DoubleByte addr)
{
    if (addr == 0xFF00)
    {
        memory[addr] |= 0xF;
        std::cout << (int)memory[addr] << std::endl;
    }

    return memory[addr];
}

// reads a double byte from memory (little endian)
DoubleByte MMU::readDoubleByte(DoubleByte addr)
{
    return ((DoubleByte)(readByte(addr + 1)) << 8) | readByte(addr);
}

// writes a byte to memory
void MMU::writeByte(DoubleByte addr, Byte val)
{   
    // writing to the address 0xFF46 means that the program wants to DMA into the OAM 
    if (addr == OAM_DMA_OFFSET)
    {
        // loop through all the bytes in the OAM
        for (int byte = 0; byte < 0xA; byte++)
            writeByte(SPRITE_DATA_OFFSET + byte, readByte((val << 8) + byte));
    }

    memory[addr] = val;
}

// writes a double byte to memory (little endian)
void MMU::writeDoubleByte(DoubleByte addr, DoubleByte val)
{
    memory[addr]     = val & 0xFF;
    memory[addr + 1] = (val & 0xFF00) >> 8;
}

// initialize some default values for the memory management unit
void MMU::init()
{
    // initialize all of the bytes in memory to 0
    for (int byte = 0; byte < 0x10000; byte++)
        memory[byte] = 0;
}