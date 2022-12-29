#include <iostream>

#include "InterruptHandler.h"

// constants
const DoubleByte INTERRUPTS_ENABLED_OFFSET = 0xFFFF;
const DoubleByte INTERRUPTS_FLAGS_OFFSET   = 0xFF0F;

InterruptHandler::InterruptHandler()
{
    mInterruptsEnabled = false;
}

// checks to see if an interrupt has come in, and if it has, if we should do anything about it
void InterruptHandler::checkInterupts(Registers* registers, MMU* mmu)
{
    // only check the for interrupts IF the interrupt's are enabled at all
    if (mInterruptsEnabled)
    {
        Byte interruptsEnabled = mmu->readByte(INTERRUPTS_ENABLED_OFFSET);
        Byte interruptsFlags   = mmu->readByte(INTERRUPTS_FLAGS_OFFSET);

        // there are 5 possible interupts. currently, we are only checking for a VBLANK interupt
        for (int bit = 0; bit < 5; bit++)
        {
            // if the interrupt is both enabled and has been sent to the CPU
            if ((interruptsEnabled & (1 << bit)) && (interruptsFlags & (1 << bit)))
            {
                // unset the interrupt flag
                mmu->writeByte(INTERRUPTS_FLAGS_OFFSET, interruptsFlags & ~(1 << bit));
                
                // determine which interrupt was called. because only one interrupt can be handled at once, return 
                // immediately if an interrupt is found to have occurred
                switch (interruptsFlags & (1 << bit))
                {
                    case (Byte)Interrupts::VBLANK:
                        // on a vblank, we want to set the pc to 0x40 and push the current position of pc to the stack
                        registers->sp -= 2;
                        mmu->writeDoubleByte(registers->sp, registers->pc);
                        registers->pc = 0x40;
                        disableInterrupts();
                        return;

                    case (Byte)Interrupts::LCD_STAT:
                        registers->sp -= 2;
                        mmu->writeDoubleByte(registers->sp, registers->pc);
                        registers->pc = 0x48;
                        disableInterrupts();
                        return;

                    case (Byte)Interrupts::JOYPAD:
                        std::cout << "joypad interrupt\n";
                        registers->sp -= 2;
                        mmu->writeDoubleByte(registers->sp, registers->pc);
                        registers->pc = 0x60;
                        disableInterrupts();
                        return;
                        
                }
            }
        }
    }
}

void InterruptHandler::disableInterrupts()
{
    mInterruptsEnabled = false;
}

void InterruptHandler::enableInterrupts()
{
    mInterruptsEnabled = true;
}

void InterruptHandler::saveDataToFile(std::ofstream& file)
{
    file.write((char*)&mInterruptsEnabled, 1);
}