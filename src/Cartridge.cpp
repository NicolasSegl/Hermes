#include <stdio.h>
#include <stdlib.h>

#include "Cartridge.h"

// constants

// memory offsets into the ROM
const int HEADER_START = 0x100;
const int HEADER_END   = 0x014F;

void Cartridge::loadROM(const char* romDir)
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

    // load the ROM file into memory
    fread(mmu.memory, mROMSize, 1, romFile); // fills the ROM's memory with the data provided by the ROM file
}