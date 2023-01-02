#include <iostream>

#include "Emulator.h"

/*
    command line arguments:
    1st: name of the program (hermes)
    2nd: name of the ROM file that is going to be emulating (ending in .gb)
    3rd: optional argument, containing the save file to be loaded (if one should be loaded at all)
*/
int main(int argc, char** argv)
{
    Emulator em;
    em.setSaveFileName(argv[1]);

    if (argc == 2)
    {
        em.loadROM(argv[1]);
        em.run();
    }
    else if (argc == 3)
    {
        em.loadROM(argv[1]);
        em.loadSave(argv[2]);
        em.run();
    }
    else
        std::cout << "Invalid use of program! Usage is: hermes <ROM file> <optional: save file>\n";

    return 0;
}