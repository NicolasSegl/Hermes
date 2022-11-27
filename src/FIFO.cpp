#include <iostream>

#include "FIFO.h"

FIFO::FIFO()
{
    // initialize the node count to 0
    mNodeCount = 0;
}

// push a new value onto the FIFO
void FIFO::push(unsigned char byte)
{
    // allocate memory for the new array
    unsigned char* newData = new unsigned char[++mNodeCount];

    // copy all of the old values to the array
    for (int i = 0; i < mNodeCount - 1; i++)
        newData[i] = mData[i];
        
    // set the last element in the array to the newly pushed value
    newData[mNodeCount - 1] = byte;
        
    // cleanup the old array
    delete[] mData;

    // point the data pointer to the new array in memory
    mData = newData;
}

// pop the first element in the array
Byte FIFO::pop()
{
    // allocate memory for the new array
    Byte* newData = new Byte[--mNodeCount];

    // copy all the old data into the new array, save for the first element!
    for (int i = 0; i < mNodeCount; i++)
        newData[i] = mData[i + 1];
        
    // get the value we popped so that we can later return it to the caller
    Byte poppedValue = mData[0];

    // cleanup the old array
    delete[] mData;

    // point the data pointer to the new array in memory
    mData = newData;

    return poppedValue;
}

// deallocates all the memory for the FIFO
void FIFO::clear()
{
    mNodeCount = 0;
    if (mData)
        delete[] mData;
}