cmake_minimum_required(VERSION 3.0.0)
project(Hermes VERSION 0.1.0)

include(CTest)
enable_testing()

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
              src/InterruptHandler.h
              src/InterruptHandler.cpp
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
