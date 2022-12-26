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

    void save();

public:
    void run();

    void load(const char* saveName);
    void loadROM(const char* romName);
};