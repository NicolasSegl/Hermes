#pragma once

#include "Constants.h"
#include "MMU.h"

class Cartridge
{
private:
    // memory management unit
    // uses a union so that we can address the entire cartridge's data or use the individual segments of it
    
    int mROMSize;

    char mName[16];

    void extractHeaderData();

public:
    Cartridge() {}

    void loadROM(const char* romDir, Byte* memory);
};