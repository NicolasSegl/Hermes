#include <stdio.h>
#include <stdlib.h>

#include "Cartridge.h"

// include all the different memory chips that we might use
#include "MemoryChips/ROMOnly.h"
#include "MemoryChips/MBC1.h"
#include "MemoryChips/MBC3.h"
#include "MemoryChips/MBC5.h"

// constants

// memory offsets into the ROM
const DoubleByte HEADER_START          = 0x100;
const DoubleByte HEADER_CARTRIDGE_TYPE = 0x147;
const DoubleByte HEADER_ROM_SIZE       = 0x148;
const DoubleByte HEADER_RAM_SIZE       = 0x149;
const DoubleByte HEADER_END            = 0x014F;

// load's a ROM into memory
void Cartridge::loadROM(const char* romDir, MMU* mmu)
{
    // open the file provided by the directory as binary
    FILE* romFile = fopen(romDir, "rb");
    if (romFile == NULL)
    {
        printf("ROM file failed to load! Was the directory provided incorrect?");
        exit(-1);
    }

    // find the length of the ROM file
    fseek(romFile, 0L, SEEK_END); // seek to the end of the file
    mROMSize = ftell(romFile);    // get the position of the cursor (now at the end of the file)
    rewind(romFile);              // seek back to the beginning of the file

    mmu->romMemory = new Byte[mROMSize];

    // load the ROM file into memory
    fread(mmu->romMemory, mROMSize, 1, romFile); // fills the ROM's memory with the data provided by the ROM file

    switch (getType(mmu->romMemory))
    {
        case CartridgeType::ROM_ONLY: 
        case CartridgeType::ROM_AND_RAM:
        case CartridgeType::ROM_AND_RAM_AND_BATTERY:
            mmu->memoryChip = new ROMOnly(mmu->romMemory, getNumRamBanks(mmu->romMemory));
            break;

        case CartridgeType::MBC1:
        case CartridgeType::MBC1_AND_RAM:
        case CartridgeType::MBC1_AND_RAM_AND_BATTERY:
            mmu->memoryChip = new MBC1(mmu->romMemory, getNumRomBanks(mmu->romMemory), getNumRamBanks(mmu->romMemory));
            break;

        case CartridgeType::MBC3:
        case CartridgeType::MBC3_AND_RAM:
        case CartridgeType::MBC3_AND_TIMER_AND_BATTERY:
        case CartridgeType::MBC3_AND_TIMER_AND_RAM_AND_BATTERY:
        case CartridgeType::MBC3_AND_RAM_AND_BATTERY:
            mmu->memoryChip = new MBC3(mmu->romMemory, getNumRomBanks(mmu->romMemory), getNumRamBanks(mmu->romMemory));
            break;

        case CartridgeType::MBC5:
        case CartridgeType::MBC5_AND_RAM:
        case CartridgeType::MBC5_AND_RAM_AND_BATTERY:
        case CartridgeType::MBC5_AND_RUMBLE:
        case CartridgeType::MBC5_AND_RUMBLE_AND_RAM:
        case CartridgeType::MBC5_AND_RUMBLE_AND_RAM_AND_BATTERY:
            mmu->memoryChip = new MBC5(mmu->romMemory, getNumRomBanks(mmu->romMemory), getNumRamBanks(mmu->romMemory));
            break;

        default:
            printf("That memory bank/cartridge type is not supported!\n");
            exit(0);
    }
}

CartridgeType Cartridge::getType(Byte* memory)
{
    return (CartridgeType)memory[HEADER_CARTRIDGE_TYPE];
}

int Cartridge::getNumRomBanks(Byte* memory)
{
    switch (memory[HEADER_ROM_SIZE])
    {
        case 0x0: return 2; // no banking
        case 0x1: return 4;
        case 0x2: return 8;
        case 0x3: return 16;
        case 0x4: return 32;
        case 0x5: return 64;
        case 0x6: return 128;
        case 0x7: return 256;
        case 0x8: return 512;
        default: return -1;
    }
}

int Cartridge::getNumRamBanks(Byte* memory)
{
    switch (memory[HEADER_ROM_SIZE])
    {
        case 0x0: return 0; // no RAM
        case 0x2: return 1;
        case 0x3: return 4;
        case 0x4: return 16;
        case 0x5: return 8;
        default:  return 0;
    }   
}
