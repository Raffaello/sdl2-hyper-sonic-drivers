cmake_minimum_required (VERSION 3.21)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


project ("sdl2-hyper-sonic-drivers" VERSION 0.13.4 DESCRIPTION "SDL2 based Hyper-Sonic Drivers for emulating old soundcards")
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
  message(STATUS "BIG_ENDIAN")
  add_compile_definitions(-DIS_BIG_ENDIAN)
else()
  message(STATUS "LITTLE_ENDIAN")
endif()

################################ Packages ####################################
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPACK_SYSTEM_NAME win64)
  else()
    set(CPACK_SYSTEM_NAME win32)
  endif()
endif()
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
include(CPack)

################################# Options ####################################
option(WITH_SDL2_STATIC "linking STATIC LIB with SDL2 STATIC" OFF)
include(CTest)
option(BUILD_EXAMPLES "build examples" ON)

##############################################################################

# Include sub-projects.
add_subdirectory ("sdl2-hyper-sonic-drivers")
