# CMakeList.txt : CMake project for sdl2-sonic-drivers, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-mixer CONFIG REQUIRED)
    
configure_file("version.h.in" "version.h")

# Add source to this project's executable.
add_executable(sdl2-sonic-drivers "sdl2-sonic-drivers.cpp")
target_sources(sdl2-sonic-drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/softsynths/generators/generators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hardware/PCSpeaker.cpp
)

target_include_directories(sdl2-sonic-drivers PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sdl2-sonic-drivers PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_mixer)
if(BUILD_TESTING)
    add_subdirectory("test")
endif()