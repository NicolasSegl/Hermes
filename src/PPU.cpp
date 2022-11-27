#include <iostream>

#include "PPU.h"

// constants
const DoubleByte OAM_OFFSET  = 0x9800;
const DoubleByte LY_OFFSET   = 0xFF44;
const DoubleByte VRAM_OFFSET = 0x8000;

// initialize default values for the PPU
void PPU::init()
{
    x         = 0;
    ly        = 0;
    mPPUTicks = 0;

    // set the states
    mFetchState = READ_TILE_ID;
    mState      = SEARCH_OAM;
}

void PPU::renderScanline()
{

}

void PPU::tickFetcher(MMU* mmu)
{
    // a simple counter to control fetching pixels' speed. pixels are fetched at half the speed of the PPU
    static Byte ticks = 0;
    ticks++;

    // see if two ticks have gone by (which would mean it is running at half the tickrate of the PPU)
    if (ticks < 2)
        return;
    
    // reset the ticks variable
    ticks = 0;

    // decide what the fetcher should be doing
    switch (mFetchState)
    {
        // reading the ID of the tile consists of reading into the OAM and indexing the number of tiles pushed to FIFO so far in
        case READ_TILE_ID:
            mTileID = mmu->readByte(OAM_OFFSET + mTileIndex);
            mFetchState = READ_TILE_0_DATA; // update fetch state
            break;

        /* 
            reading the data in tile's consists of finding the offset into the VRAM that we need to index

            index into the VRAM by an offset that gets us to the first row of the 8-pixel high tile
            because each tile's graphical data takes up 16 bytes of data (each of the 8 rows takes 2 bytes. 
            this is because each pixel needs 2 bits of data, each combination of bits resulting in either a
            black, dark gray, light gray, or white pixel)

            we multiply the tile ID by 16 to get the offset
            from said offset, index a further 2 bytes for each row we go down
        */
        case READ_TILE_0_DATA:
            mPixelDataBuffer = mmu->readByte(VRAM_OFFSET + (mTileID * 16) + mTileLine * 2);

            // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
            for (int bit = 0; bit < 8; bit++)
            {
                if ((mPixelDataBuffer >> bit) & 1)
                    std::cout << "non 0\n";
                mPixelData[bit] = (mPixelDataBuffer >> bit) & 1;
            }

            mFetchState = READ_TILE_1_DATA; // update fetch state
            break;

        // if we are reading the second of the two bytes that each tile takes up, then index an extra 1 byte
        case READ_TILE_1_DATA:
            mPixelDataBuffer = mmu->readByte(VRAM_OFFSET + (mTileID * 16) + mTileLine * 2 + 1);

            // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
            for (int bit = 8; bit < 16; bit++)
            {
                if ((mPixelDataBuffer >> bit % 8) & 1)
                    std::cout << "non 0\n";

                mPixelData[bit] = (mPixelDataBuffer >> bit % 8) & 1;
            }

            mFetchState = PUSH_TO_FIFO; // update fetch state
            break;

        case PUSH_TO_FIFO:
            // we should only push pixels to the FIFO when there is room for 8 pixels
            if (mPixelsFIFO.size() <= 8)
            {
                // push 8 pixels from our buffer onto the FIFO stack
                // note that we push them in reverse order because bits are read from right to left, and 
                // we set the values of mPixelData with the most significant bits being on the right, and not the left!
                for (int bit = 7; bit >= 0; bit--)
                {
                    mPixelsFIFO.push_back(mPixelData[bit]);
                }

                // move on to the next tile in the row
                mTileIndex++;
                mFetchState = READ_TILE_ID;
            }

            break;
    }
}

void PPU::tick(int ticks, MMU* mmu)
{
    mPPUTicks += ticks;
    
    switch (mState)
    {
        // during this stage of the PPU, we are searching in the object attribute memory (OAM)
        // looking for the offset into the pixel data that we'll have to use in order to get the 
        // proper pixels for the scanline
        case SEARCH_OAM:
            // on the original gameboy, it took the PPU 80 ticks to search the OAM
            if (mPPUTicks >= 80)
            {                
                // set the x-offset of the pixels we'll be fetching back to 0
                x = 0;

                // the ly ranges from 0-144, each tile in the tilemap is 8x8 pixels, so we divide by 8, and there are 32 tiles in a row, so we multiply by 32
                mTileMapRowAddr = OAM_OFFSET + ly / 8 * 32;

                /* initialize the values for the fetcher */
                // clear the FIFO vector
                mPixelsFIFO.clear();

                // set the tile index to the leftmost tile
                mTileIndex = 0;

                // get the row of the tile
                mTileLine = ly % 8;

                // switch to the next state
                mState = PUSH_PIXELS;
            }

            break;

        case PUSH_PIXELS:
            tickFetcher(mmu);

            x++;

            // TODO: pop the pixels off the fifo and on to some sort of screen

            // if we have traversed the entire width of the screen, then HBLANK
            if (x == 160)
                mState = HBLANK;
            
            break;
        
        case HBLANK:
            // each scanline takes 456 ticks to fully complete
            if (mPPUTicks >= 456)
            {
                // reset the number of PPU ticks, as we are now starting on a new line
                mPPUTicks = 0;

                // increment the yline, meaning that we are now looking at a different scanline
                ly++;

                // if the ly equals 144, then it has gone through the entirety of the screen (which has a height of 144 scanlines)
                if (ly == 144)
                {
                    mState = VBLANK;
                }
                else
                    mState = SEARCH_OAM; // if the ly does not equal 144, then we want to again search the OAM, and repeat this process
            }
            
            break;

        case VBLANK:
            // a vblank would take the gameboy 4560 ticks
            if (mPPUTicks >= 4560)
            {
                mmu->writeByte(LY_OFFSET, 144);
                ly = 0;
                mState = SEARCH_OAM;
            }

            break;

    }
}