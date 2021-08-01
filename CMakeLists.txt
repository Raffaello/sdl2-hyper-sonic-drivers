# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


project ("sdl2-sonic-drivers" VERSION 0.1.0 DESCRIPTION "SDL2 mixer Sonic Drivers for emulating old soundcards")
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
 message(STATUS "BIG_ENDIAN")
 add_compile_definitions(-DIS_BIG_ENDIAN)
else()
 message(STATUS "LITTLE_ENDIAN")
endif()


include(CTest)
# Include sub-projects.
add_subdirectory ("sdl2-sonic-drivers")
