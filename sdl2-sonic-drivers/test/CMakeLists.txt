cmake_minimum_required (VERSION 3.8)

find_package(GTest 1.10 CONFIG REQUIRED)
include(GoogleTest)


add_executable (TestWaveGenerators "TestWaveGenerators.cpp" "../src/softsynths/generators/generators.hpp")
target_include_directories(TestWaveGenerators PUBLIC 
    ${CMAKE_SOURCE_DIR}/sdl2-sonic-drivers/src
)
target_link_libraries(TestWaveGenerators PRIVATE GTest::gtest GTest::gmock)
add_custom_command(TARGET TestWaveGenerators POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:TestWaveGenerators>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:TestWaveGenerators>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:TestWaveGenerators>
)