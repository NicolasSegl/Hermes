#include "MemoryChip.h"

class ROMOnly : public MemoryChip
{
public:
    ROMOnly(Byte* romMemory) : MemoryChip(romMemory) {}

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val);

    // ROM only uses no memory banking
    virtual void saveRAMToFile(std::ofstream& file) {}
};