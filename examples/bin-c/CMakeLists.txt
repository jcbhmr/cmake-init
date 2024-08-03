cmake_minimum_required(VERSION 3.29)

# Project
project(
  bin-c
  VERSION 0.1.0
  LANGUAGES C)

# Project dependencies
include(FetchContent)


# Binaries
add_executable(bin-c)
target_sources(bin-c PRIVATE src/main.cpp)
target_compile_features(bin-c PRIVATE c_std_23)


# Testing
include(CTest)
if(BUILD_TESTING)
  # TODO
endif()

# Installation info
include(GNUInstallDirs)
include(CPack)
install(TARGETS bin-c)

# Tasks
add_custom_target(
  format
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=format
    -DTASK_CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DTASK_CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
