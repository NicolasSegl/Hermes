#include "Constants.h"

#define SDL_MAIN_HANDLED // this macro is necessary for preventing odd linking errors where WinMain cannot be found
#include "SDL.h"

// the display class handles the drawing of pixels given by the ppu to the screen using sdl
class Display
{
private:

    // contains the current ids that hold one of the four colours 
    SDL_Color mBackgroundPallete[4];

    // contains the RGB values for each colour the gameboy uses
    SDL_Color mColourPallete[4];

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

public:
    Display();

    void handleEvents();
    void init();
    void blit(Byte x, Byte y, Byte colourData); // a function that "blits" (draws) a pixel to the screen, which will be visible to the user on the next flip
    void update();                              // a function that updates the SDL2 window

    void checkPalletes(Byte bgPallete);
};