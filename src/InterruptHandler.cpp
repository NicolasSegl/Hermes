#include <iostream>

#include "InterruptHandler.h"

// constants
const DoubleByte INTERRUPTS_ENABLED_OFFSET = 0xFFFF;
const DoubleByte INTERRUPTS_FLAGS_OFFSET   = 0xFF0F;

// enum that defines which bits allow which interrupts to fire
enum INTERRUPTS
{
    VBLANK_INTERRUPT   = 1 << 0,
    LCD_STAT_INTERRUPT = 1 << 1,
    TIMER_INTERRUPT    = 1 << 2,
    SERIAL_INTERRUPT   = 1 << 3,
    JOYPAD_INTERRUPT   = 1 << 4,
};

InterruptHandler::InteruptHandler()
{
    mInterruptsEnabled = false;
}

// checks to see if an interrupt has come in, and if it has, if we should do anything about it
void InterruptHandler::checkInterupts(Registers* registers, MMU* mmu)
{
    Byte interruptsEnabled = mmu->readByte(INTERRUPTS_ENABLED_OFFSET);
    Byte interruptsFlags   = mmu->readByte(INTERRUPTS_FLAGS_OFFSET);
    // only check the for interrupts IF the interrupt's are enabled at all
    if (mInterruptsEnabled)
    {
        // Byte interruptsEnabled = mmu->readByte(INTERRUPTS_ENABLED_OFFSET);
        // Byte interruptsFlags   = mmu->readByte(INTERRUPTS_FLAGS_OFFSET);

        // there are 5 possible interupts. currently, we are only checking for a VBLANK interupt
        for (int bit = 0; bit < 5; bit++)
        {
            // if the interrupt is both enabled and has been sent to the CPU
            if ((interruptsEnabled & (1 << bit)) && (interruptsFlags & (1 << bit)))
            {
                switch (interruptsFlags & (1 << bit))
                {
                    case VBLANK_INTERRUPT:
                        // on a vblank, we want to set the pc to 0x40 and push the current position of pc to the stack
                        registers->sp -= 2;
                        mmu->writeDoubleByte(registers->sp, registers->pc);
                        registers->pc = 0x40;
                }

                // unset the interrupt flag
                mmu->writeByte(INTERRUPTS_FLAGS_OFFSET, interruptsFlags & ~(1 << bit));
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