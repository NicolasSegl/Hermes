#include "Display.h"
#include "InputHandler.h"
#include "MMU.h"

#include <iostream>

// offsets
const DoubleByte SPRITE_DATA_OFFSET  = 0xFE00;
const DoubleByte OAM_DMA_OFFSET      = 0xFF46;
const DoubleByte JOYPAD_OFFSET       = 0xFF00;

// timer offsets
const DoubleByte DIV_REGISTER_OFFSET  = 0xFF04;

// save file offsets
const DoubleByte SAVE_FILE_RAM_OFFSET_START = 0xD;

// reads a single bye from memory
// depending on what is trying to be read from memory, we may have to 
// do something particular (such as for input)
Byte MMU::readByte(DoubleByte addr)
{
    if (addr == JOYPAD_OFFSET)
    {
        if (!(ramMemory[addr - RAM_OFFSET] & 0x10)) // if the 4th bit is unset (looking for regular buttons)
            return (ramMemory[addr - RAM_OFFSET] & 0xf0) | InputHandler::getDirectionKeysPressed();

        else if (!(ramMemory[addr - RAM_OFFSET] & 0x20)) // if action buttons are selected
            return (ramMemory[addr - RAM_OFFSET] & 0xf0) | InputHandler::getActionKeysPressed();

        return 0xFF;
    }
    else if (addr <= 0x7FFF || (addr >= 0xA000 && addr <= 0xBFFF))
        return memoryChip->readByte(addr);
    else
        return ramMemory[addr - RAM_OFFSET];
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
    {
        Display::updateBackgroundPalette(val);
        ramMemory[addr - RAM_OFFSET] = val;
    }

    else if (addr == S0_PALETTE_OFFSET)
    {
        Display::updateSpritePalette0(val);
        ramMemory[addr - RAM_OFFSET] = val;
    }

    else if (addr == S1_PALETTE_OFFSET)
    {
        Display::updateSpritePalette1(val);
        ramMemory[addr - RAM_OFFSET] = val;
    }

    // writing to the DIV register causes it to reset to 0
    else if (addr == DIV_REGISTER_OFFSET)
    {
        romMemory[addr] = 0;
        return;
    }
    else if (addr <= 0x7FFF || (addr >= 0xA000 && addr <= 0xBFFF))
        memoryChip->writeByte(addr, val);
    else
        ramMemory[addr - RAM_OFFSET] = val;
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

    // initialize the values in the RAM. taken from the pandocs at https://gbdev.io/pandocs/Power_Up_Sequence.html
    writeByte(0xFF00, 0xCF);
    writeByte(0xFF01, 0x0);
    writeByte(0xFF02, 0x7E);
    writeByte(0xFF04, 0x18);
    writeByte(0xFF05, 0x0);
    writeByte(0xFF06, 0x0);
    writeByte(0xFF07, 0xF8);
    writeByte(0xFF0F, 0xE1);
    writeByte(0xFF10, 0x80);
    writeByte(0xFF11, 0xBF);
    writeByte(0xFF12, 0xF3);
    writeByte(0xFF13, 0xFF);
    writeByte(0xFF14, 0xBF);
    writeByte(0xFF16, 0x3F);
    writeByte(0xFF17, 0x0);
    writeByte(0xFF18, 0xFF);
    writeByte(0xFF19, 0xBF);
    writeByte(0xFF1A, 0x7F);
    writeByte(0xFF1B, 0xFF);
    writeByte(0xFF1C, 0x9F);
    writeByte(0xFF1D, 0xFF);
    writeByte(0xFF1E, 0xBF);
    writeByte(0xFF20, 0xFF);
    writeByte(0xFF21, 0x0);
    writeByte(0xFF22, 0x0);
    writeByte(0xFF23, 0xBF);
    writeByte(0xFF24, 0x77);
    writeByte(0xFF25, 0xF3);
    writeByte(0xFF26, 0xF1);
    writeByte(0xFF40, 0x91);
    writeByte(0xFF41, 0x81);
    writeByte(0xFF42, 0x0);
    writeByte(0xFF43, 0x0);
    writeByte(0xFF44, 0x91);
    writeByte(0xFF45, 0x0);
    writeByte(0xFF46, 0xFF);
    writeByte(0xFF47, 0xFC);
    writeByte(0xFF4A, 0x0);
    writeByte(0xFF4B, 0x0);
    writeByte(0xFF4D, 0xFF);
    writeByte(0xFF4F, 0xFF);
    writeByte(0xFF51, 0xFF);
    writeByte(0xFF52, 0xFF);
    writeByte(0xFF53, 0xFF);
    writeByte(0xFF54, 0xFF);
    writeByte(0xFF55, 0xFF);
    writeByte(0xFF56, 0xFF);
    writeByte(0xFF68, 0xFF);
    writeByte(0xFF69, 0xFF);
    writeByte(0xFF6A, 0xFF);
    writeByte(0xFF6B, 0xFF);
    writeByte(0xFF70, 0xFF);
    writeByte(0xFFFF, 0x0);
}

void MMU::saveRAMToFile(std::ofstream& file)
{
    file.write((char*)ramMemory, RAM_MEMORY_SIZE);
    memoryChip->saveRAMToFile(file);
}

void MMU::setRAMFromFile(std::ifstream& file)
{
    char* dataBuffer = new char[RAM_MEMORY_SIZE];

    // read 12 bytes in (as to read past the register values)
    file.seekg(0, file.beg);
    file.seekg(SAVE_FILE_RAM_OFFSET_START, file.beg);

    file.read(dataBuffer, RAM_MEMORY_SIZE);
    
    for (int byte = 0; byte < RAM_MEMORY_SIZE; byte++)
        ramMemory[byte] = Byte(dataBuffer[byte]);

    memoryChip->setRAMFromFile(file);
}