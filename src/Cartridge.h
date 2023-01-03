#pragma once

#include "Constants.h"
#include "MMU.h"

enum class CartridgeType
{
    ROM_ONLY = 0x0,
    MBC1 = 0x1,
    MBC1_AND_RAM = 0x2,
    MBC1_AND_RAM_AND_BATTERY = 0x3,
    MBC2 = 0x5,
    MBC2_AND_BATTERY = 0x6,
    ROM_AND_RAM = 0x8,
    ROM_AND_RAM_AND_BATTERY = 0x9,
    MMM01 = 0xB,
    MMM01_AND_RAM = 0xC,
    MMM01_AND_RAM_AND_BATTERY = 0xD,
    MBC3_AND_TIMER_AND_BATTERY = 0xF,
    MBC3_AND_TIMER_AND_RAM_AND_BATTERY = 0x10,
    MBC3 = 0x11,
    MBC3_AND_RAM = 0x12,
    MBC3_AND_RAM_AND_BATTERY = 0x13,
    MBC5 = 0x19,
    MBC5_AND_RAM = 0x1A,
    MBC5_AND_RAM_AND_BATTERY = 0x1B,
    MBC5_AND_RUMBLE = 0x1C,
    MBC5_AND_RUMBLE_AND_RAM = 0x1D,
    MBC5_AND_RUMBLE_AND_RAM_AND_BATTERY = 0x1E,
    MBC6 = 0x20,
    MBC7_AND_SENSOR_AND_RUMBLE_AND_RAM_AND_BATTERY = 0x22,
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