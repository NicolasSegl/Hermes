#pragma once

#include "Cartridge.h"
#include "CPU.h"
#include "InputHandler.h"

class Emulator
{
private:
    Cartridge mCartridge;
    CPU mCPU;
    InputHandler mInputHandler;

    uint64_t mLastInputTicks;
    uint64_t mLastFrameTicks;

    void save();

public:
    Emulator();

    void run();

    void loadSave(const char* saveName);
    void loadROM(const char* romName);
};