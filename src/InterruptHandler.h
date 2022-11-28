#include "Constants.h"
#include "MMU.h"
#include "Registers.h"

class InterruptHandler
{
public:
    InteruptHandler();

    bool checkInterupts(Registers* registers, MMU* mmu);
};