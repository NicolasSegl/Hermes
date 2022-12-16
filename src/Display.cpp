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

// the buttons used for input on the gameboy use a bit value of 1 to indicate that the button is NOT pressed
// and a 0 for if the button IS pressed. So for default, have all keys unpressed
Byte Display::mActionButtonKeys    = 0xF;
Byte Display::mDirectionButtonkeys = 0xF;

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

// defines the colour pallete and general SDL_Rect object for our pixel
Display::Display()
{
    for (int pixel = 0; pixel < GAMEBOY_SCREEN_WIDTH * GAMEBOY_SCREEN_HEIGHT; pixel++)
        mPixels[pixel] = ((int)mColourPallete[0].r << 24) | ((int)mColourPallete[0].g << 16) | ((int)mColourPallete[0].b << 8);
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

    // initialize the pixels
    mPixelTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, GAMEBOY_SCREEN_WIDTH, GAMEBOY_SCREEN_HEIGHT);
}

// draws a pixel of the background to the screen
void Display::blitBG(Byte x, Byte y, Byte colourData)
{
    uint32_t rgb = ((int)mBackgroundPallete[colourData].r << 24) | ((int)mBackgroundPallete[colourData].g << 16) | ((int)mBackgroundPallete[colourData].b << 8);
    mPixels[y * GAMEBOY_SCREEN_WIDTH + x] = rgb;
}

// draws a pixel of a sprite to the screen
void Display::blitSprite(Byte x, Byte y, Byte colourData, Byte palette)
{
    uint32_t rgb;

    // if the palette byte is set then set then index into the second sprite pallete
    if (palette)
        rgb = ((int)mSpritePallete1[colourData].r << 24) | ((int)mSpritePallete1[colourData].g << 16) | ((int)mSpritePallete1[colourData].b << 8);
    else
        rgb = ((int)mSpritePallete0[colourData].r << 24) | ((int)mSpritePallete0[colourData].g << 16) | ((int)mSpritePallete0[colourData].b << 8);

    mPixels[y * GAMEBOY_SCREEN_WIDTH + x] = rgb;
}

// simply updates the screen and then clears it to white
void Display::drawFrame()
{
    SDL_UpdateTexture(mPixelTexture, NULL, mPixels, GAMEBOY_SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(mRenderer, mPixelTexture, NULL, NULL);
    SDL_RenderPresent(mRenderer);

    // clear the screen with a white background - this way we only have to draw pixels that are not white 
    SDL_SetRenderDrawColor(mRenderer, mColourPallete[0].r, mColourPallete[0].g, mColourPallete[0].b, 255);
    SDL_RenderClear(mRenderer);
}

// handles all of the events that occur to the SDL window (including keyboard inputs!)
void Display::handleEvents(MMU* mmu)
{
    // make an interrupt if the key was not unset before

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
                    mDirectionButtonkeys &= ~INPUT_RIGHT;
                    break;

                case SDLK_LEFT:
                    mDirectionButtonkeys &= ~INPUT_LEFT;
                    break;

                case SDLK_UP:
                    mDirectionButtonkeys &= ~INPUT_UP;
                    break;

                case SDLK_DOWN:
                    mDirectionButtonkeys &= ~INPUT_DOWN;
                    break;

                /* action buttons */
                case SDLK_RETURN:
                    mActionButtonKeys &= ~INPUT_START;
                    break;

                case SDLK_RSHIFT:
                    mActionButtonKeys &= ~INPUT_SELECT;
                    break;

                case SDLK_l:
                    mActionButtonKeys &= ~INPUT_A;
                    break;

                case SDLK_k:
                    mActionButtonKeys &= ~INPUT_B;
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
            }
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