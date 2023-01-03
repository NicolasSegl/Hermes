#pragma once

#include "Constants.h"
#include "MMU.h"

enum class CartridgeType
{
    ROM_ONLY = 0x0,
    MBC1,
    MBC1_AND_RAM,
    MBC1_AND_RAM_AND_BATTERY,
    MBC2,
    MBC2_AND_BATTERY,
    ROM_AND_RAM,
    ROM_AND_RAM_AND_BATTERY,
    MMM01,
    MMM01_AND_RAM,
    MMM01_AND_RAM_AND_BATTERY,
    MBC3_AND_TIMER_AND_BATTERY,
    MBC3_AND_TIMER_AND_RAM_AND_BATTERY,
    MBC3,
    MBC3_AND_RAM,
    MBC3_AND_RAM_AND_BATTERY = 0x13,
    MBC5,
    MBC5_AND_RAM,
    MBC5_AND_RAM_AND_BATTERY,
    MBC5_AND_RUMBLE,
    MBC5_AND_RUMBLE_AND_RAM,
    MBC5_AND_RUMBLE_AND_RAM_AND_BATTERY,
    MBC6,
    MBC7_AND_SENSOR_AND_RUMBLE_AND_RAM_AND_BATTERY,
};

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

    CartridgeType getType(Byte* romMemory); // returns the cartridge type (most importantly the type of memory bank controller)
    int getNumRomBanks(Byte* romMemory);
    int getNumRamBanks(Byte* romMemory);
};