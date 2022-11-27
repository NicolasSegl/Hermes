#pragma once

#include "Constants.h"

// a very simple first in first out implentation for this emulator's needs
class FIFO
{
private:
    Byte* mData;
    int mNodeCount;
    
public:
    FIFO();
    
    void push(Byte byte);
    Byte pop();
    void clear();

    int getSize() { return mNodeCount; }
};
