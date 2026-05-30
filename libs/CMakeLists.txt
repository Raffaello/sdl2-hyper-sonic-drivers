include(FetchContent)

# fmt library (required for spdlog)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        12.1.0)

FetchContent_MakeAvailable(fmt)

# spdlog
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.17.0)

FetchContent_MakeAvailable(spdlog)
target_link_libraries(spdlog PUBLIC fmt::fmt)


# RtAudio
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE) # Disable RtAudio tests
set(OLD_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)
# set(RTAUDIO_CMAKE_DESTINATION ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)
# set(RTAUDIO_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # build static library
FetchContent_Declare(
  rtaudio
  GIT_REPOSITORY https://github.com/thestk/rtaudio.git
  GIT_TAG        e5f0774b2156082ec3db998bd6b2a94b66ade8ac #6.0.2
)

FetchContent_MakeAvailable(rtaudio)
set(CMAKE_INSTALL_PREFIX ${OLD_CMAKE_INSTALL_PREFIX})
unset(OLD_CMAKE_INSTALL_PREFIX)
include(copy_runtime_deps)
