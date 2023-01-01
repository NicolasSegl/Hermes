#include <fstream>

#include "Constants.h"
#include "MMU.h"
#include "Registers.h"

class InterruptHandler
{
private:
    bool mInterruptsEnabled;

    void serviceInterrupt(Byte lastOpcode, Registers* registers, MMU* mmu, Byte addr);

public:
    InterruptHandler();

    void checkInterupts(Byte lastOpcode, Registers* registers, MMU* mmu);
    void disableInterrupts();
    void enableInterrupts();

    void saveDataToFile(std::ofstream& file);

    bool areInterruptsEnabled() { return mInterruptsEnabled; }
};