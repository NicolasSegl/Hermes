#include "MMU.h"

// reads a single bye from memory
Byte MMU::readByte(DoubleByte addr)
{
    // // deternmine from which part of the memory map the byte is being read from
    // switch (addr & 0xF000)
    // {
    //     // BIOS: the first 256b (0x100) 
    //     case 0x0000:
    //     {
    //         // if the bios is not finished booting yet, then return the instructions from the BIOS (and not the ROM)
    //         if (!biosFinished)
    //         {
    //             // if the address is still in the region of the bios
    //             if (addr < 0x100)
    //                 return bios[addr];
    //             else
    //                 biosFinished = true;
    //         }
            
    //         return bios[addr];
    //     }

    //     // Rom Bank 0: from 0x0150 - 0x3FFF
    //     case 0x1000:
    //     case 0x2000:
    //     case 0x3000:
    //         return romBank0[addr];

    //     // Rom Bank 1: from 0x4000 - 0x7FFF
    //     case 0x4000:
    //     case 0x5000:
    //     case 0x6000:
    //     case 0x7000:
    //         return romBank1[addr];

    //     // Graphics (VRAM): from 0x8000 - 0xA000
    //     case 0x8000:
    //     case 0x9000:
    //         return virtualRam[addr];

    //     // External RAM: from 0xA000 - 0xBFFF
    //     case 0xA000:
    //     case 0xB000:
    //         return externalRam[addr];

    //     // Working RAM: from 0xC000 - 0xDFFF
    //     case 0xC000:
    //     case 0xD000:
    //         return workingRam[addr];

    //     // Working Ram Shadow: 
    // }

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
    biosFinished = false;
}