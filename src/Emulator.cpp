#include <stdio.h>

#include "Emulator.h"

void Emulator::run()
{
    mCartridge.loadROM("bios.gb");
    mCPU.emulateCycle(&mCartridge);
}