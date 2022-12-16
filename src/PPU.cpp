#include <iostream>

#include "PPU.h"

// offsets
const DoubleByte TILE_MAP_0_OFFSET       = 0x9800;
const DoubleByte TILE_MAP_1_OFFSET = 0x9C00;
const DoubleByte VRAM_OFFSET      = 0x8000;
const DoubleByte BG_AND_WINDOW_TILE_DATA_OFFSET_1 = 0x8800;

const DoubleByte LCDC_OFFSET      = 0xFF40;
const DoubleByte SCROLL_Y_OFFSET  = 0xFF42;
const DoubleByte SCROLL_X_OFFSET  = 0xFF43;
const DoubleByte LY_OFFSET        = 0xFF44;
const DoubleByte SPRITE_DATA_OFFSET = 0xFE00;

// initialize default values for the PPU
void PPU::init(MMU* mmu)
{
    x         = 0;
    ly        = 0;
    mPPUTicks = 0;

    // set the states
    mFetchState = READ_TILE_ID;
    mState      = SEARCH_OAM;

    // initialize the display
    mDisplay.init();

    // point the LCDC pointer to the correct place in memory
    mLCDC = &mmu->memory[LCDC_OFFSET];
}

void PPU::renderTile(MMU* mmu)
{
    // reading the ID of the tile consists of reading into the OAM and indexing the number of tiles pushed to FIFO so far in
    mTileID = mmu->readByte(mTileMapRowAddr + mTileIndex);

    /* 
        reading the data in tile's consists of finding the offset into the VRAM that we need to index

        index into the VRAM by an offset that gets us to the first row of the 8-pixel high tile
        because each tile's graphical data takes up 16 bytes of data (each of the 8 rows takes 2 bytes. 
        this is because each pixel needs 2 bits of data, each combination of bits resulting in either a
        black, dark gray, light gray, or white pixel). the first byte of the tile data contains the first bit
        of each pixel's colour, and the second byte of the tile data contains the seconf bit of each pixel's colour
        this why when reading tile 1's data, we shift the data one to the left for any given element. this means
        that each byte stored in mPixelData will have two of its bits being used

        we multiply the tile ID by 16 to get the offset
        from said offset, index a further 2 bytes for each row we go down
    */

    // if (*mLCDC & BG_AND_WINDOW_TILE_MAP)
    // {
    //     mPixelDataBuffer = mmu->readByte(BG_AND_WINDOW_TILE_DATA_OFFSET_1 + ((Signedbyte)mTileID + 128) * 16 + mTileLine * 2);
    // }
    // else
        mPixelDataBuffer = mmu->readByte(VRAM_OFFSET + (mTileID * 16) + mTileLine * 2);

    // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
    for (int bit = 0; bit < 8; bit++)
        mPixelData[bit] = (mPixelDataBuffer >> bit) & 1;

// if (*mLCDC & (BG_AND_WINDOW_TILE_MAP))
//     {
//         mPixelDataBuffer = mmu->readByte(BG_AND_WINDOW_TILE_DATA_OFFSET_1 + ((Signedbyte)mTileID + 128) * 16 + mTileLine * 2 + 1);
//     }
//     else
        // if we are reading the second of the two bytes that each tile takes up, then index an extra 1 byte
        mPixelDataBuffer = mmu->readByte(VRAM_OFFSET + (mTileID * 16) + mTileLine * 2 + 1);

    // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
    for (int bit = 0; bit < 8; bit++)
        mPixelData[bit] += ((mPixelDataBuffer >> bit) & 1) << 1;

    // draw them in reverse order because bits are read from right to left, and 
    // we set the values of mPixelData with the most significant bits being on the right, and not the left!
    for (int bit = 7; bit >= 0; bit--)
    {
        x++;
        if (mPixelData[bit]) // only draw the pixel if it is not white
            mDisplay.blitBG(x, ly, mPixelData[bit]);
    }

    // move on to the next tile in the row
    mTileIndex++;
}

void PPU::renderSprites(MMU* mmu)
{
    // read in current line (out of the 256 pixels of height) that we are using to draw to the screen
    Byte tileline = ly + mmu->readByte(SCROLL_Y_OFFSET);

    for (int sprite = 0; sprite < 40; sprite++)
    {
        // calculate how far into the OAM we will need to index (each sprite takes up 4 bytes of memory)
        Byte index = 4 * sprite;

        // read in the ypos (subtracting 16 from this value is necessary to get the proper ypos due to how the gameboy lays out its pixels)
        Byte ypos = mmu->readByte(SPRITE_DATA_OFFSET + index) - 16;

        // read in the xpos (subtracting 8 from this value is necessary to get the proper xpos due to how the gameboy lays out its pixels)
        Byte xpos = mmu->readByte(SPRITE_DATA_OFFSET + index + 1) - 8;

        Byte tileLocation = mmu->readByte(SPRITE_DATA_OFFSET + index + 2);

        // read in the attributes of the sprite
        Byte attributes = mmu->readByte(SPRITE_DATA_OFFSET + index + 3);

        // if (ypos != 240)
        // {
        //     std::cout << "ypos: " << (int)ypos << std::endl;
        // }

        // if the sprite should be drawn on this scanline 
        if (tileline >= ypos && tileline < ypos + 8) // each sprite is 8 pixels high (for now this is all that is supported)
        {
            // this variable stores the row number of the sprite we're drawing. i.e., which row we're going to draw from the sprite's 8 pixel height
            int spriteLine = tileline - ypos;

            spriteLine *= 2; // 2 bytes for the 8 pixels

            DoubleByte spriteDataAddr = VRAM_OFFSET + tileLocation * 16 + spriteLine;
            Byte pixelData0 = mmu->readByte(spriteDataAddr);
            Byte pixelData1 = mmu->readByte(spriteDataAddr + 1);

            for (int pixel = 0; pixel < 8; pixel++)
            {
                // calculate the colour data of the pixel by combining the bits set in pixelData0 and the bits set in pixelData1 (1 bit of the
                // colour data is stored in each buffer)
                Byte colourData = ((pixelData0 >> pixel) & 1) | (((pixelData1 >> pixel) & 1) << 1);
                // don't draw the pixel if it is going to be white 
                //if (colourData)
                    mDisplay.blitSprite(xpos + 8 - pixel, ly, colourData, attributes & S_PALLETE);
            }
        }
    }
}

void PPU::tick(int ticks, MMU* mmu)
{
    // update any events the user may have dispatched to the display window
    mDisplay.handleEvents(mmu);

    // if the LCDC register does not have the LCD_ENABLE bit set, then return immediately (as the screen is supposed to be off)
    if (!(*mLCDC & LCD_ENABLE))
        return;

    mPPUTicks += ticks;

    switch (mState)
    {
        // during this stage of the PPU, we are searching in the object attribute memory (OAM)
        // looking for the offset into the pixel data that we'll have to use in order to get the 
        // proper pixels for the scanline
        case SEARCH_OAM:        
            /* 
                the ly ranges from 0 - 144, each tile in the tilemap is 8x8 pixels, so we divide by 8, and there are 32 tiles in a row, so we multiply by 32
                we convert the result of ly + scy to a byte, because we want to wrap back to the top if the row we were looking at would have
                exceeded 256px 
            */
            if (*mLCDC & (BG_TILE_MAP))
                mTileMapRowAddr = TILE_MAP_1_OFFSET + Byte((ly + mmu->readByte(SCROLL_Y_OFFSET))) / 8 * 32;
            else
                mTileMapRowAddr = TILE_MAP_0_OFFSET + Byte((ly + mmu->readByte(SCROLL_Y_OFFSET))) / 8 * 32;

            /* initialize the values for the fetcher */

            // set the tile index to the leftmost tile
            mTileIndex = 0;

            // get the row of the tile
            mTileLine = (ly + mmu->readByte(SCROLL_Y_OFFSET)) % 8;

            // switch to the next state
            mState = PUSH_PIXELS;

            break;

        // get all the pixels in the scanline and render them to the screen
        case PUSH_PIXELS:            
            // set x to the leftmost pixel (so we start rendering form the left side of the screen to the right)
            x = 0;

            // if bg is enabled
            // for 160 pixels / 8 pixels (per tile) = 20 iterations for 20 tiles
            if (*mLCDC & BG_ENABLE)
                for (int tile = 0; tile < 160 / 8; tile++)
                    renderTile(mmu);

            if (*mLCDC & SPRITE_ENABLE)
                renderSprites(mmu);

            // update state
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
                mmu->writeByte(LY_OFFSET, ly);

                // if the ly equals 144, then it has gone through the entirety of the screen (which has a height of 144 scanlines)
                if (ly == 144)
                {
                    // update the display
                    mDisplay.update();

                    // set the interupt flag for vblanking
                    mmu->writeByte(INTERRUPT_OFFSET, (Byte)Interrupts::VBLANK);

                    // update state
                    mState = VBLANK;
                }
                else
                    mState = SEARCH_OAM; // if the ly does not equal 144, then we want to again search the OAM, and repeat this process
            }
            
            break;

        case VBLANK:

            if (mPPUTicks >= 456)
            {
                ly++;

                // if the vblank is over, reset the PPU state machine
                if (ly > 153)
                {
                    ly = 0;
                    mState = SEARCH_OAM;
                }

                mmu->writeByte(LY_OFFSET, ly);

                mPPUTicks = 0;
            }

            break;
    };
}