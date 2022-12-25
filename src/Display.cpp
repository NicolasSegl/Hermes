#include <iostream>

#include "Display.h"

// constants 
const int SDL_WINDOW_WIDTH  = 160;
const int SDL_WINDOW_HEIGHT = 144;

// offsets
const DoubleByte BG_PALETTE_OFFSET = 0xFF47;

// define the statics variables in Display
SDL_Color Display::mBackgroundPalette[4];
SDL_Color Display::mSpritePalette0[4];
SDL_Color Display::mSpritePalette1[4];
SDL_Color Display::mColourPalette[4] = 
{
    {224, 248, 208},
    {136, 192, 112},
    {52, 104, 86},
    {8, 24, 32}
};

// defines the colour palette and general SDL_Rect object for our pixel
Display::Display()
{
    for (int pixel = 0; pixel < GAMEBOY_SCREEN_WIDTH * GAMEBOY_SCREEN_HEIGHT; pixel++)
        mPixels[pixel] = ((int)mColourPalette[0].r << 24) | ((int)mColourPalette[0].g << 16) | ((int)mColourPalette[0].b << 8);
}

// initialize the SDL2 window and fetch palette data
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
    uint32_t rgb = ((int)mBackgroundPalette[colourData].r << 24) | ((int)mBackgroundPalette[colourData].g << 16) | ((int)mBackgroundPalette[colourData].b << 8);
    mPixels[y * GAMEBOY_SCREEN_WIDTH + x] = rgb;
}

// draws a pixel of a sprite to the screen
void Display::blitSprite(Byte x, Byte y, Byte colourData, Byte palette, Byte priority)
{
    DoubleByte pixelIndex = y * GAMEBOY_SCREEN_WIDTH + x;

    /*
        if the priority attribute is set, then we only draw the sprite's pixel if the pixel that it would be drawing to is 
        the colour id 0. for this reason, all sprites are rendered AFTER the background

        this is achieved with the below if statement. note that for the sake of speed, we only compare the blue value of 
        the pixel's rgb to the blue value of the background palette's colour id 0, as this is just as good as comparing
        all three values for our purposes
    */
    if (priority && ((mPixels[pixelIndex] & 0xFF00) >> 8) != mBackgroundPalette[0].b) return;

    uint32_t rgb;

    // if the palette byte is set then set then index into the second sprite palette
    if (palette)
        rgb = ((int)mSpritePalette1[colourData].r << 24) | ((int)mSpritePalette1[colourData].g << 16) | ((int)mSpritePalette1[colourData].b << 8);
    else
        rgb = ((int)mSpritePalette0[colourData].r << 24) | ((int)mSpritePalette0[colourData].g << 16) | ((int)mSpritePalette0[colourData].b << 8);

    mPixels[pixelIndex] = rgb;
}

// simply updates the screen and then clears it to white
void Display::drawFrame()
{
    SDL_UpdateTexture(mPixelTexture, NULL, mPixels, GAMEBOY_SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(mRenderer, mPixelTexture, NULL, NULL);
    SDL_RenderPresent(mRenderer);

    // clear the screen with a white background - this way we only have to draw pixels that are not white 
    SDL_SetRenderDrawColor(mRenderer, mColourPalette[0].r, mColourPalette[0].g, mColourPalette[0].b, 255);
    SDL_RenderClear(mRenderer);
}

void Display::updateSpritePalette0(Byte palette)
{
    mSpritePalette0[0] = mColourPalette[palette  & 0b00000011];
    mSpritePalette0[1] = mColourPalette[(palette & 0b00001100) >> 2];
    mSpritePalette0[2] = mColourPalette[(palette & 0b00110000) >> 4];
    mSpritePalette0[3] = mColourPalette[(palette & 0b11000000) >> 6];
}

void Display::updateSpritePalette1(Byte palette)
{
    mSpritePalette1[0] = mColourPalette[palette  & 0b00000011];
    mSpritePalette1[1] = mColourPalette[(palette & 0b00001100) >> 2];
    mSpritePalette1[2] = mColourPalette[(palette & 0b00110000) >> 4];
    mSpritePalette1[3] = mColourPalette[(palette & 0b11000000) >> 6];
}

void Display::updateBackgroundPalette(Byte palette)
{
    mBackgroundPalette[0] = mColourPalette[palette  & 0b00000011];
    mBackgroundPalette[1] = mColourPalette[(palette & 0b00001100) >> 2];
    mBackgroundPalette[2] = mColourPalette[(palette & 0b00110000) >> 4];
    mBackgroundPalette[3] = mColourPalette[(palette & 0b11000000) >> 6];
}