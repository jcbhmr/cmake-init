cmake_minimum_required(VERSION 3.29)

# Project
project(
  bin-cxx
  VERSION 0.1.0
  LANGUAGES CXX)

# Project dependencies
include(FetchContent)


# Binaries
add_executable(bin-cxx)
target_sources(bin-cxx PRIVATE src/main.cpp)
target_compile_features(bin-cxx PRIVATE cxx_std_23)


# Testing
include(CTest)
if(BUILD_TESTING)
  # TODO
endif()

# Installation info
include(GNUInstallDirs)
include(CPack)
install(TARGETS bin-cxx)

# Tasks
add_custom_target(
  format
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=format
    -DTASK_CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DTASK_CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
