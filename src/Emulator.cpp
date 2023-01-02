#include <fstream>

#include "Emulator.h"

// mapping for the save file encoding
const DoubleByte SAVE_FILE_INTERRUPT_OFFSET = 0xC;

Emulator::Emulator()
{
    mLastTicks = 0;
}

void Emulator::loadROM(const char* romName)
{
    mCartridge.loadROM(romName, mCPU.mmu);
}

void Emulator::run()
{
    while (true)
    {
        // only updating the input every ~1000 ticks significantly increases the speed of the emulator with no practical
        // difference being made in terms of input delay
        if (mCPU.getTicks() - mLastTicks >= 1000)
        {
            if (mInputHandler.handleInput(mCPU.mmu) == InputResponse::SAVE)
                save();

            mLastTicks = mCPU.getTicks();
        }

        mCPU.emulateCycle();
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