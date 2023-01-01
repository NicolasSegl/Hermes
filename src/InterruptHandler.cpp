#include <iostream>

#include "InterruptHandler.h"

// constants
const DoubleByte INTERRUPTS_ENABLED_OFFSET = 0xFFFF;
const DoubleByte INTERRUPTS_FLAGS_OFFSET   = 0xFF0F;

InterruptHandler::InterruptHandler()
{
    mInterruptsEnabled = false;
}

// this function sets the PC to the point in memory designated for the interrupt that occured
// we check the previously hanlded opcode for the sole purpose of checking if it was the HALT operation
// if it was, then we want to increment the pc right before pushing it onto the stack, otherwise we might
// get stuck in a loop, wherein the HALT operation never exits
void InterruptHandler::serviceInterrupt(Byte lastOpcode, Registers* registers, MMU* mmu, Byte addr)
{
    // 0x76 is the HALT opcode
    if (lastOpcode == 0x76)
        registers->pc++;

    registers->sp -= 2;
    mmu->writeDoubleByte(registers->sp, registers->pc);
    registers->pc = addr;
    disableInterrupts();
}

// checks to see if an interrupt has come in, and if it has, if we should do anything about it
void InterruptHandler::checkInterupts(Byte lastOpcode, Registers* registers, MMU* mmu)
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
                        serviceInterrupt(lastOpcode, registers, mmu, 0x40);
                        return;

                    case (Byte)Interrupts::LCD_STAT:
                        serviceInterrupt(lastOpcode, registers, mmu, 0x48);
                        return;

                    case (Byte)Interrupts::TIMER:
                        serviceInterrupt(lastOpcode, registers, mmu, 0x50);
                        return;

                    case (Byte)Interrupts::JOYPAD:
                        serviceInterrupt(lastOpcode, registers, mmu, 0x60);
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