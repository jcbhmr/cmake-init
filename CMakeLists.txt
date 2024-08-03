cmake_minimum_required(VERSION 3.29)

# Project
project(
  cmake-init
  VERSION 1.0.0
  DESCRIPTION "🌱 The missing CMake project initializer"
  HOMEPAGE_URL "https://github.com/jcbhmr/cmake-init"
  LANGUAGES CXX)

# Project dependencies
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.2)
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.4.2)
FetchContent_Declare(
  subprocess
  GIT_REPOSITORY https://github.com/arun11299/cpp-subprocess.git
  GIT_TAG 4025693decacaceb9420efedbf4967a04cb028e7)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1)
FetchContent_Declare(
  inja
  GIT_REPOSITORY https://github.com/pantor/inja.git
  GIT_TAG v3.4.0)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

FetchContent_MakeAvailable(CLI11 subprocess json)

# FMT_OS doesn't work with cosmocc.
set(FMT_OS OFF)
FetchContent_MakeAvailable(fmt)

# spdlog uses fmt and so do we.
set(SPDLOG_FMT_EXTERNAL TRUE)
FetchContent_MakeAvailable(spdlog)
# cosmocc defaults to no exceptions.
target_compile_options(spdlog PRIVATE -fexceptions)

# inja uses nlohmann/json too.
set(INJA_USE_EMBEDDED_JSON FALSE)
# inja defaults to building tests & benchmarks.
set(INJA_BUILD_TESTS FALSE)
set(BUILD_BENCHMARK FALSE)
FetchContent_MakeAvailable(inja)

# Binaries
add_executable(cmake-init)
target_sources(cmake-init PRIVATE src/main.cpp)
target_compile_features(cmake-init PRIVATE cxx_std_23)
# cosmocc defaults to no exceptions.
target_compile_options(cmake-init PRIVATE -fexceptions)
target_link_libraries(cmake-init PRIVATE spdlog::spdlog fmt::fmt
                                             CLI11::CLI11 subprocess pantor::inja nlohmann_json::nlohmann_json)
# Installation info
include(GNUInstallDirs)
include(CPack)
install(TARGETS cmake-init)

# Testing
include(CTest)
if(BUILD_TESTING)
  add_test(NAME bin-c
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=test-bin-c
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME bin-cxx
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=test-bin-cxx
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME lib-c
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=test-lib-c
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME lib-cxx
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=test-lib-cxx
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Tasks
add_custom_target(
  format
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=format
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_target(
  lint
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=lint
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_target(
  generate
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=generate
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
