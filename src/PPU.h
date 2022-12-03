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
        OBJ_SIZE               = 1 << 2,
        BG_TILE_MAP            = 1 << 3,
        BG_AND_WINDOW_TILE_MAP = 1 << 4,
        WINDOW_ENABLE          = 1 << 5,
        WINDOW_TILE_MAP_AREA   = 1 << 6,
        LCD_ENABLE             = 1 << 7
    };

    // an enum containing the various meanings of the different flags that can be set in the sprite attributes register
    // note that this enum starts at bit 7!
    enum SPRITE_ATTRIBUTES
    {
        BG_WINDOW_DRAWN_OVER = 1 << 7, // 0 = draw sprites over bg and window, 1 = draw colours ids 1, 2, and 3 (not 0) of the bg and window over the sprite
        Y_FLIP               = 1 << 6, // 0 = no flip, 1 = flip vertically
        X_FLIP               = 1 << 5, // 0 = no flip, 1 = flip horizontally
        S_PALLETE              = 1 << 4  // 0 = 0xFF48, 1 = 0xFF49
    };

    // an enum containing the states that the PPU can be in
    enum PPU_STATE
    {
        SEARCH_OAM,
        PUSH_PIXELS,
        HBLANK,
        VBLANK,
    };

    // an enum containing the states that the PPU's fetcher can be in
    enum FETCH_STATE
    {
        READ_TILE_ID,
        READ_TILE_0_DATA,
        READ_TILE_1_DATA,
        PUSH_TO_FIFO,
    };

    // pointer to the MMU's memory address at 0xFF40: the LCDC (LCD control) register
    Byte* mLCDC;

    // holds the current state of the PPU (i.e., what it is doing at any given moment)
    PPU_STATE mState;

    // holds the current state of the PPU's fetcher (i.e., what it is doing at any given moment)
    FETCH_STATE mFetchState;

    // holds the current memory address offset of the current tile being read in
    DoubleByte mTileMapRowAddr;

    // holds the x, y position of the tilemap
    int x, ly;
    
    // holds the number of ticks that the ppu has counted
    int mPPUTicks;

    // the tile index is the x-value of the tile the fetcher is fetching. for instance, the first tile from the left has a tile ID
    // of 0, while the fourteenth tile has an index value of 14
    Byte mTileIndex;

    // the tile ID is the value stored at the start of the OAM memory + tileIndex bytes
    DoubleByte mTileID;

    // the tile line defines which of the 8 rows in each tile we want to look at 
    Byte mTileLine;

    // holds the pixel data for a given byte of a tile's row
    Byte mPixelDataBuffer;

    // temporarily holds the the data for a row of pixels in a tile
    Byte mPixelData[8];

    // contains the window of the emulator
    Display mDisplay;

    void renderTile(MMU* mmu);
    void renderSprites(MMU* mmu);

public:

    void init(MMU* mmu);
    void tick(int ticks, MMU* mmu);
};