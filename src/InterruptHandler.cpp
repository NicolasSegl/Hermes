#include <iostream>

#include "InterruptHandler.h"

bool InterruptHandler::checkInterupts(Registers* registers, MMU* mmu)
{
    // there are 5 possible interupts. currently, we are only checking for a VBLANK interupt
    for (int i = 0; i < 5; i++)
    {
        // vblank interupt
        if (mmu->readByte(0xFF0F) & i)
        {
            // on a vblank, we want to set the pc to 0x40 and push the current position of pc to the stack
            registers->sp -= 2;
            mmu->writeDoubleByte(registers->sp, registers->pc);
            registers->pc = 0x40;

            std::cout << std::hex << (int)mmu->readByte(0xFF44) << std::endl;

            // unset the interupt flag for vblanks
            mmu->writeByte(0xFF0F, mmu->readByte(0xFF0F) & ~1);

            return true;
        }
    }

    return false;
}