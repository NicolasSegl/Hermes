#include <iostream>

#include "Emulator.h"

/*
    command line arguments:
    1st: name of the program (hermes)
    2nd: name of the ROM file that is going to be emulating (ending in .gb)
*/
int main(int argc, char** argv)
{
    if (argc == 2)
    {
        Emulator em;
        em.run(argv[1]);
    }
    else
        std::cout << "Invalid use of program! Usage is: hermes <ROM file>\n";

    return 0;
}