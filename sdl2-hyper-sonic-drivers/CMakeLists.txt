cmake_minimum_required (VERSION 3.21)

find_package(SDL2 CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DEBUG_POSTFIX d)

configure_file("version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/version.h")

if(WITH_SDL2_STATIC)
    set(LIB_SDL2 SDL2::SDL2-static)
else()
    set(LIB_SDL2 SDL2::SDL2)
endif()

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

########################### Static Lib #######################################
set(LIB_NAME hyper-sonic-drivers-static)
set(LIB_CONF ${LIB_NAME}-config)
set(LIB_VER ${LIB_CONF}-version)
include(CMakePackageConfigHelpers)

add_library(${LIB_NAME} STATIC)
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(
    TARGETS ${LIB_NAME}
    EXPORT ${LIB_CONF}
    DESTINATION static/lib
    PUBLIC_HEADER DESTINATION static/include
)

## TODO: this is ok, but some includes files i don't want to be "public",
##       so it looks like it must be built manually..
# try defining PUBLIC_HEADER than do an install target (it is at line 31)
install(DIRECTORY
    src/
    DESTINATION static/include
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
)

## NOTE: very error prone in this way...
## TODO: must create an include directory and "duplicate" those .hpp files
##       and declaring the classes that are not included and remove to include them
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/SDLMixerManager.hpp
#    DESTINATION "static/include/audio/scummvm"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/MIDI.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/Sound.hpp
#    DESTINATION "static/include/audio"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/opl/banks/OP2Bank.hpp
#DESTINATION "static/include/audio/opl/banks"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/PCMDriver.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/MIDDriver.hpp
#DESTINATION "static/include/drivers"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/westwood/ADLDriver.hpp
#DESTINATION "static/include/drivers/westwood"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/Device.hpp
#DESTINATION "static/include/drivers/midi"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Native.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/ScummVM.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Adlib.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/SbPro2.hpp
#DESTINATION "static/include/drivers/midi/devices"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/WAVFile.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/VOCFile.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/GetMIDI.hpp
#DESTINATION "static/include/files"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/westwood/ADLFile.hpp
#DESTINATION "static/include/files/westwood"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/MUSFile.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/OP2File.hpp
#DESTINATION "static/include/files/dmx"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/XMIFile.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/AILDriverFile.hpp
#DESTINATION "static/include/files/miles"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OPL.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OPLFactory.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OplType.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OplEmulator.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/Timer.hpp
#DESTINATION "static/include/hardware/opl"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/MameOPL3.hpp
#DESTINATION "static/include/hardware/opl/mame"
#)

#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/dosbox/DosBoxOPL.hpp
#DESTINATION "static/include/hardware/opl/scummvm/dosbox"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/mame/MameOPL2.hpp
#DESTINATION "static/include/hardware/opl/scummvm/mame"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/nuked/NukedOPL3.hpp
#DESTINATION "static/include/hardware/opl/scummvm/nuked"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/EmulatedOPL.hpp
#
#DESTINATION "static/include/hardware/opl/scummvm"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/WoodyOPL.hpp
#DESTINATION "static/include/hardware/opl/woody"
#)

#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/PCSpeaker.hpp
#
#DESTINATION "static/include/hardware"
#)
#
#install(FILES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/algorithms.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/constants.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/endianness.hpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/opl.hpp
#DESTINATION "static/include/utils"
#)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

export(TARGETS ${LIB_NAME}
    FILE ${CMAKE_BINARY_DIR}/${LIB_NAME}.cmake
    NAMESPACE HyperSonicDrivers::
)

install(EXPORT ${LIB_CONF}
    DESTINATION static/cmake
    NAMESPACE HyperSonicDrivers::
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
    DESTINATION static/cmake
)

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:${LIB_NAME}>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:static/include>
#PRIVATE
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_sources(${LIB_NAME} PRIVATE
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/IMixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/mixer/Channel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/sdl2/Mixer.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/MIDI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/midi/MIDITrack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/Sound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/streams/SoundStream.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/converters/IRateConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/converters/bitsConverter.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/scummvm/Timestamp.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/opl/banks/OP2Bank.cpp
    # --- #

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/Adlib.cpp
    
    # --- # 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/westwood/ADLDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/PCMDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/MIDDriver.cpp
   
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/Native.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_BASE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibPart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibPercussionChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/ScummVM.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplVoice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/Opl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/Adlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/SbPro.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/devices/SbPro2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/opl/OplWriter.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/IFFFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/westwood/ADLFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/MIDFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/dmx/MUSFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/dmx/OP2File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/miles/XMIFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/miles/AILDriverFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/RIFFFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/IPCMFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/WAVFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/files/VOCFile.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/OPLFactory.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/Chip.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/EmulatedOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/dosbox/DosBoxOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/dosbox/dbopl/dbopl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/mame/MameOPL2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/mame/mame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/nuked/NukedOPL3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/scummvm/nuked/opl3.c
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/ymfm/ymfm_adpcm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/ymfm/ymfm_fm.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/ymfm/ymfm_opl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/ymfm/ymfm_pcm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/MameOPL3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/mame/opl3/ymf262.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/woody/WoodyOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/woody/OPLChip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/woody/SurroundOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/woody/WoodyEmuOPL.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/PCSpeaker.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/algorithms.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/constants.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/endianness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/opl.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/ILogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/utils/sdl2/Logger.cpp
    # --- #
)

target_link_libraries(${LIB_NAME} PUBLIC ${LIB_SDL2} RtMidi::rtmidi)
if(NOT (WITH_SDL2_STATIC))
    install(FILES
        $<TARGET_FILE:SDL2::SDL2>
        DESTINATION static/lib
    )
endif()

if(BUILD_EXAMPLES)
    if(NOT (WITH_SDL2_STATIC))
    install(FILES
        $<TARGET_FILE:SDL2::SDL2>
        DESTINATION "static/examples"
    )
    endif()
endif()

unset(LIB_NAME)
unset(LIB_CONF)
unset(LIB_VER)
##############################################################################

# Add source to this project's executable.
add_executable(sdl2-hyper-sonic-drivers "sdl2-hyper-sonic-drivers.cpp")
#target_sources(sdl2-sonic-drivers PUBLIC
    # --- #
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/IFFFile.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/XMIFile.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.cpp
#)

target_link_libraries(sdl2-hyper-sonic-drivers PRIVATE  ${LIB_SDL2main} hyper-sonic-drivers-static)

if(MSVC)
# @see https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=vs-2019
    #add_compile_options(#/WX
    #$<$<CONFIG:DEBUG>:"/W4">
      #  $<$<CONFIG:RELEASE>:/O2 /Ot /GA /GL /Gw>
    #)
   
    ## use static library ...
    ##add_link_options($<$<CONFIG:Debug>:"/DEBUG">)
endif()

if(BUILD_TESTING)
    add_subdirectory("test")
endif()

if (BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
