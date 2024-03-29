cmake_minimum_required(VERSION 3.0.0)
project(Hermes VERSION 0.1.0)

include(CTest)
enable_testing()

# if Cmake cannot find sdl2-config.cmake, uncomment the below line and change "PATH TO SDL2 DIRECTORY" with your computer's path to your sdl2 directory
# list(APPEND CMAKE_PREFIX_PATH "PATH TO SDL2 DIRECTORY")
find_package(SDL2 REQUIRED)
include_directories(Hermes ${SDL2_INCLUDE_DIRS})

add_executable(Hermes 
              src/main.cpp
              src/Cartridge.h
              src/Cartridge.cpp
              src/cbOpcodes.cpp
              src/Constants.h
              src/CPU.h
              src/CPU.cpp
              src/Display.h
              src/Display.cpp
              src/Emulator.h
              src/Emulator.cpp
              src/InputHandler.h
              src/InputHandler.cpp
              src/InterruptHandler.h
              src/InterruptHandler.cpp
              src/MemoryChips/MBC.h
              src/MemoryChips/MBC.cpp
              src/MemoryChips/MBC1.h
              src/MemoryChips/MBC1.cpp
              src/MemoryChips/MBC2.h
              src/MemoryChips/MBC2.cpp
              src/MemoryChips/MBC3.cpp
              src/MemoryChips/MBC3.h
              src/MemoryChips/MBC5.h
              src/MemoryChips/MBC5.cpp
              src/MemoryChips/MemoryChip.cpp
              src/MemoryChips/MemoryChip.h
              src/MemoryChips/ROMOnly.cpp
              src/MemoryChips/ROMOnly.h
              src/MMU.h
              src/MMU.cpp
              src/opcodes.cpp
              src/PPU.h
              src/PPU.cpp
              src/Registers.h
              src/Registers.cpp)

target_link_libraries(Hermes ${SDL2_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
