#pragma once

#include "Constants.h"
#include "MMU.h"

class Cartridge
{
private:    
    Byte mReplacedROMData[256];

    int mROMSize;

    char mName[16];

    void extractHeaderData();

public:
    Cartridge() {}

    void loadROM(const char* romDir, MMU* mmu);
    void loadBIOS(Byte* romMemory);
    void unloadBIOS(Byte* romMemory);
};