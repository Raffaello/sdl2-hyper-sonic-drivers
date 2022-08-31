cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog 1.9.0 CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)


configure_file("version.h.in" "version.h")

if(WITH_SDL2_STATIC)
    set(LIB_SDL2 SDL2::SDL2-static)
else()
    set(LIB_SDL2 SDL2::SDL2)
endif()

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

########################### Static Lib #######################################
set(LIB_NAME sonic-drivers-static)
set(LIB_CONF ${LIB_NAME}-config)
set(LIB_VER ${LIB_CONF}-version)
include(CMakePackageConfigHelpers)

add_library(${LIB_NAME} STATIC)
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(
    TARGETS ${LIB_NAME}
    EXPORT ${LIB_CONF}
    DESTINATION "static/lib"
    PUBLIC_HEADER DESTINATION "static/include"
)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/SDLMixerManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/MIDI.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/Sound.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/opl/banks/OP2Bank.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/westwood/ADLDriver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/PCMDriver.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/MIDDriver.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/Device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Native.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/ScummVM.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Adlib.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/westwood/ADLFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/MUSFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/OP2File.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/XMIFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/AILDriverFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/WAVFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/VOCFile.hpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OPL.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/dosbox/dosbox.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/mame/mame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/nuked/OPL.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/Config.hpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/EmulatedOPL.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/WoodyOPL.hpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/PCSpeaker.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/algorithms.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/constants.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/endianness.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/opl.hpp

    DESTINATION "static/include"
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

export(TARGETS ${LIB_NAME}
    FILE ${CMAKE_BINARY_DIR}/${LIB_NAME}.cmake
    #NAMESPACE sonic::
)

install(EXPORT ${LIB_CONF}
    DESTINATION static/cmake
    #NAMESPACE sonic::
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
    DESTINATION "static/cmake"
)

target_include_directories(${LIB_NAME} PUBLIC 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:${LIB_NAME}>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:"static/include">
)
target_sources(${LIB_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/DiskRendererMixerManager.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/MIDI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/midi/MIDITrack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/Sound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/streams/SoundStream.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/SDLMixerManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/Channel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/MixerImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/RateConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/scummvm/Timestamp.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/opl/banks/OP2Bank.cpp
    # --- # 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/westwood/ADLDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/PCMDriver.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/miles/XMidi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/MIDDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/miles/XMIParser.cpp
   
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Native.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/MidiDriver_BASE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/MidiDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/AdLibInstrument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/AdLibPart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/AdLibPercussionChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/scummvm/MidiDriver_ADLIB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/ScummVM.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/adlib/MidiDriver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/adlib/MidiChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/midi/devices/Adlib.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/IFFFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/westwood/ADLFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/MUSFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/dmx/OP2File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/XMIFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/miles/AILDriverFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/RIFFFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/WAVFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/files/VOCFile.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/dosbox/dosbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/dosbox/dbopl/dbopl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/mame/mame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/nuked/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/nuked/opl3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/scummvm/EmulatedOPL.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/ymfm/ymfm_adpcm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/ymfm/ymfm_fm.ipp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/ymfm/ymfm_opl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/ymfm/ymfm_pcm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/MameOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/mame/opl3/ymf262.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/WoodyOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/OPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/OPLChip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/SurroundOPL.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/opl/woody/WoodyEmuOPL.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/PCSpeaker.cpp
    # --- #
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/endianness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/algorithms.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/opl.cpp
)

target_link_libraries(${LIB_NAME} PUBLIC ${LIB_SDL2} spdlog::spdlog RtMidi::rtmidi)
if(NOT (WITH_SDL2_STATIC))
    install(FILES
        $<TARGET_FILE:SDL2::SDL2>
        DESTINATION static/lib
    )
endif()

unset(LIB_NAME)
unset(LIB_CONF)
unset(LIB_VER)
##############################################################################

# Add source to this project's executable.
add_executable(sdl2-sonic-drivers "sdl2-sonic-drivers.cpp")
#target_sources(sdl2-sonic-drivers PUBLIC
    #${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/miles/XMidi.cpp
    # --- #
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/IFFFile.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/XMIFile.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/files/MIDFile.cpp
#)

target_link_libraries(sdl2-sonic-drivers PRIVATE ${LIB_SDL2main} )
target_link_libraries(sdl2-sonic-drivers PRIVATE sonic-drivers-static)

if(BUILD_TESTING)
    add_subdirectory("test")
endif()

if (BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()
