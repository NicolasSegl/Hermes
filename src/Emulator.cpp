#include <stdio.h>

#include "Emulator.h"

void Emulator::run(const char* romName)
{
    mCartridge.loadROM(romName, mCPU.mmu);
    //mCartridge.loadBIOS(mCPU.mmu.memory);

    bool biosUnloaded = false;
    while (true)
    {
        mCPU.emulateCycle();

        // we need to put the ROM's original 256 bytes back into the MMU's memory once the bios has finished booting
        if (mCPU.finishedBios && !biosUnloaded)
        {
            //mCartridge.unloadBIOS(mCPU.mmu.memory);
            biosUnloaded = true;
        }
    }
}