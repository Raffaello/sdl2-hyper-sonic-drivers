cmake_minimum_required (VERSION 3.8)

find_package(GTest 1.10 CONFIG REQUIRED)
include(GoogleTest)
include(CMakeParseArguments)

macro(macro_test) # ${ARGN} link libraries
    
    set(prefix TEST)
    #set(flags DEFAULT)
    set(singleValues EXE)
    set(multiValues FILES)
    
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("${EXE} - ${FILES}")
    #message("EXE: ${TEST_EXE}")
    #message("FILES: ${TEST_FILES}")
    #message("IS DEFAULT: ${TEST_DEFAULT}")
    #message("IS SNAPSHOT: ${TEST_SNAPSHOT}")
    #message("NEED SHARELIB: ${TEST_SHARELIB}")
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
        "../src/hardware/opl/woody/OPL.CPP"
        "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
        "../src/hardware/opl/woody/OPLChip.cpp"
)

macro_test(
    EXE TestSurroundOPL
    FILES "hardware/opl/woody/TestSurroundOPL.cpp"
        "../src/hardware/opl/woody/OPL.CPP"
        "../src/hardware/opl/woody/SurroundOPL.cpp"
        "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
        "../src/hardware/opl/woody/OPLChip.cpp"
)

macro_test(
    EXE TestEndianness
    FILES "utils/TestEndianness.cpp"
        "../src/utils/endianness.cpp"
)
