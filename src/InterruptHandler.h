#include "Constants.h"
#include "MMU.h"
#include "Registers.h"

class InterruptHandler
{
private:
    bool mInterruptsEnabled;

public:
    InterruptHandler();

    void checkInterupts(Registers* registers, MMU* mmu);
    void disableInterrupts();
    void enableInterrupts();
};