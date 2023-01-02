#pragma once

#include <vector>

#include "Constants.h"
#include "Display.h"
#include "MMU.h"

// the PPU (picture processing unit) handles the graphics
class PPU
{
private:
    // an enum containing the various meanings of the different flags set in the LCD control register
    enum LCDC_BITS
    {
        BG_ENABLE              = 1 << 0,
        SPRITE_ENABLE          = 1 << 1,
        SPRITE_HEIGHT          = 1 << 2,
        BG_TILE_MAP            = 1 << 3,
        BG_AND_WINDOW_TILE_MAP = 1 << 4,
        WINDOW_ENABLE          = 1 << 5,
        WINDOW_TILE_MAP        = 1 << 6,
        LCD_ENABLE             = 1 << 7
    };

    // an enum containing the various meanings of the different flags that can be set in the sprite attributes register
    // note that this enum starts at bit 7!
    enum SPRITE_ATTRIBUTES
    {
        BG_WINDOW_DRAWN_OVER = 1 << 7, // 0 = draw sprites over bg and window, 1 = draw colours ids 1, 2, and 3 (not 0) of the bg and window over the sprite
        Y_FLIP               = 1 << 6, // 0 = no flip, 1 = flip vertically
        X_FLIP               = 1 << 5, // 0 = no flip, 1 = flip horizontally
        S_PALLETE            = 1 << 4  // 0 = 0xFF48,  1 = 0xFF49
    };

    // an enum containing the states that the PPU can be in
    enum PPU_STATE
    {
        SEARCH_OAM,
        RENDER_SCANLINE,
        HBLANK,
        VBLANK,
    };

    // pointer to the MMU's memory address at 0xFF40: the LCDC (LCD control) register
    Byte* mLCDC;

    // holds the current state of the PPU (i.e., what it is doing at any given moment)
    PPU_STATE mState;

    // holds the current memory address offset of the current tile being read in for the background
    DoubleByte mBgTileMapRowAddr;

    // same as above but for the window
    DoubleByte mWindowTileMapRowAddr;

    // holds the x, y position of the tilemap
    int x, ly;
    
    // holds the number of ticks that the ppu has counted
    int mPPUTicks;

    // the tile index is the x-value of the tile the fetcher is fetching. for instance, the first tile from the left has a tile ID
    // of 0, while the fourteenth tile has an index value of 14
    Byte mTileIndex;

    // the tile ID is the value stored at the start of the OAM memory + tileIndex bytes
    Byte mTileID;

    // the tile line defines which of the 8 rows in each tile we want to look at 
    Byte mTileLine;

    // holds the pixel data for a given byte of a tile's row
    Byte mPixelDataBuffer;

    // temporarily holds the the data for a row of pixels in a tile
    Byte mPixelData[8];

    // contains the window of the emulator
    Display mDisplay;

    // contains the current state of the STAT register
    Byte mSTAT;

    // pointer to the CPU's MMU
    MMU* mMMU;

    Byte mInternalWindowCounter;

    void renderTile(DoubleByte tileMapAddr, Byte scx, Byte startingXPixel = 0, Byte endingXPixel = 7);
    void renderSprites();
    void renderBackground();
    void renderWindow();

    // checks for the LY = LYC stat interrupt
    void checkLycCoincidence();

public:
    void init(MMU* mmu);
    void tick(int ticks);
};