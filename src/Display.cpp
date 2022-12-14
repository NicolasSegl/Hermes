#include <iostream>

#include "Display.h"

// constants 
const int SDL_WINDOW_WIDTH  = 160;
const int SDL_WINDOW_HEIGHT = 144;

// offsets
const DoubleByte BG_PALLETE_OFFSET = 0xFF47;

// define the statics variables in Display
SDL_Color Display::mBackgroundPallete[4];
SDL_Color Display::mSpritePallete0[4];
SDL_Color Display::mSpritePallete1[4];
SDL_Color Display::mColourPallete[4] = 
{
    {224, 248, 208},
    {136, 192, 112},
    {52, 104, 86},
    {8, 24, 32}
};

// defines the colour pallete and general SDL_Rect object for our pixel
Display::Display()
{
    mPixelRect.w = 1;
    mPixelRect.h = 1;
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

// draws a pixel of the background to the screen
void Display::blitBG(Byte x, Byte y, Byte colourData)
{
    // find what the colour of the pixel should be
    SDL_Color colour = mBackgroundPallete[colourData];

    // set the colour of the pixel
    SDL_SetRenderDrawColor(mRenderer, colour.r, colour.g, colour.b, 255);

    mPixelRect.x = x;
    mPixelRect.y = y;

    SDL_RenderFillRect(mRenderer, &mPixelRect);
}

// draws a pixel of a sprite to the screen
void Display::blitSprite(Byte x, Byte y, Byte colourData, Byte palette)
{
    // if the palette byte is set then set then index into the second sprite pallete
    if (palette)
        SDL_SetRenderDrawColor(mRenderer, mSpritePallete1[colourData].r, mSpritePallete1[colourData].g, mSpritePallete1[colourData].b, 255);
    else
        SDL_SetRenderDrawColor(mRenderer, mSpritePallete0[colourData].r, mSpritePallete0[colourData].g, mSpritePallete0[colourData].b, 255);

    mPixelRect.x = x;
    mPixelRect.y = y;
    
    SDL_RenderFillRect(mRenderer, &mPixelRect);
}

// simply updates the screen and then clears it to white
void Display::update()
{
    SDL_RenderPresent(mRenderer);

    // clear the screen with a white background - this way we only have to draw pixels that are not white 
    SDL_SetRenderDrawColor(mRenderer, mColourPallete[0].r, mColourPallete[0].g, mColourPallete[0].b, 255);
    SDL_RenderClear(mRenderer);
}

// handles all of the events that occur to the SDL window (including keyboard inputs!)
void Display::handleEvents()
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
                    /*if (mmu->readByte(0xFF00) & (1 << 4)) // if directional is being looked at
                    {
                        mmu->writeByte(INTERRUPT_OFFSET, (Byte)Interrupts::JOYPAD);
                        mmu->writeByte(0xFF00, mmu->readByte(0xFF00) | 1);
                    }
                    */

                    break;
            }
        }
        else if (e.type == SDL_KEYUP)
        {

        }   
    }
}

void Display::updateSpritePallete0(Byte pallete)
{
    mSpritePallete0[0] = mColourPallete[pallete  & 0b00000011];
    mSpritePallete0[1] = mColourPallete[(pallete & 0b00001100) >> 2];
    mSpritePallete0[2] = mColourPallete[(pallete & 0b00110000) >> 4];
    mSpritePallete0[3] = mColourPallete[(pallete & 0b11000000) >> 6];
}

void Display::updateSpritePallete1(Byte pallete)
{
    mSpritePallete1[0] = mColourPallete[pallete  & 0b00000011];
    mSpritePallete1[1] = mColourPallete[(pallete & 0b00001100) >> 2];
    mSpritePallete1[2] = mColourPallete[(pallete & 0b00110000) >> 4];
    mSpritePallete1[3] = mColourPallete[(pallete & 0b11000000) >> 6];
}

void Display::updateBackgroundPallete(Byte pallete)
{
    mBackgroundPallete[0] = mColourPallete[pallete  & 0b00000011];
    mBackgroundPallete[1] = mColourPallete[(pallete & 0b00001100) >> 2];
    mBackgroundPallete[2] = mColourPallete[(pallete & 0b00110000) >> 4];
    mBackgroundPallete[3] = mColourPallete[(pallete & 0b11000000) >> 6];
}