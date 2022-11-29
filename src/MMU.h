#pragma once

#include "Constants.h"

/* 
    the memory management unit (MMU) struct is responsible for handling all the memory of the cartridge
    it uses a union so that we can reference the memory using different names for convienience
*/
struct MMU
{
    union 
    {
        // 0x10000 = 65536, which is the maximum address that the gameboy can use
        Byte memory[0x10000];

        // the bios takes up 0x100 bytes of instructions. it fills the memory right at the start of the cartridge
        Byte bios[0x100];

        // this structure contains the various subsets of memory found in the gameboy's cartridge
        // more info can be found here: http://gameboy.mongenel.com/dmg/asmmemmap.html
        struct 
        {
            Byte romBank0[0x4000];
            Byte romBank1[0x4000];
            Byte virtualRam[0x2000];
            Byte externalRam[0x2000];
            Byte workingRam[0x2000];
            Byte workingRamShadow[0x1DFF];
            Byte oam[0xA0];
            Byte empty[0x60];
            Byte io[0x40];
            Byte ppu[0x40];
            Byte zeroPageRam[0x80];
        };
    };

    void init();

    Byte readByte(DoubleByte addr);
    DoubleByte readDoubleByte(DoubleByte addr);

    void writeByte(DoubleByte addr, Byte val);
    void writeDoubleByte(DoubleByte addr, DoubleByte val);
};

// the bios is loaded into the cartridge's memory. place is already allocated for it in Rom Bank 0s