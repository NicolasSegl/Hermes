#pragma once

// data types
typedef unsigned char Byte;
typedef signed char Signedbyte;
typedef unsigned short DoubleByte;

// gameboy screen dimensions
const Byte GAMEBOY_SCREEN_WIDTH  = 160;
const Byte GAMEBOY_SCREEN_HEIGHT = 144;

// memory offsets
const DoubleByte INTERRUPT_OFFSET = 0xFF0F;

// enum for the bits that are set for various interrupts
enum class Interrupts
{
    VBLANK   = 1 << 0,
    LCD_STAT = 1 << 1,
    TIMER    = 1 << 2,
    SERIAL   = 1 << 3,
    JOYPAD   = 1 << 4,
};