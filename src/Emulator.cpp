#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "Emulator.h"

// mapping for the save file encoding
const DoubleByte SAVE_FILE_INTERRUPT_OFFSET = 0xC;

// the refresh rate of the gameboy's LCD was 59.73Hz
const double MILLISECONDS_PER_FRAME = 1000.f / 59.73;

const int TICKS_BETWEEN_FRAMES = 70224;

Emulator::Emulator()
{
    mLastInputTicks = 0;
    mLastFrameTicks = 0;
}

// sets the name that will be used for save files (the name of the ROM file + .sav)
void Emulator::setSaveFileName(const char* title)
{
    int endIndex = 0;

    // iterate through the ROM file being ran until you run into the extension (which starts with a period)
    for (int character = 0; character < 256; character++)
    {
        mSaveFileName[character] = title[character];

        if (title[character] == '.')
        {
            endIndex = character + 1;
            break;
        }
    }

    mSaveFileName[endIndex]     = 's';
    mSaveFileName[endIndex + 1] = 'a';
    mSaveFileName[endIndex + 2] = 'v';
    mSaveFileName[endIndex + 3] = '\0';

}

void Emulator::loadROM(const char* romName)
{
    mCartridge.loadROM(romName, mCPU.mmu);
}

void Emulator::run()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> time1, time2;
    time2 = std::chrono::high_resolution_clock::now();

    while (true)
    {
        time1 = std::chrono::high_resolution_clock::now();

        while (mCPU.getTicks() - mLastFrameTicks < TICKS_BETWEEN_FRAMES)
        {
            // only updating the input every ~1000 ticks significantly increases the speed of the emulator with no practical
            // difference being made in terms of input delay
            if (mCPU.getTicks() - mLastInputTicks >= 1000)
            {
                if (mInputHandler.handleInput(mCPU.mmu) == InputResponse::SAVE)
                    save();

                mLastInputTicks = mCPU.getTicks();
            }

            mCPU.emulateCycle();
        }

        mLastFrameTicks = mCPU.getTicks();

        time2 = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time2 - time1);

        // if the frame did not take as long as one frame should take to match the ~60Hz LCD of the gameboy,
        // then sleep for the remaining time so that ROMs run as intended
        if (deltaTime.count() < MILLISECONDS_PER_FRAME)
            std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(MILLISECONDS_PER_FRAME - deltaTime.count()));
    }
}

// dump the contents of the RAM into a file
void Emulator::save()
{
    // load in the file
    std::ofstream saveFile(mSaveFileName, std::ios::binary);

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