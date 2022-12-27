#include "MBC3.h"

#include <iostream>

// constants
const DoubleByte ROM_BANK_SIZE = 0x4000;
const DoubleByte RAM_BANK_SIZE = 0x2000;

// save file offsets
const DoubleByte SAVE_FILE_MBC_OFFSET = 0x800C; // the offset at which the data for the mbc3 starts

MBC3::MBC3(Byte* memory)
 : MemoryChip(memory)
{ 
    mSelectedROMBank = 1;
    mSelectedRAMBank = 0;

    mRAMEnabled = false;
    
    for (int ramBank = 0; ramBank < 4; ramBank++)
        mRAMBanks[ramBank] = new Byte[RAM_BANK_SIZE];
}

Byte MBC3::readByte(DoubleByte addr)
{
    // if trying to read a ROM memory bank that is NOT bank 0:
    if (addr >= 0x4000 && addr <= 0x7FFF)
        return mROMMemory[(addr - 0x4000) + ROM_BANK_SIZE * mSelectedROMBank];
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            return mRAMBanks[mSelectedRAMBank][addr - 0xA000];
        else
            return 0xFF;
    }

    return mROMMemory[addr];
}

void MBC3::writeByte(DoubleByte addr, Byte val)
{
    if (addr <= 0x1FFF)
        // enable RAM if the first 4 bits of the value being writte equal 0xA. Otherwise, set it to false
        mRAMEnabled = (val & 0xF) == 0xA;
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // only use the first 7 (not all 8) bits of the val to determine which rom bank is being selected
        mSelectedROMBank = val & 0x7F;

        // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
        if (mSelectedROMBank == 0)
            mSelectedROMBank = 1;
    }
    else if (addr >= 0x4000 && addr <= 0x5FFF)
    {
        if (val <= 0x3)
        {
            std::cout << "selected ram bank: " << (int)val << std::endl;
            mSelectedRAMBank = val;
        }
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
    }
}

// from addresses 0x800C-0x1000F (the original 0x8000 RAM in all games + 0xB bytes in registers and 0x1 byte in saving the state of the master interrupt),
// the RAM from the memory controller's RAM banks will be stored
// the memory stored contains all of the memory banks, as well as the state of the ram/rom at the time of saving
void MBC3::saveRAMToFile(std::ofstream& file)
{
    // the first three bytes starting at the mbc offset contain the states of the ram and rom
    file.write((char*)&mRAMEnabled, 1);
    file.write((char*)&mSelectedRAMBank, 1);
    file.write((char*)&mSelectedROMBank, 1);

    for (int bank = 0; bank < NUM_OF_RAM_BANKS; bank++)
        file.write((char*)mRAMBanks[bank], RAM_BANK_SIZE);
}

void MBC3::setRAMFromFile(std::ifstream& file)
{
    char ramEnabled, ramBank, romBank;
    file.seekg(SAVE_FILE_MBC_OFFSET, file.beg);

    // the first 3 bytes saved store the state of the ram and rom
    file.read(&ramEnabled, 1);
    file.read(&ramBank, 1);
    file.read(&romBank, 1);

    mRAMEnabled = ramEnabled;
    mSelectedRAMBank = ramBank;
    mSelectedROMBank = romBank;

    if (mSelectedROMBank == 0)
        mSelectedROMBank = 1;

    char* dataBuffer = new char[RAM_BANK_SIZE * NUM_OF_RAM_BANKS];

    file.read(dataBuffer, RAM_BANK_SIZE * NUM_OF_RAM_BANKS);

    for (int bank = 0; bank < NUM_OF_RAM_BANKS; bank++)
        for (int byte = RAM_BANK_SIZE * bank; byte < RAM_BANK_SIZE * bank + RAM_BANK_SIZE; byte++)
            mRAMBanks[bank][byte - RAM_BANK_SIZE * bank] = dataBuffer[byte];
}