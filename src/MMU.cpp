#include "Display.h"
#include "MMU.h"

#include <iostream>

// offsets
const DoubleByte SPRITE_DATA_OFFSET = 0xFE00;
const DoubleByte JOYPAD_OFFSET  = 0xFF00;
const DoubleByte OAM_DMA_OFFSET = 0xFF46;

// pallete offsets
const DoubleByte BG_PALETTE_OFFSET = 0xFF47;
const DoubleByte S0_PALETTE_OFFSET = 0xFF48;
const DoubleByte S1_PALETTE_OFFSET = 0xFF49;

// reads a single bye from memory
// depending on what is trying to be read from memory, we may have to 
// do something particular (such as for input)
Byte MMU::readByte(DoubleByte addr)
{
    if (addr == JOYPAD_OFFSET)
    {
        // using cinoop as a reference
        // store the currently pressed keys in a vairable in the MMU (as are declared in MMU.h)
        // then return a byte of all 1s save for whichever of the 4th or 5th bit is not set,
        // as well as any keys that are pressed (which will be set to 0)

        // if both the 5th and 4th bit are set (i.e. it's trying to look at both the directioanl buttons and regular button)
        //if (memory[addr] & 0x30) return 0xFF;
        if (memory[addr] & 0x10) // if the 4th bit is set (looking for directional buttons)
        {
            //return (0xC0 | 0xFF | 0x10) & ~(1 << 0);
        }
        else if (memory[addr] & 0x20)
        {
           // std::cout << ((0xFF | 0xC0 | 0x20) & ~(1 << 3)) << '\n';
            //return (0xFF | 0xC0 | 0x20) & ~(1 << 3);
        }
        return 0xFF;
    }

    //if (addr == 0xff44)
    //    return 0x90;
    
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
        // increase ticks by 160?
        // loop through all the bytes in the OAM
        for (int byte = 0; byte < 0xA0; byte++)
            writeByte(SPRITE_DATA_OFFSET + byte, readByte(((val << 8) + byte)));
    }

    else if (addr == BG_PALETTE_OFFSET)
        Display::updateBackgroundPallete(val);

    else if (addr == S0_PALETTE_OFFSET)
        Display::updateSpritePallete0(val);

    else if (addr == S1_PALETTE_OFFSET)
        Display::updateSpritePallete1(val);

    // unless we are allowing ROM bank switching, do not allow the actual ROM (read only memory) to be changed!
    else if (addr < 0x8000)
        return;

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

    memory[0xFF00] |= 0xFF;
}