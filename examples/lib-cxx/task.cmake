#!/usr/bin/env -S cmake -P
cmake_minimum_required(VERSION 3.29)

if(NOT TASK_NAME)
  message(FATAL_ERROR "no TASK_NAME")
endif()
if(TASK_SOURCE_DIR)
  set(CMAKE_CURRENT_SOURCE_DIR "${TASK_SOURCE_DIR}")
  set(CMAKE_SOURCE_DIR "${TASK_SOURCE_DIR}")
else()
  message(FATAL_ERROR "no TASK_SOURCE_DIR")
endif()
if(TASK_BINARY_DIR)
  set(CMAKE_CURRENT_BINARY_DIR "${TASK_BINARY_DIR}")
  set(CMAKE_BINARY_DIR "${TASK_BINARY_DIR}")
else()
  message(FATAL_ERROR "no TASK_BINARY_DIR")
endif()

function(task_format)
  file(
    GLOB_RECURSE
    c_cxx_files
    src/*.c
    src/*.cpp
    src/*.cc
    src/*.cxx
    src/*.h
    src/*.hxx
    src/*.hh
    src/*.hpp
    include/*.h
    include/*.hxx
    include/*.hh
    include/*.hpp
    test/*.c
    test/*.cpp
    test/*.cc
    test/*.cxx
    test/*.h
    test/*.hxx
    test/*.hh
    test/*.hpp
    examples/*.c
    examples/*.cpp
    examples/*.cc
    examples/*.cxx
    examples/*.h
    examples/*.hxx
    examples/*.hh
    examples/*.hpp)
  execute_process(COMMAND clang-format -i ${c_cxx_files} COMMAND_ECHO
                          STDERR )

  file(
    GLOB_RECURSE
    cmake_files
    cmake/*.cmake
    src/*CMakeLists.txt
    src/*.cmake
    test/*CMakeLists.txt
    test/*.cmake
    examples/*CMakeLists.txt
    examples/*.cmake)
  list(APPEND cmake_format_files CMakeLists.txt task.cmake)
  execute_process(COMMAND cmake-format -i ${cmake_files} COMMAND_ECHO
                          STDERR)
endfunction()

function(task_lint)
    file(GLOB_RECURSE all_files .github/* cmake/* src/* examples/* test/* docs/*)
    list(APPEND .gitignore CMakeLists.txt CMakePresets.json README.md task.cmake)
    execute_process(COMMAND codespell -w ${all_files} COMMAND_ECHO STDERR)

    # TODO
endfunction()

if(TASK_NAME STREQUAL format)
  task_format()
elseif(TASK_NAME STREQUAL lint)
    task_lint()
else()
  message(FATAL_ERROR "no such task: ${TASK_NAME}")
endif()
