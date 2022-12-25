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

public:
    void run(const char* romName);
};