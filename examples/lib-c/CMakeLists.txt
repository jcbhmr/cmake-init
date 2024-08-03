cmake_minimum_required(VERSION 3.29)

# Project
project(
  lib-c
  VERSION 0.1.0
  LANGUAGES C)

# Project dependencies
include(FetchContent)


# Libraries
add_library(lib-c)
target_sources(lib-c PRIVATE src/lib.cpp)
target_include_directories(lib-c PUBLIC include)
target_compile_features(lib-c PRIVATE c_std_23)


# Testing
include(CTest)
if(BUILD_TESTING)
  # TODO
endif()

# Installation info
include(GNUInstallDirs)
include(CPack)
install(TARGETS lib-c)

# Tasks
add_custom_target(
  format
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=format
    -DTASK_CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DTASK_CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
