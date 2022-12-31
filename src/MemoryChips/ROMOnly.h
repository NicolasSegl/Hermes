#include "MemoryChip.h"

class ROMOnly : public MemoryChip
{
private:
    bool mSupportsRam;
    Byte* mRamMemory;

public:
    ROMOnly(Byte* romMemory, Byte numOfRamBanks);

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val);

    // ROM only uses no memory banking
    virtual void saveRAMToFile(std::ofstream& file)  {}
    virtual void setRAMFromFile(std::ifstream& file) {};
};