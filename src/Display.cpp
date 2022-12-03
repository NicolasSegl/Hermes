#include <iostream>

#include "Display.h"

// constants 
const int SDL_WINDOW_WIDTH  = 160;
const int SDL_WINDOW_HEIGHT = 144;

// offsets
const DoubleByte BG_PALLETE_OFFSET = 0xFF47;

// defines the colour pallete
Display::Display()
{
    mColourPallete[0] = {155, 188, 15};
    mColourPallete[1] = {139, 172, 15};
    mColourPallete[2] = {48, 98, 48};
    mColourPallete[3] = {15, 56, 15};
}

// initialize the SDL2 window and fetch pallete data
void Display::init()
{    
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL2 failed to initialize!");
        exit(1);
    }

    // create the window and check if there was any error in doing so
    mWindow = SDL_CreateWindow("Gameboy Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (mWindow == NULL)
    {
        std::cout << "SDL2 window failed to be created!\n";
        exit(1);
    }

    // create the renderer and check if there was any error in doing so
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (mRenderer == NULL)
    {
        std::cout << "SDL2 renderer failed to be created!\n";
        exit(1);
    }

    // clear the screen with white
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderClear(mRenderer);

    // checkPalletes();
}

// draws a pixel to the screen
void Display::blit(Byte x, Byte y, Byte colourData)
{
    // find what the colour of the pixel should be
    SDL_Color colour = mBackgroundPallete[colourData];

    // set the colour of the pixel
    SDL_SetRenderDrawColor(mRenderer, colour.r, colour.g, colour.b, 255);

    SDL_Rect pixel;

    pixel.w = 1;
    pixel.h = 1;
    pixel.x = x;
    pixel.y = y;

    SDL_RenderFillRect(mRenderer, &pixel);
}

// simply updates the screen and then clears it to white
void Display::update()
{
    SDL_RenderPresent(mRenderer);

    // clear the screen with a white background - this way we only have to draw pixels that are not white 
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderClear(mRenderer);
}

// handles all of the events that occur to the SDL window (including keyboard inputs!)
void Display::handleEvents(MMU* mmu)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            exit(0);

        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_RIGHT:
                        // set both the right movement as well as the directional buttons column?
                        //  mmu->writeByte(0xFF00, mmu->readByte(0xFF00) | 1 | (1 << 4));

                        // write an interrupt?
                      //  mmu->writeByte()

                    // okay so also only do this when the key was previously unset 
                    if (mmu->readByte(0xFF00) & (1 << 4)) // if directional is being looked at
                    {
                        mmu->writeByte(INTERRUPT_OFFSET, (Byte)Interrupts::JOYPAD);
                        mmu->writeByte(0xFF00, mmu->readByte(0xFF00) | 1);
                    }
                    
                    break;
            }
        }
        else if (e.type == SDL_KEYUP)
        {

        }   
    }
}

void Display::checkPalletes(Byte bgPallete)
{
    // check the background pallete
    mBackgroundPallete[0] = mColourPallete[bgPallete & 0b00000011];
    mBackgroundPallete[1] = mColourPallete[(bgPallete & 0b00001100) >> 2];
    mBackgroundPallete[2] = mColourPallete[(bgPallete & 0b00110000) >> 4];
    mBackgroundPallete[3] = mColourPallete[(bgPallete & 0b11000000) >> 6];
}
