#include <fstream>

#include "Emulator.h"

// mapping for the save file encoding
const DoubleByte SAVE_FILE_INTERRUPT_OFFSET = 0xC;

void Emulator::loadROM(const char* romName)
{
    mCartridge.loadROM(romName, mCPU.mmu);
}

void Emulator::run()
{
    //mCartridge.loadBIOS(mCPU.mmu.memory);

    bool biosUnloaded = false;
    while (true)
    {
        if (mInputHandler.handleInput(mCPU.mmu) == InputResponse::SAVE)
            save();

        mCPU.emulateCycle();

        // we need to put the ROM's original 256 bytes back into the MMU's memory once the bios has finished booting
        if (mCPU.finishedBios && !biosUnloaded)
        {
            //mCartridge.unloadBIOS(mCPU.mmu.memory);
            biosUnloaded = true;
        }
    }
}

// dump the contents of the RAM into a file
void Emulator::save()
{
    // load in the file
    std::ofstream saveFile("last.sav", std::ios::binary);

    // first save the registers in the CPU
    mCPU.saveRegistersToFile(saveFile);

    // then save the state of the master interrupt (i.e., if interrupts are enabled or not)
    mCPU.saveInterruptDataToFile(saveFile);

    // then save the RAM in the mmu, and any RAM potentially banked on a memory bank controller
    mCPU.mmu->saveRAMToFile(saveFile);

    saveFile.close();
}

// load the contents of a save, resuming the emulator to that exact same state 
void Emulator::loadSave(const char* saveName)
{
    // load in the file
    std::ifstream saveFile(saveName, std::ios::binary);

    mCPU.setRegistersFromFile(saveFile);
    mCPU.mmu->setRAMFromFile(saveFile);

    // update all of the palettes given the data just loaded
    Display::updateBackgroundPalette(mCPU.mmu->readByte(BG_PALETTE_OFFSET));
    Display::updateSpritePalette0(mCPU.mmu->readByte(S0_PALETTE_OFFSET));
    Display::updateSpritePalette1(mCPU.mmu->readByte(S1_PALETTE_OFFSET));

    // the 13th byte of the save file gives the state of the master interrupt at the time of saving
    saveFile.seekg(SAVE_FILE_INTERRUPT_OFFSET, saveFile.beg);
    char interruptsEnabled;
    saveFile.read(&interruptsEnabled, 1);
   
    if (interruptsEnabled)
        mCPU.enableInterrupts();
    else
        mCPU.disableInterrupts();

    saveFile.close();
}