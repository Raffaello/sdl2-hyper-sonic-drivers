cmake_minimum_required (VERSION 3.21)

find_package(SDL2 CONFIG REQUIRED)
find_package(MT32emu CONFIG REQUIRED)

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

install(DIRECTORY
    test/
    DESTINATION static/test/include
    FILES_MATCHING
        PATTERN "*Mock.hpp"
)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/sdl2/Renderer.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/MIDI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/midi/MIDITrack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/PCMSound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/streams/PCMStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/streams/EmulatedStream.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/converters/IRateConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/converters/bitsConverter.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/scummvm/Timestamp.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/audio/opl/banks/OP2Bank.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/IDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/Opl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/Adlib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/SbPro.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/SbPro2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/devices/MT32.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/PCMDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/IMusicDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/MIDDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/IMidiDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/IMidiChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/IMidiChannelVoice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/westwood/ADLDriver.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/MidiDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibInstrument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/AdLibPercussionChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/scummvm/MidiDriver_ADLIB.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/opl/OplVoice.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/drivers/midi/mt32/MT32Driver.cpp

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/IHardware.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/OPLFactory.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/opl/Chip.cpp

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/mt32/MT32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HyperSonicDrivers/hardware/mt32/MT32ReportHandler.cpp
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

target_link_libraries(${LIB_NAME}
    PUBLIC 
        ${LIB_SDL2}
        MT32Emu::mt32emu
)
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
