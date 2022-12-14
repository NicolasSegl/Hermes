#include "InputHandler.h"

// the buttons used for input on the gameboy use a bit value of 1 to indicate that the button is NOT pressed
// and a 0 for if the button IS pressed. So for default, have all keys unpressed
Byte InputHandler::mActionButtonKeys    = 0xF;
Byte InputHandler::mDirectionButtonkeys = 0xF;

// en enum defining 
enum INPUT_BUTTON_ENCODINGS
{
    // direction buttons
    INPUT_RIGHT = 1 << 0,
    INPUT_LEFT  = 1 << 1,
    INPUT_UP    = 1 << 2,
    INPUT_DOWN  = 1 << 3,

    // actions buttons
    INPUT_A      = 1 << 0,
    INPUT_B      = 1 << 1,
    INPUT_SELECT = 1 << 2,
    INPUT_START  = 1 << 3,
};

// handles a keypress and checks to see if an interrupt should or should not occur
// keyset is a pointer to the Byte representing either the direction keys or the action keys
void InputHandler::keyPress(MMU* mmu, Byte* keyset, Byte keyPressed)
{
    // if the key was not set before, then send an interrupt
    if (*keyset & keyPressed)
        mmu->writeByte(INTERRUPT_OFFSET, mmu->readByte(INTERRUPT_OFFSET) | (Byte)Interrupts::JOYPAD);

    *keyset &= ~keyPressed;
}

// handles all the user input using SDL2
InputResponse InputHandler::handleInput(MMU* mmu)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            exit(0);

        else if (e.type == SDL_KEYDOWN)
        {
            // depending on which key is pressed, unset one of the bits in the Byte variables that represent which
            // keys are currently pressed (unsetting because a value of 0 indicates that the button is pressed)
            switch (e.key.keysym.sym)
            {
                /* direction buttons */
                case SDLK_RIGHT:
                    keyPress(mmu, &mDirectionButtonkeys, INPUT_RIGHT);
                    break;

                case SDLK_LEFT:
                    keyPress(mmu, &mDirectionButtonkeys, INPUT_LEFT);
                    break;

                case SDLK_UP:
                    keyPress(mmu, &mDirectionButtonkeys, INPUT_UP);
                    break;

                case SDLK_DOWN:
                    keyPress(mmu, &mDirectionButtonkeys, INPUT_DOWN);
                    break;

                /* action buttons */
                case SDLK_RETURN:
                    keyPress(mmu, &mActionButtonKeys, INPUT_START);
                    break;

                case SDLK_RSHIFT:
                    keyPress(mmu, &mActionButtonKeys, INPUT_SELECT);
                    break;

                case SDLK_l:
                    keyPress(mmu, &mActionButtonKeys, INPUT_A);
                    break;

                case SDLK_k:
                    keyPress(mmu, &mActionButtonKeys, INPUT_B);
                    break;
            }
        }
        else if (e.type == SDL_KEYUP)
        {
            // same idea here as the switch statement for when the key is pressed down, only now we are setting the bit
            // as to indicate that the button is no longer being pressed down
            switch (e.key.keysym.sym)
            {
                /* direction buttons */
                case SDLK_RIGHT:
                    mDirectionButtonkeys |= INPUT_RIGHT;
                    break;

                case SDLK_LEFT:
                    mDirectionButtonkeys |= INPUT_LEFT;
                    break;

                case SDLK_UP:
                    mDirectionButtonkeys |= INPUT_UP;
                    break;
                
                case SDLK_DOWN:
                    mDirectionButtonkeys |= INPUT_DOWN;
                    break;

                /* action buttons */
                case SDLK_RETURN:
                    mActionButtonKeys |= INPUT_START;
                    break;

                case SDLK_RSHIFT:
                    mActionButtonKeys |= INPUT_SELECT;
                    break;

                case SDLK_l:
                    mActionButtonKeys |= INPUT_A;
                    break;

                case SDLK_k:
                    mActionButtonKeys |= INPUT_B;
                    break;

                // pressing escape will save the game
                case SDLK_ESCAPE:
                    return InputResponse::SAVE;
            }
        }   
    }

    return InputResponse::NOTIHNG;
}