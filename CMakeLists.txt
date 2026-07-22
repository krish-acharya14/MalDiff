cmake_minimum_required(VERSION 3.21)

project(MalwareEvolutionDiffEngine
    VERSION 0.1.0
    DESCRIPTION "MalDiff- Phase 1: Project Foundation & Architecture"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "MalwareEvolutionDiffEngine version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(MEDE_BUILD_TESTS "Build unit tests" ON)
option(MEDE_ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

add_library(mede_compiler_flags INTERFACE)
target_compile_features(mede_compiler_flags INTERFACE cxx_std_20)

set(MSVC_WARNINGS /W4 /permissive-)
set(CLANG_GCC_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
)

if(MEDE_ENABLE_WARNINGS_AS_ERRORS)
    list(APPEND CLANG_GCC_WARNINGS -Werror)
    list(APPEND MSVC_WARNINGS /WX)
endif()

target_compile_options(mede_compiler_flags INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:${CLANG_GCC_WARNINGS}>
)

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(src)

if(MEDE_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    add_subdirectory(tests)
endif()

install(TARGETS mede_cli RUNTIME DESTINATION bin)
