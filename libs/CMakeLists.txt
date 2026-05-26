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
