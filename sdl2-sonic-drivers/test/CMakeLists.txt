cmake_minimum_required (VERSION 3.15)

find_package(GTest 1.11 CONFIG REQUIRED)
include(GoogleTest)
include(CMakeParseArguments)

macro(macro_test) # ${ARGN} link libraries
    
    set(prefix TEST)
    #set(flags DEFAULT)
    set(singleValues EXE)
    set(multiValues FILES FIXTURES LINKS_PRIVATE)
    
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("EXE: ${TEST_EXE}")
    #message("FILES: ${TEST_FILES}")
    #message("FIXTURES: ${TEST_FIXTURES}")
    #message("LINKS_PRIVATE: ${TEST_LINKS_PRIVATE}")
    add_executable (${TEST_EXE} ${TEST_FILES})
    target_include_directories(${TEST_EXE} PUBLIC 
        ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
    )
    #target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS})
    target_link_libraries(${TEST_EXE} PRIVATE GTest::gtest GTest::gmock)
    add_custom_command(TARGET ${TEST_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:${TEST_EXE}>
    )

    set(fixtures ${TEST_FIXTURES})
    list(LENGTH fixtures num_fixtures)
    unset(fixtures)
    if(num_fixtures EQUAL 0)
        message("${TEST_EXE}: No fixtures declared")
    else()
        message("${TEST_EXE} Fixtures: ${TEST_FIXTURES}")
        foreach(fixture_i ${TEST_FIXTURES})
            message("copy fixture ${fixture_i}")
            add_custom_command(TARGET ${TEST_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${CMAKE_CURRENT_SOURCE_DIR}/${fixture_i}
                        ${CMAKE_CURRENT_BINARY_DIR}/${fixture_i}
            )
        endforeach()
    endif()

    target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS_PRIVATE})

    gtest_discover_tests(${TEST_EXE})
endmacro()

macro_test(
    EXE TestWaveGenerators
    FILES "TestWaveGenerators.cpp"
)

macro_test(
    EXE TestAlgorithms
    FILES "utils/TestAlgorithms.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestPCSpeaker
    FILES "hardware/TestPCSpeaker.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestEndianness
    FILES "utils/TestEndianness.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestFile
    FILES "files/TestFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestADLFile
    FILES "files/westwood/TestADLFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES 
        "fixtures/EOBSOUND.ADL" # v1
        "fixtures/DUNE19.ADL"   # v2
        "fixtures/DUNE0.ADL"    # v2
        "fixtures/LOREINTR.ADL" # v3
)

macro_test(
    EXE TestMIDFile
    FILES "files/TestMIDFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES "fixtures/MI_intro.mid"
)

macro_test(
    EXE TestSDL2Mixer
    FILES "audio/TestSDL2Mixer.cpp"
    LINKS_PRIVATE 
        sonic-drivers-static
        SDL2::SDL2_mixer
        ${LIB_SDL2} 
        ${LIB_SDL2main}
)

macro_test(
    EXE TestMame
    FILES "hardware/opl/scummvm/mame/TestMame.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES "fixtures/mame-22050-16bit-440Hz.dat"
)

macro_test(
    EXE TestDOSBox
    FILES "hardware/opl/scummvm/dosbox/TestDOSBox.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES
        #"fixtures/mame-22050-16bit-440Hz.dat"
)

macro_test(
    EXE TestNuked
    FILES "hardware/opl/scummvm/nuked/TestNuked.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES
        #"fixtures/mame-22050-16bit-440Hz.dat"
)

macro_test(
    EXE TestWoodyOPL
    FILES "hardware/opl/woody/TestWoodyOPL.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES
        #"fixtures/mame-22050-16bit-440Hz.dat"
)

macro_test(
    EXE TestConfig
    FILES 
        "hardware/opl/scummvm/TestConfig.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestADLDriver
    FILES "drivers/westwood/TestADLDriver.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

##### future tests ###
macro_test(
    EXE TestIFFFile
    FILES "files/TestIFFFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestXMIFile
    FILES "files/miles/TestXMIFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES 
        "fixtures/header.xmi"
        "fixtures/header_no_xdir.xmi"
        "fixtures/AIL2_14_DEMO.XMI"
)


macro_test(
    EXE TestXMidi
    FILES "drivers/miles/TestXMidi.cpp"
    LINKS_PRIVATE sonic-drivers-static
)

macro_test(
    EXE TestAILFileDriver
    FILES "files/miles/TestAILDriverFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES 
        "fixtures/ADLIB.ADV"
        "fixtures/ADLIBG.ADV"
        #"fixutres/ALGDIG.ADV"
        "fixtures/MT32MPU.ADV"
        #"fixutres/PASDIG.ADV"
        #"fixutres/PASFM.ADV"
        #"fixutres/PASOPL.ADV"
        "fixtures/PCSPKR.ADV"
        #"fixutres/SBDIG.ADV"
        "fixtures/SBFM.ADV"
        "fixtures/SBP1FM.ADV"
        "fixtures/SBP2FM.ADV"
        #"fixutres/SBPDIG.ADV"
        "fixtures/TANDY.ADV"
)

macro_test(
    EXE TestWAVFile
    FILES "files/TestWAVFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES
        "fixtures/Wav_868kb.wav" # format 1
)

macro_test(
    EXE TestVOCFile
    FILES "files/TestVOCFile.cpp"
    LINKS_PRIVATE sonic-drivers-static
    FIXTURES
        "fixtures/VSCREAM1.VOC"
        "fixtures/DUNE.VOC"
)
