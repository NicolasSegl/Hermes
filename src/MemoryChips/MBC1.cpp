#include "MBC1.h"

// the number of ROM banks needed (64) for the two higher bits to be able to be set (if in ROM mode and not RAM mode)
const Byte ROM_SIZE_FOR_HIGHER_BITS = 0x40;

MBC1::MBC1(Byte* memory, Byte numOfRomBanks, Byte numOfRamBanks) 
    : MBC(memory, numOfRomBanks, numOfRamBanks)
{ 
    mUpperRomBankBits = 0; 
    mMemoryMode = MEMORY_MODE::ROM_MODE;
}

void MBC1::writeByte(DoubleByte addr, Byte val)
{
    if (addr <= 0x1FFF && mNumOfRamBanks > 0)
        // enable RAM if the first 4 bits of the value being writte equal 0xA. Otherwise, set it to false
        mRAMEnabled = (val & 0xF) == 0xA;
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // only use the first 5 (not all 8) bits of the val to determine which rom bank is being selected
        mSelectedROMBank = val & 0x1F;

        // if we are in ROM mode, then we want to use the extra 2 bits supplied by the program when it writes to addresses 0x4000-0x5FFF
        if (mMemoryMode == MEMORY_MODE::ROM_MODE && mNumOfRomBanks >= ROM_SIZE_FOR_HIGHER_BITS)
            // shift the upper 2 bits left 5 times because the first 5 bits are occupied by val (the value just written to 0x2000-0x3FFF)
            mSelectedROMBank |= (mUpperRomBankBits << 5);

        // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
        if (mSelectedROMBank == 0 || mSelectedROMBank == 0x20 || mSelectedROMBank == 0x40 || mSelectedROMBank == 0x60)
            mSelectedROMBank++;
    }
    else if (addr >= 0x4000 && addr <= 0x5FFF)
    {
        if (mMemoryMode == MEMORY_MODE::RAM_MODE)
            mSelectedRAMBank = val & 0b11;
        else if (mMemoryMode == MEMORY_MODE::ROM_MODE && mNumOfRomBanks >= ROM_SIZE_FOR_HIGHER_BITS)
        {
            mUpperRomBankBits = val & 0b11;
            mSelectedROMBank = (mSelectedROMBank & 0x1F) | (mUpperRomBankBits << 5);

            // never let the ROM bank be 0 (the first 0x4000 bytes already belong to rom bank 0)
            if (mSelectedROMBank == 0 || mSelectedROMBank == 0x20 || mSelectedROMBank == 0x40 || mSelectedROMBank == 0x60)
                mSelectedROMBank++;
        }
    }
    else if (addr >= 0x6000 && addr <= 0x7FFF) // this address is written to when switching the rom/ram mode
    {
        // we are only concerned with the first 2 bits of the value
        // value of 1 is written when selecting RAM mode
        if (val == 1)
            mMemoryMode = MEMORY_MODE::RAM_MODE;
        else // a value of zero for ROM mode
        {
            // can only use RAM bank 0 when in ROM mode
            mSelectedRAMBank = 0;

            mMemoryMode = MEMORY_MODE::ROM_MODE;
        }
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if (mRAMEnabled)
            mRAMBanks[mSelectedRAMBank][addr - 0xA000] = val;
    }
}

// the memory stored contains all of the memory banks, as well as the state of the ram/rom at the time of saving
// as well, the MBC1 stores the memory mode and (if any) the upper bits of the ROM bank
void MBC1::saveRAMToFile(std::ofstream& file)
{
    Byte ramEnabled = (Byte)mRAMEnabled;

    // the first three bytes starting at the mbc offset contain the states of the ram and rom
    file.write((char*)&ramEnabled, 1);
    file.write((char*)&mSelectedRAMBank, 1);
    file.write((char*)&mSelectedROMBank, 2);
    file.write((char*)&mMemoryMode, 1);
    file.write((char*)&mUpperRomBankBits, 1);

    for (int bank = 0; bank < mNumOfRamBanks; bank++)
        file.write((char*)mRAMBanks[bank], RAM_BANK_SIZE);
}

// because the MBC1 has some specifics variables that need to be loaded with a save file, we need to define our own 
// virtual function 
void MBC1::setRAMFromFile(std::ifstream& file)
{
    char ramEnabled, ramBank, romBank[2], memoryMode, upperRomBits;
    file.seekg(SAVE_FILE_MBC_OFFSET, file.beg);

    file.read(&ramEnabled, 1);
    file.read(&ramBank, 1);
    file.read(romBank, 2);
    file.read(&memoryMode, 1);
    file.read(&upperRomBits, 1);

    mRAMEnabled      = false;
    mSelectedRAMBank = ramBank;
    mSelectedROMBank = romBank[0] | (romBank[1] << 8);

    mMemoryMode       = (MEMORY_MODE)(memoryMode & 0b1);
    mUpperRomBankBits = (upperRomBits & 0b11);

    if (mSelectedROMBank == 0 || mSelectedROMBank == 0x20 || mSelectedROMBank == 0x40 || mSelectedROMBank == 0x60)
        mSelectedROMBank++;

    char* dataBuffer = new char[RAM_BANK_SIZE * mNumOfRamBanks];

    file.read(dataBuffer, RAM_BANK_SIZE * mNumOfRamBanks);

    for (int bank = 0; bank < mNumOfRamBanks; bank++)
        for (int byte = RAM_BANK_SIZE * bank; byte < RAM_BANK_SIZE * bank + RAM_BANK_SIZE; byte++)
            mRAMBanks[bank][byte - RAM_BANK_SIZE * bank] = dataBuffer[byte];
}