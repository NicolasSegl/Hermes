#pragma once

#include "Constants.h"
#include "MMU.h"

class Cartridge
{
private:
    // memory management unit
    // uses a union so that we can address the entire cartridge's data or use the individual segments of it
    
    Byte mReplacedROMData[256];

    int mROMSize;

    char mName[16];

    void extractHeaderData();

public:
    Cartridge() {}

    void loadROM(const char* romDir, Byte* memory);
    void loadBIOS(Byte* memory);
    void unloadBIOS(Byte* memory);
};