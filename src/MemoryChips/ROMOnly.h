#include "MemoryChip.h"

class ROMOnly : public MemoryChip
{
public:
    ROMOnly(Byte* romMemory) : MemoryChip(romMemory) {}

    virtual Byte readByte(DoubleByte addr);
    virtual void writeByte(DoubleByte addr, Byte val);
};