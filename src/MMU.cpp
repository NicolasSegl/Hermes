#include "MMU.h"

// reads a single bye from memory
Byte MMU::readByte(DoubleByte addr)
{
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