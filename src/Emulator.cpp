#include <stdio.h>

#include "Emulator.h"

void Emulator::run()
{
    mCartridge.loadROM("bios.gb", mCPU.mmu.memory);

    for (int i = 0; i < 0x100; i++)
        mCPU.emulateCycle();
}