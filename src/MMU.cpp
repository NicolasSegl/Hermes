#include "Display.h"
#include "MMU.h"

#include <iostream>

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

// reads a single bye from memory
// depending on what is trying to be read from memory, we may have to 
// do something particular (such as for input)
Byte MMU::readByte(DoubleByte addr)
{
    if (addr == JOYPAD_OFFSET)
    {
        if (!(ramMemory[addr - 0x8000] & 0x10)) // if the 4th bit is unset (looking for regular buttons)
            return (ramMemory[addr - 0x8000] & 0xf0) | Display::getDirectionKeysPressed();

        else if (!(ramMemory[addr - 0x8000] & 0x20)) // if action buttons are selected
            return (ramMemory[addr - 0x8000] & 0xf0) | Display::getActionKeysPressed();

        return 0xFF;
    }
    else if (addr <= 0x7FFF || (addr >= 0xA000 && addr <= 0xBFFF))
        return memoryChip->readByte(addr);
    else
        return ramMemory[addr - 0x8000];
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
        Display::updateBackgroundPalette(val);

    else if (addr == S0_PALETTE_OFFSET)
        Display::updateSpritePalette0(val);

    else if (addr == S1_PALETTE_OFFSET)
        Display::updateSpritePalette1(val);

    // writing to the DIV register causes it to reset to 0
    else if (addr == DIV_REGISTER_OFFSET)
    {
        romMemory[addr] = 0;
        return;
    }
    else if (addr <= 0x7FFF || (addr >= 0xA000 && addr <= 0xBFFF))
        memoryChip->writeByte(addr, val);
    else
        ramMemory[addr - 0x8000] = val;
}

// writes a double byte to memory (little endian)
void MMU::writeDoubleByte(DoubleByte addr, DoubleByte val)
{
    writeByte(addr, val & 0xFF);
    writeByte(addr + 1, (val & 0xFF00) >> 8);
}

// initialize some default values for the memory management unit
void MMU::init(uint64_t* ticks)
{
    mTicks = ticks;
}