#include <CLI/CLI.hpp>
#include <cstdlib>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fstream>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <subprocess.hpp>
#include <unordered_map>
#include <vector>

auto main(int argc, char *argv[]) -> int;

namespace cmake_init {

enum class vcs { none, git };
auto to_string(cmake_init::vcs vcs) -> std::string;

enum class c_standard { c90, c99, c11, c17, c23 };
enum class cxx_standard { cxx98, cxx11, cxx17, cxx20, cxx23, cxx26 };
auto to_string(cmake_init::c_standard c_standard) -> std::string;
auto to_string(cmake_init::cxx_standard cxx_standard) -> std::string;
auto to_cmake_feature_string(cmake_init::c_standard c_standard) -> std::string;
auto to_cmake_feature_string(cmake_init::cxx_standard cxx_standard)
    -> std::string;

namespace filesystem {

auto read(const std::filesystem::path &path) -> std::string;
auto write(const std::filesystem::path &path, const std::string &data) -> void;
auto append(const std::filesystem::path &path, const std::string &data) -> void;

} // namespace filesystem

} // namespace cmake_init

auto cmake_init::to_string(cmake_init::vcs vcs) -> std::string {
  switch (vcs) {
  case cmake_init::vcs::none:
    return "none";
  case cmake_init::vcs::git:
    return "git";
  default:
    throw std::runtime_error("unknown cmake_init::vcs value");
  }
}

auto cmake_init::to_string(cmake_init::c_standard c_standard) -> std::string {
  switch (c_standard) {
  case cmake_init::c_standard::c90:
    return "c90";
  case cmake_init::c_standard::c99:
    return "c99";
  case cmake_init::c_standard::c11:
    return "c11";
  case cmake_init::c_standard::c17:
    return "c17";
  case cmake_init::c_standard::c23:
    return "c23";
  default:
    throw std::runtime_error("unknown cmake_init::c_standard value");
  }
}

auto cmake_init::to_string(cmake_init::cxx_standard cxx_standard)
    -> std::string {
  switch (cxx_standard) {
  case cmake_init::cxx_standard::cxx98:
    return "c++98";
  case cmake_init::cxx_standard::cxx11:
    return "c++11";
  case cmake_init::cxx_standard::cxx17:
    return "c++17";
  case cmake_init::cxx_standard::cxx20:
    return "c++20";
  case cmake_init::cxx_standard::cxx23:
    return "c++23";
  case cmake_init::cxx_standard::cxx26:
    return "c++26";
  default:
    throw std::runtime_error("unknown cmake_init::cxx_standard value");
  }
}

auto cmake_init::to_cmake_feature_string(cmake_init::c_standard c_standard)
    -> std::string {
  switch (c_standard) {
  case cmake_init::c_standard::c90:
    return "c_std_90";
  case cmake_init::c_standard::c99:
    return "c_std_99";
  case cmake_init::c_standard::c11:
    return "c_std_11";
  case cmake_init::c_standard::c17:
    return "c_std_17";
  case cmake_init::c_standard::c23:
    return "c_std_23";
  default:
    throw std::runtime_error("unknown cmake_init::c_standard value");
  }
}

auto cmake_init::to_cmake_feature_string(cmake_init::cxx_standard cxx_standard)
    -> std::string {
  switch (cxx_standard) {
  case cmake_init::cxx_standard::cxx98:
    return "cxx_std_98";
  case cmake_init::cxx_standard::cxx11:
    return "cxx_std_11";
  case cmake_init::cxx_standard::cxx17:
    return "cxx_std_17";
  case cmake_init::cxx_standard::cxx20:
    return "cxx_std_20";
  case cmake_init::cxx_standard::cxx23:
    return "cxx_std_23";
  case cmake_init::cxx_standard::cxx26:
    return "cxx_std_26";
  default:
    throw std::runtime_error("unknown cmake_init::cxx_standard value");
  }
}

auto cmake_init::filesystem::read(const std::filesystem::path &path)
    -> std::string {
  auto ifs = std::ifstream(path, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error(
        fmt::format("error opening {} for reading", path.string()));
  }
  auto oss = std::ostringstream(std::ios::binary);
  oss << ifs.rdbuf();
  return oss.str();
}

auto cmake_init::filesystem::write(const std::filesystem::path &path,
                                   const std::string &data) -> void {
  auto ofs = std::ofstream(path, std::ios::binary | std::ios::trunc);
  if (!ofs) {
    throw std::runtime_error(
        fmt::format("error opening {} for writing", path.string()));
  }
  ofs << data;
}

auto cmake_init::filesystem::append(const std::filesystem::path &path,
                                    const std::string &data) -> void {
  auto ofs = std::ofstream(path, std::ios::binary);
  if (!ofs) {
    throw std::runtime_error(
        fmt::format("error opening {} for writing", path.string()));
  }
  ofs << data;
}

auto main(int argc, char *argv[]) -> int {
  spdlog::cfg::load_env_levels();

  // All these newlines make the --help look prettier.
  auto app = CLI::App(std::string("cmake-init\n") +
                      "🌱 The missing CMake project initializer\n" +
                      "https://github.com/jcbhmr/cmake-init\n");

  cmake_init::vcs vcs;
  app.add_option("--vcs", vcs,
                 "Initialize a new repository for the given version control "
                 "system, overriding a global configuration.")
      ->transform(
          CLI::Transformer(std::unordered_map<std::string, cmake_init::vcs>{
              {"none", cmake_init::vcs::none}, {"git", cmake_init::vcs::git}}))
      ->default_val(cmake_init::vcs::none);

  bool bin;
  auto bin_opt =
      app.add_flag("--bin", bin, "Use a binary (application) template")
          ->default_val(true);

  bool lib;
  auto lib_opt =
      app.add_flag("--lib", lib, "Use a library template")->excludes(bin_opt);
  bin_opt->excludes(lib_opt);

  bool cxx;
  auto cxx_opt =
      app.add_flag("--cxx", cxx, "Use a C++ template")->default_val(true);

  bool c;
  auto c_opt = app.add_flag("--c", c, "Use a C template")->excludes(cxx_opt);
  cxx_opt->excludes(c_opt);

  cmake_init::c_standard c_standard;
  auto c_standard_opt =
      app.add_option("--c-standard", c_standard,
                     "Which edition of The C Standard to configure")
          ->transform(CLI::Transformer(
              std::unordered_map<std::string, cmake_init::c_standard>{
                  {"c90", cmake_init::c_standard::c90},
                  {"c99", cmake_init::c_standard::c99},
                  {"c11", cmake_init::c_standard::c11},
                  {"c17", cmake_init::c_standard::c17},
                  {"c23", cmake_init::c_standard::c23},
              }))
          ->default_val(cmake_init::c_standard::c23);

  cmake_init::cxx_standard cxx_standard;
  auto cxx_standard_opt =
      app.add_option("--cxx-standard", cxx_standard,
                     "Which edition of The C++ Standard to configure")
          ->transform(CLI::Transformer(
              std::unordered_map<std::string, cmake_init::cxx_standard>{
                  {"c++98", cmake_init::cxx_standard::cxx98},
                  {"c++11", cmake_init::cxx_standard::cxx11},
                  {"c++17", cmake_init::cxx_standard::cxx17},
                  {"c++20", cmake_init::cxx_standard::cxx20},
                  {"c++23", cmake_init::cxx_standard::cxx23},
                  {"c++26", cmake_init::cxx_standard::cxx26},
              }))
          ->excludes(c_standard_opt)
          ->default_val(cmake_init::cxx_standard::cxx23);
  c_standard_opt->excludes(cxx_standard_opt);

  std::string name;
  app.add_option(
         "--name", name,
         "Set the resulting package name, defaults to the directory name")
      ->default_val(std::filesystem::current_path().filename().string());

  CLI11_PARSE(app, argc, argv);

  if (std::filesystem::exists(".gitignore")) {
    cmake_init::filesystem::append(".gitignore", "\nbuild");
  } else {
    cmake_init::filesystem::write(".gitignore", R"(build

#region https://github.com/github/gitignore/blob/main/CMake.gitignore
CMakeLists.txt.user
CMakeCache.txt
CMakeFiles
CMakeScripts
Testing
Makefile
cmake_install.cmake
install_manifest.txt
compile_commands.json
CTestTestfile.cmake
_deps
CMakeUserPresets.json
#endregion)");
  }

  if (std::filesystem::exists("CMakeLists.txt")) {
    throw std::runtime_error("CMakeLists.txt already exists");
  }
  if (std::filesystem::exists("task.cmake")) {
    throw std::runtime_error("task.cmake already exists");
  }
  if (std::filesystem::exists("CMakePresets.json")) {
    throw std::runtime_error("CMakePresets.json already exists");
  }

  nlohmann::json data;
  data["bin"] = bin;
  data["lib"] = lib;
  data["cxx"] = cxx;
  data["c"] = c;
  data["name"] = name;
  data["c_std"] = cmake_init::to_cmake_feature_string(c_standard);
  data["cxx_std"] = cmake_init::to_cmake_feature_string(cxx_standard);
  cmake_init::filesystem::write(
      "CMakeLists.txt", inja::render(R"(cmake_minimum_required(VERSION 3.29)

# Project
project(
  {{ name }}
  VERSION 0.1.0
  LANGUAGES {% if cxx %}CXX{% else %}C{% endif %})

# Project dependencies
include(FetchContent)

{% if lib %}
# Libraries
add_library({{ name }})
target_sources({{ name }} PRIVATE {% if cxx %}src/lib.cpp{% else %}src/lib.c{% endif %})
target_include_directories({{ name }} PUBLIC include)
target_compile_features({{ name }} PRIVATE {% if cxx %}{{ cxx_std }}{% else %}{{ c_std }}{% endif %})
{% else %}
# Binaries
add_executable({{ name }})
target_sources({{ name }} PRIVATE {% if cxx %}src/main.cpp{% else %}src/main.c{% endif %})
target_compile_features({{ name }} PRIVATE {% if cxx %}{{ cxx_std }}{% else %}{{ c_std }}{% endif %})
{% endif %}

# Testing
include(CTest)
if(BUILD_TESTING)
  # TODO
endif()

# Installation info
include(GNUInstallDirs)
include(CPack)
install(TARGETS {{ name }})

# Tasks
add_custom_target(
  format
  COMMAND
    "${CMAKE_COMMAND}" -DTASK_NAME=format
    "-DTASK_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DTASK_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cmake"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
)",
                                     data));

  cmake_init::filesystem::write("task.cmake", R"(#!/usr/bin/env -S cmake -P
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
)");

  cmake_init::filesystem::write("CMakePresets.json", R"({
  "version": 8,
  "cmakeMinimumRequired": { "major": 3, "minor": 29, "patch": 0 },
  "configurePresets": [
    {
      "name": "default",
      "binaryDir": "build"
    }
  ],
  "buildPresets": [
    { "name": "default", "configurePreset": "default" },
    { "name": "format", "configurePreset": "default", "targets": ["format"] },
    { "name": "lint", "configurePreset": "default", "targets": ["lint"] }
  ],
  "workflowPresets": [
    {
      "name": "default",
      "steps": [
        { "type": "configure", "name": "default" },
        { "type": "build", "name": "default" }
      ]
    },
    {
      "name": "format",
      "steps": [
        { "type": "configure", "name": "default" },
        { "type": "build", "name": "format" }
      ]
    },
    {
      "name": "lint",
      "steps": [
        { "type": "configure", "name": "default" },
        { "type": "build", "name": "lint" }
      ]
    }
  ]
}
)");

  if (bin) {
    if (cxx && !std::filesystem::exists("src/main.cpp")) {
      std::filesystem::create_directories("src");
      cmake_init::filesystem::write("src/main.cpp", R"(#include <iostream>

int main() {
  std::cout << "Hello world!\n";
  return 0;
}
)");
    } else if (c && !std::filesystem::exists("src/main.c")) {
      std::filesystem::create_directories("src");
      cmake_init::filesystem::write("src/main.c", R"(#include <stdio.h>

int main() {
  puts("Hello world!");
  return 0;
}
)");
    }
  } else if (lib) {
    if (cxx) {
      if (!std::filesystem::exists("src/lib.cpp")) {
        std::filesystem::create_directories("src");
        cmake_init::filesystem::write("src/lib.cpp", R"()");
      }
      if (!std::filesystem::exists("src/lib.h")) {
        std::filesystem::create_directories("src");
        cmake_init::filesystem::write("src/lib.h", R"()");
      }
      if (!std::filesystem::exists(fmt::format("include/{}.h", name))) {
        std::filesystem::create_directories("include");
        cmake_init::filesystem::write(fmt::format("include/{}.h", name),
                                      std::string("#pragma once\n#include <") +
                                          name + ">\n");
      }
    } else if (c) {
      if (!std::filesystem::exists("src/lib.c")) {
        std::filesystem::create_directories("src");
        cmake_init::filesystem::write("src/lib.c", R"()");
      }
      if (!std::filesystem::exists("src/lib.h")) {
        std::filesystem::create_directories("src");
        cmake_init::filesystem::write("src/lib.h", R"()");
      }
      if (!std::filesystem::exists(fmt::format("include/{}.h", name))) {
        std::filesystem::create_directories("include");
        cmake_init::filesystem::write(fmt::format("include/{}.h", name),
                                      std::string("#pragma once\n#include <") +
                                          name + ">\n");
      }
    }
  }

  fmt::print(fg(fmt::color::green), "Successfully generated!\n");
  fmt::println("Run `cmake --workflow --preset default` to get started");

  return 0;
}
