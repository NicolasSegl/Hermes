#include <iostream>

#include "Display.h"

const int SDL_WINDOW_WIDTH  = 160;
const int SDL_WINDOW_HEIGHT = 144;

// initialize the SDL2 window
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
}

void Display::blit(Byte x, Byte y, Byte colourData)
{
    // find what the colour of the pixel should be
    Byte rgbColour = 256 / (colourData + 1) - 1;

    // set the colour of the pixel
    SDL_SetRenderDrawColor(mRenderer, rgbColour, rgbColour, rgbColour, 255);

    SDL_Rect pixel;
    pixel.w = 1;
    pixel.h = 1;
    pixel.x = x;
    pixel.y = y;

    SDL_RenderFillRect(mRenderer, &pixel);
}

void Display::update()
{
    SDL_RenderPresent(mRenderer);

    // clear the screen with a white background - this way we only have to draw pixels that are not white 
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderClear(mRenderer);
}