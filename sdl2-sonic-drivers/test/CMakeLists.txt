cmake_minimum_required (VERSION 3.8)

find_package(GTest 1.10 CONFIG REQUIRED)
include(GoogleTest)

add_executable(TestWaveGenerators "TestWaveGenerators.cpp" "../src/softsynths/generators/generators.hpp")
target_include_directories(TestWaveGenerators PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestWaveGenerators PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestWaveGenerators POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestWaveGenerators>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestWaveGenerators>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestWaveGenerators>
)
gtest_discover_tests(TestWaveGenerators)


add_executable(TestPCSpeaker "hardware/TestPCSpeaker.cpp" "../src/hardware/PCSpeaker.cpp")
target_include_directories(TestPCSpeaker PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestPCSpeaker PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestPCSpeaker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestPCSpeaker>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestPCSpeaker>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestPCSpeaker>
)
gtest_discover_tests(TestPCSpeaker)

add_executable(
    TestOPLChip
    "hardware/opl/woody/TestOPLChip.cpp"
    "../src/hardware/opl/woody/OPLChip.cpp"
)
target_include_directories(TestOPLChip PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestOPLChip PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestOPLChip POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestOPLChip>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestOPLChip>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestOPLChip>
)
gtest_discover_tests(TestOPLChip)

add_executable(
    TestWoodyEmuOPL
    "hardware/opl/woody/TestWoodyEmuOPL.cpp"
    "../src/hardware/opl/OPL.CPP"
    "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
    "../src/hardware/opl/woody/OPLChip.cpp"
)
target_include_directories(TestWoodyEmuOPL PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestWoodyEmuOPL PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestWoodyEmuOPL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestWoodyEmuOPL>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestWoodyEmuOPL>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestWoodyEmuOPL>
)
gtest_discover_tests(TestWoodyEmuOPL)

add_executable(
    TestSurroundOPL
    "hardware/opl/woody/TestSurroundOPL.cpp"
    "../src/hardware/opl/OPL.CPP"
    "../src/hardware/opl/woody/SurroundOPL.cpp"
    "../src/hardware/opl/woody/WoodyEmuOPL.cpp"
    "../src/hardware/opl/woody/OPLChip.cpp"
)
target_include_directories(TestSurroundOPL PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestSurroundOPL PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestSurroundOPL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestSurroundOPL>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestSurroundOPL>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestSurroundOPL>
)
gtest_discover_tests(TestSurroundOPL)
