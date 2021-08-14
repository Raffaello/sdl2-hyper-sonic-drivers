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
    #message("LINKS: ${TEST_LINKS}")
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

    target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS})

    gtest_discover_tests(${TEST_EXE})
endmacro()

macro_test(
    EXE TestWaveGenerators
    FILES "TestWaveGenerators.cpp" "../src/softsynths/generators/generators.hpp"
)

macro_test(
    EXE TestPCSpeaker
    FILES "hardware/TestPCSpeaker.cpp"
        "../src/hardware/PCSpeaker.cpp"
)

macro_test(
    EXE TestOPLChip
    FILES "hardware/opl/woody/TestOPLChip.cpp"
        "../src/hardware/opl/woody/OPLChip.cpp"
)

macro_test(
    EXE TestWoodyEmuOPL
    FILES "hardware/opl/woody/TestWoodyEmuOPL.cpp"
        "../src/hardware/opl/OPL.CPP"
        "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
        "../src/hardware/opl/woody/OPLChip.cpp"
)

macro_test(
    EXE TestSurroundOPL
    FILES "hardware/opl/woody/TestSurroundOPL.cpp"
        "../src/hardware/opl/OPL.CPP"
        "../src/hardware/opl/woody/SurroundOPL.cpp"
        "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
        "../src/hardware/opl/woody/OPLChip.cpp"
)

macro_test(
    EXE TestEndianness
    FILES "utils/TestEndianness.cpp"
        "../src/utils/endianness.cpp"
)

macro_test(
    EXE TestFile
    FILES "files/TestFile.cpp"
        "../src/files/File.cpp"
        "../src/utils/endianness.cpp"
)

macro_test(
    EXE TestIFFFile
    FILES "files/TestIFFFile.cpp"
        "../src/files/IFFFile.cpp"
        "../src/files/File.cpp"
        "../src/utils/endianness.cpp"
)

macro_test(
    EXE TestXMIFile
    FILES "files/TestXMIFile.cpp"
        "../src/files/XMIFile.cpp"
        "../src/files/IFFFile.cpp"
        "../src/files/File.cpp"
        "../src/utils/endianness.cpp"
    FIXTURES 
        "fixtures/header.xmi"
        "fixtures/header_no_xdir.xmi"
        "fixtures/AIL2_14_DEMO.XMI"
)

macro_test(
    EXE TestADLFile
    FILES "files/TestADLFile.cpp"
        "../src/files/ADLFile.cpp"
        "../src/files/File.cpp"
        "../src/utils/endianness.cpp"
    FIXTURES 
        "fixtures/EOBSOUND.ADL" # v1
        "fixtures/DUNE19.ADL"   # v2
        "fixtures/LOREINTR.ADL" # v3
)

macro_test(
    EXE TestADLDriver
    FILES "drivers/westwood/TestADLDriver.cpp"
        "../src/drivers/westwood/ADLDriver.cpp"
    LINKS_PRIVATE spdlog::spdlog
)
