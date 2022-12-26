#pragma once

#include "SDL.h"

#include "MMU.h"

enum class InputResponse
{
    NOTIHNG,
    SAVE,
};

class InputHandler
{
private:
    // contains the info about which keys are currently pressed (encoding them using bits)
    static Byte mActionButtonKeys;
    static Byte mDirectionButtonkeys;

public:
    InputHandler() {}

    InputResponse handleInput(MMU* mmu);

    static Byte getDirectionKeysPressed() { return mDirectionButtonkeys; }
    static Byte getActionKeysPressed()    { return mActionButtonKeys;    }
};