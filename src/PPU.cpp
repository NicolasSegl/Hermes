#include <iostream>

#include "PPU.h"

/* memory offsets */

// vram offsets
const DoubleByte TILE_MAP_0_OFFSET   = 0x9800;
const DoubleByte TILE_MAP_1_OFFSET   = 0x9C00;
const DoubleByte VRAM_BLOCK_0_OFFSET = 0x8000;
const DoubleByte VRAM_BLOCK_1_OFFSET = 0x8800;
const DoubleByte VRAM_BLOCK_2_OFFSET = 0x9000;

// high ram register offsets
const DoubleByte LCDC_OFFSET      = 0xFF40;
const DoubleByte LY_OFFSET        = 0xFF44;
const DoubleByte SPRITE_DATA_OFFSET = 0xFE00;

// x/y position registers
const DoubleByte SCROLL_Y_OFFSET = 0xFF42;
const DoubleByte SCROLL_X_OFFSET = 0xFF43;
const DoubleByte WINDOW_Y_OFFSET = 0xFF4A;
const DoubleByte WINDOW_X_OFFSET = 0xFF4B;

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

    // point the LCDC pointer to the correct place in ram memory
    mLCDC = &mmu->ramMemory[LCDC_OFFSET - RAM_OFFSET];
}

void PPU::renderTile(MMU* mmu, DoubleByte tileMapAddr, Byte scx)
{
    Byte tileNum = scx / 8 + mTileIndex;
    if (tileNum >= 32)
        tileNum -= 32;

    // reading the ID of the tile consists of reading into the OAM and indexing the number of tiles pushed to FIFO so far in
    mTileID = mmu->readByte(tileMapAddr + tileNum);

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

    // determine which addressing mode to use based on if bit 4 of the LCDC register is set
    if (*mLCDC & BG_AND_WINDOW_TILE_MAP)
        mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_0_OFFSET + (mTileID * 16) + mTileLine * 2);
    else
    {
        if (mTileID < 128)
            mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_2_OFFSET + (mTileID * 16) + mTileLine * 2);
        else
            mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_1_OFFSET + ((mTileID - 128) * 16) + mTileLine * 2);
    }

    // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
    for (int bit = 0; bit < 8; bit++)
        mPixelData[bit] = (mPixelDataBuffer >> bit) & 1;

    // if we are reading the second of the two bytes that each tile takes up, then index an extra 1 byte
    if (*mLCDC & BG_AND_WINDOW_TILE_MAP)
        mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_0_OFFSET + (mTileID * 16) + mTileLine * 2 + 1);
    else
    {
        if (mTileID < 128)
            mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_2_OFFSET + (mTileID * 16) + mTileLine * 2 + 1);
        else
            mPixelDataBuffer = mmu->readByte(VRAM_BLOCK_1_OFFSET + ((mTileID - 128) * 16) + mTileLine * 2 + 1);
    }

    // iterate over all the bits in the buffer and set the values of the pixel data to 1 or 0
    for (int bit = 0; bit < 8; bit++)
        mPixelData[bit] += ((mPixelDataBuffer >> bit) & 1) << 1;

    // draw them in reverse order because bits are read from right to left, and 
    // we set the values of mPixelData with the most significant bits being on the right, and not the left!
    for (int bit = 7; bit >= 0; bit--)
    {
        if (x + scx % 8 >= 0)
            mDisplay.blitBG(x - scx % 8, ly, mPixelData[bit]);
        x++;
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
        Byte ypos = mmu->readByte(SPRITE_DATA_OFFSET + index) - 16 + mmu->readByte(SCROLL_Y_OFFSET);

        // if the sprite should be drawn on this scanline 
        if (tileline >= ypos && tileline < ypos + 8) // each sprite is 8 pixels high (for now this is all that is supported)
        {
            // read in the xpos (subtracting 8 from this value is necessary to get the proper xpos due to how the gameboy lays out its pixels)
            Byte xpos = mmu->readByte(SPRITE_DATA_OFFSET + index + 1) - 8;

            Byte tileLocation = mmu->readByte(SPRITE_DATA_OFFSET + index + 2);

            // read in the attributes of the sprite
            Byte attributes = mmu->readByte(SPRITE_DATA_OFFSET + index + 3);

            // this variable stores the row number of the sprite we're drawing. i.e., which row we're going to draw from the sprite's 8 pixel height
            int spriteLine = tileline - ypos;

            spriteLine *= 2; // 2 bytes for the 8 pixels

            DoubleByte spriteDataAddr = VRAM_BLOCK_0_OFFSET + tileLocation * 16 + spriteLine;
            Byte pixelData0 = mmu->readByte(spriteDataAddr);
            Byte pixelData1 = mmu->readByte(spriteDataAddr + 1);

            // draw the sprite right to left if the sprite is flipped horizontally
            if (attributes & X_FLIP)
                for (int pixel = 7; pixel >= 0; pixel--)
                {
                    // calculate the colour data of the pixel by combining the bits set in pixelData0 and the bits set in pixelData1 (1 bit of the
                    // colour data is stored in each buffer)
                    Byte colourData = ((pixelData0 >> pixel) & 1) | (((pixelData1 >> pixel) & 1) << 1);
                    Byte pixelX = xpos + pixel;

                    // don't draw the pixel if it is transparent 
                    if (colourData && pixelX < 160)
                        mDisplay.blitSprite(pixelX, ly, colourData, attributes & S_PALLETE, attributes & BG_WINDOW_DRAWN_OVER);
                }
            else
                for (int pixel = 0; pixel < 8; pixel++)
                {
                    Byte colourData = ((pixelData0 >> pixel) & 1) | (((pixelData1 >> pixel) & 1) << 1);
                    Byte pixelX = xpos + 8 - pixel;

                    if (colourData && pixelX < 160)
                        mDisplay.blitSprite(pixelX, ly, colourData, attributes & S_PALLETE, attributes & BG_WINDOW_DRAWN_OVER);
                }
        }
    }
}

void PPU::tick(int ticks, MMU* mmu)
{
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
            if (*mLCDC & BG_TILE_MAP)
                mBgTileMapRowAddr = TILE_MAP_1_OFFSET + Byte(ly + mmu->readByte(SCROLL_Y_OFFSET)) / 8 * 32;
            else
                mBgTileMapRowAddr = TILE_MAP_0_OFFSET + Byte(ly + mmu->readByte(SCROLL_Y_OFFSET)) / 8 * 32;

            // ly - window_y because we want to read the tile numbers as though the top of the window is 0
            // this means we would want to read right at 0x9800 or 0x9c00 
            if (*mLCDC & WINDOW_TILE_MAP)
                mWindowTileMapRowAddr = TILE_MAP_1_OFFSET + Byte(ly - mmu->readByte(WINDOW_Y_OFFSET)) / 8 * 32;
            else
                mWindowTileMapRowAddr = TILE_MAP_0_OFFSET + Byte(ly - mmu->readByte(WINDOW_Y_OFFSET)) / 8 * 32;

            /* initialize the values for the fetcher */

            // set the tile index to the leftmost tile
            mTileIndex = 0;

            // get which row of the tile we're looking at (can be any number from 0-7 for all 8 pixels of the tile's height)
            mTileLine = Byte(ly + mmu->readByte(SCROLL_Y_OFFSET)) % 8;

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
                for (int tile = 0; tile < 20; tile++)
                    renderTile(mmu, mBgTileMapRowAddr, mmu->readByte(SCROLL_X_OFFSET));

            if (*mLCDC & WINDOW_ENABLE)
                // if this scanline is on the same row or underneath the row that the window has its upper left corner on 
                if (ly >= mmu->readByte(WINDOW_Y_OFFSET))
                {
                    // reset the tile index and the x position of the pixel being looked at (i.e. go all the way back to the left side of the screen)
                    x = mmu->readByte(WINDOW_X_OFFSET) - 7;
                    mTileIndex = 0;
                    mTileLine = Byte(ly - mmu->readByte(WINDOW_Y_OFFSET)) % 8;

                    // we subtract xPos / 8 here because we do not want to always draw all 20 tiles, for instance
                    // in the case that the window 
                    for (int tile = x / 8; tile < 20; tile++)
                        renderTile(mmu, mWindowTileMapRowAddr, 0);
                }

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
                    mDisplay.drawFrame();

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