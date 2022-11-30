#pragma once

#include <vector>

#include "Constants.h"
#include "Display.h"
#include "FIFO.h"
#include "MMU.h"

// the PPU (picture processing unit) handles the graphics
class PPU
{
private:
    // an enum containing the various meanings of the different flags set in the LCD control register
    enum LCDC_BITS
    {
        BG_ENABLE,
        OBJ_ENABLE,
        OBJ_SIZE,
        BG_TILE_MAP,
        BG_AND_WINDOW_TILE_MAP,
        WINDOW_ENABLE,
        WINDOW_TILE_MAP_AREA,
        LCD_ENABLE
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

    // holds up to 16 pixels pushed onto or popped off when fetching and pushing pixels to the screen
    FIFO mPixelsFIFO;

    // contains the window of the emulator
    Display mDisplay;

    void tickFetcher(MMU* mmu);

public:

    void init(MMU* mmu);
    void tick(int ticks, MMU* mmu);
};