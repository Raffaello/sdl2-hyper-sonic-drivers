# CMakeList.txt : CMake project for sdl2-sonic-drivers, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
#find_package(fmt CONFIG REQUIRED)
find_package(spdlog 1.9.0 CONFIG REQUIRED) # used in ADLDriver

configure_file("version.h.in" "version.h")


# Add source to this project's executable.
add_executable(sdl2-sonic-drivers "sdl2-sonic-drivers.cpp")
target_sources(sdl2-sonic-drivers PUBLIC
    #${CMAKE_CURRENT_SOURCE_DIR}/src/adl/sound_adlib.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/adl/format.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/adl/surroundopl.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/adl/woodyopl.cpp
    # ---- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/miles/XMidi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/endianness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/IFFFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/XMIFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/ADLFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.cpp

    # ---- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/PCSpeaker.cpp
    # ---- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/OPLChip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/WoodyEmuOpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/SurroundOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/westwood/woody/ADLDriver.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/EmulatedOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/mame/mame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/westwood/ADLDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/SDL2Mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/Timestamp.cpp



)

target_include_directories(sdl2-sonic-drivers PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sdl2-sonic-drivers PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_mixer)
target_link_libraries(sdl2-sonic-drivers PRIVATE spdlog::spdlog)
if(BUILD_TESTING)
    add_subdirectory("test")
endif()
