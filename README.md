# Hermes
Hermes is a work-in-progress gameboy emulator written in C++. 

## Dependencies
Hermes uses SDL2 for the display. Note that on Windows SDL and cmake can be a bit of a hassle, so make sure that CMake can find the sdl2-config.cmake files for everything to build properly.

## Resources
Just a list of some helpful resources I've come across while working on Hermes
* https://cturt.github.io/cinoop.html
* https://imrannazar.com/Gameboy-Z80-Opcode-Map
* https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
* https://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM
* https://www.youtube.com/watch?v=HyzD8pNlpwI&t=3028s&ab_channel=media.ccc.de
* https://blog.tigris.fr/2019/09/15/writing-an-emulator-the-first-pixel/

## Building
Building Hermes is as simple as:
>git clone (url)<br/>
>cd Hermes</br>
>mkdir build<br/>
>cd build</br>
>cmake ..</br>
>make</br>
