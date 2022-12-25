#include "Constants.h"
#include "MMU.h"

#define SDL_MAIN_HANDLED // this macro is necessary for preventing odd linking errors where WinMain cannot be found
#include "SDL.h"

// the display class handles the drawing of pixels given by the ppu to the screen using sdl
class Display
{
private:
    // contains the current ids that hold one of the four colours for the background
    static SDL_Color mBackgroundPalette[4];

    // contains the current ids for each of the four colours for sprites 
    static SDL_Color mSpritePalette0[4];
    static SDL_Color mSpritePalette1[4];

    // contains the RGB values for each colour the gameboy uses
    static SDL_Color mColourPalette[4];

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // contains the pixels that are drawn to the screen every vblank
    SDL_Texture* mPixelTexture;
    uint32_t mPixels[GAMEBOY_SCREEN_WIDTH * GAMEBOY_SCREEN_HEIGHT + 1];

public:
    Display();

    void init();
    void blitBG(Byte x, Byte y, Byte colourData); // a function that "blits" (draws) a pixel to the screen, which will be visible to the user on the next flip
    void blitSprite(Byte x, Byte y, Byte colourData, Byte palette, Byte priority);
    void drawFrame();                                // a function that updates the SDL2 window

    // void handleInput(MMU* mmu);                // a function that handles all the key inputs from the user

    // functions for updating palettes
    static void updateSpritePalette0(Byte palette);
    static void updateSpritePalette1(Byte palette);
    static void updateBackgroundPalette(Byte palette);
};