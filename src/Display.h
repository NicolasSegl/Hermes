#include "Constants.h"
#include "MMU.h"

#define SDL_MAIN_HANDLED // this macro is necessary for preventing odd linking errors where WinMain cannot be found
#include "SDL.h"

// the display class handles the drawing of pixels given by the ppu to the screen using sdl
class Display
{
private:
    // enum for the different keys that will be mapped to gameboy inputs
    enum class InputKeys
    {
        F      = SDL_KeyCode::SDLK_f,
        V      = SDL_KeyCode::SDLK_v,
        UP     = SDL_KeyCode::SDLK_UP,
        DOWN   = SDL_KeyCode::SDLK_DOWN,
        LEFT   = SDL_KeyCode::SDLK_LEFT,
        RIGHT  = SDL_KeyCode::SDLK_RIGHT,
        START  = SDL_KeyCode::SDLK_RETURN,
        SELECT = SDL_KeyCode::SDLK_RSHIFT,
    };

    // contains the current ids that hold one of the four colours for the background
    static SDL_Color mBackgroundPallete[4];

    // contains the current ids for each of the four colours for sprites 
    static SDL_Color mSpritePallete0[4];
    static SDL_Color mSpritePallete1[4];

    // contains the RGB values for each colour the gameboy uses
    static SDL_Color mColourPallete[4];

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    // contains the info about which keys are currently pressed (encoding them using bits)
    static Byte mActionButtonKeys;
    static Byte mDirectionButtonkeys;

    // contains the pixels that are drawn to the screen every vblank
    SDL_Texture* mPixelTexture;
    uint32_t mPixels[GAMEBOY_SCREEN_WIDTH * GAMEBOY_SCREEN_HEIGHT];

public:
    Display();

    void handleEvents(MMU* mmu);
    void init();
    void blitBG(Byte x, Byte y, Byte colourData); // a function that "blits" (draws) a pixel to the screen, which will be visible to the user on the next flip
    void blitSprite(Byte x, Byte y, Byte colourData, Byte palette, Byte priority);
    void drawFrame();                                // a function that updates the SDL2 window

    // void handleInput(MMU* mmu);                // a function that handles all the key inputs from the user

    // functions for updating palettes
    static void updateSpritePallete0(Byte pallete);
    static void updateSpritePallete1(Byte pallete);
    static void updateBackgroundPallete(Byte pallete);

    static Byte getDirectionKeysPressed() { return mDirectionButtonkeys; }
    static Byte getActionKeysPressed()    { return mActionButtonKeys;    }
};