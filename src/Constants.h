#pragma once

typedef unsigned char Byte;
typedef signed char Signedbyte;
typedef unsigned short DoubleByte;

// enum for the bits that are set for various interrupts
enum class Interrupts
{
    VBLANK = 1 << 0,
};